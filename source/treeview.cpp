/*
 * treeview.cpp
 *
 *  Created on: 4 Jun 2013
 *      Author: nick

Copyright (c) 2013, dharc ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
 */

#include "treeview.h"

#include <dsb/nid.h>
#include <dsb/iterator.h>
#include <dsb/wrap.h>
#include <qt4/QtGui/QTreeWidget>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QToolBar>
#include <qt4/QtGui/QLineEdit>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QAction>

Q_DECLARE_METATYPE(NID);

AddObject::AddObject()
	: QWidget(0,Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_attr = new QLineEdit();
	m_attr->setAutoFillBackground(true);
	mainlayout->addWidget(m_attr);

	m_value = new QLineEdit();
	m_value->setAutoFillBackground(true);
	mainlayout->addWidget(m_value);

	QHBoxLayout *buttonlayout = new QHBoxLayout();
	mainlayout->addLayout(buttonlayout);
	m_save = new QPushButton("Add");
	buttonlayout->addWidget(m_save);
	m_cancel = new QPushButton("Cancel");
	buttonlayout->addWidget(m_cancel);

	setWindowTitle("Add Hyperarc");

	connect(m_save, SIGNAL(clicked()), this, SLOT(addclicked()));
	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));

	m_item = 0;
}

AddObject::~AddObject()
{

}

void AddObject::showAddObject(QTreeWidgetItem *item)
{
	m_item = item;
	show();
}

void AddObject::addclicked()
{
	NID_t root;
	NID_t key;
	NID_t value;

	root = m_item->data(1,Qt::UserRole).value<NID>();
	dsb_nid_fromStr(m_attr->text().toAscii().constData(),&key);

	if (m_value->text() == "new")
	{
		dsb_new(&root,&value);
	}
	else
	{
		dsb_nid_fromStr(m_value->text().toAscii().constData(),&value);
	}

	dsb_set(&root,&key,&value);
	dsb_dict(&root,&key);

	hide();
}

void AddObject::cancelclicked()
{
	hide();
}

//---------------------

EditObject::EditObject()
	: QWidget(0,Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_value = new QLineEdit();
	m_value->setAutoFillBackground(true);
	mainlayout->addWidget(m_value);

	QHBoxLayout *buttonlayout = new QHBoxLayout();
	mainlayout->addLayout(buttonlayout);
	m_save = new QPushButton("Edit");
	buttonlayout->addWidget(m_save);
	m_cancel = new QPushButton("Cancel");
	buttonlayout->addWidget(m_cancel);

	setWindowTitle("Add Hyperarc");

	connect(m_save, SIGNAL(clicked()), this, SLOT(editclicked()));
	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));

	m_item = 0;
}

EditObject::~EditObject()
{

}

void EditObject::showEditObject(QTreeWidgetItem *item)
{
	m_item = item;
	m_value->setText(m_item->text(1));
	show();
}

void EditObject::editclicked()
{
	NID_t root;
	NID_t key;
	NID_t value;

	root = m_item->parent()->data(1,Qt::UserRole).value<NID>();
	key = m_item->data(0,Qt::UserRole).value<NID>();

	if (m_value->text() == "new")
	{
		dsb_new(&root,&value);
	}
	else
	{
		dsb_nid_fromStr(m_value->text().toAscii().constData(),&value);
	}

	dsb_set(&root,&key,&value);

	hide();
}

void EditObject::cancelclicked()
{
	hide();
}

//--------------------------------

TreeView::TreeView()
 : QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	make_toolbar(mainlayout);

	m_tree = new QTreeWidget();
	m_tree->setColumnCount(2);
	m_tree->setHeaderLabels(QStringList(QString("Object")) << QString("Value"));
	m_tree->setColumnWidth(0,250);
	mainlayout->addWidget(m_tree);

	m_addobj = new AddObject();
	m_editobj = new EditObject();

	setWindowTitle("Tree Browser");
	//resize(600,400);
	show();

	connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(collapsed(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(expanded(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(doubleclick(QTreeWidgetItem*,int)));
}

TreeView::~TreeView()
{

}

void TreeView::make_toolbar(QLayout *l)
{
	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/add.png"),"Add");
	m_bar->addAction(QIcon(":/icons/delete.png"),"Delete");
	m_bar->addAction(QIcon(":/icons/pencil.png"),"Edit");
	l->addWidget(m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
}

void TreeView::toolclick(QAction *a)
{
	if (a->text() == "Add")
	{
		m_addobj->showAddObject(m_tree->currentItem());
	}
}

void TreeView::doubleclick(QTreeWidgetItem *item, int col)
{
	if (item != 0)
	{
		m_editobj->showEditObject(item);
	}
}

void TreeView::setRoot(const NID_t &root)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *item2;

	item = new QTreeWidgetItem();
	item->setText(0,"root");
	item->setIcon(0,QIcon(":/icons/house.png"));
	item->setData(1,Qt::UserRole,QVariant::fromValue(root));
	m_tree->insertTopLevelItem(0,item);

	struct DSBIterator it;
	const NID_t *p;
	dsb_iterator_begin(&it,&root);
	p = dsb_iterator_next(&it);
	while (p != 0)
	{
		char buf[100];
		NID_t v;
		dsb_nid_toStr(p,buf,100);
		item2 = new QTreeWidgetItem(item);
		item2->setText(0,buf);
		item2->setData(0,Qt::UserRole,QVariant::fromValue(*p));
		p = dsb_iterator_next(&it);
	}
	dsb_iterator_end(&it);
}

void TreeView::expanded(QTreeWidgetItem *item)
{
	QTreeWidgetItem *item2;
	QTreeWidgetItem *item3;
	NID_t key;
	NID_t root;
	NID_t value;
	char buf[100];

	root = item->data(1,Qt::UserRole).value<NID>();

	//Need to loop over all children and add their children
	for (int i=0; i<item->childCount(); i++)
	{
		item2 = item->child(i);
		key = item2->data(0,Qt::UserRole).value<NID>();

		dsb_get(&root,&key,&value);
		dsb_nid_toStr(&value,buf,100);
		item2->setText(1,buf);
		item2->setData(1,Qt::UserRole,QVariant::fromValue(value));

		//Now generate partial children.
		struct DSBIterator it;
		const NID_t *p;
		dsb_iterator_begin(&it,&value);
		p = dsb_iterator_next(&it);
		while (p != 0)
		{
			dsb_nid_toStr(p,buf,100);
			item3 = new QTreeWidgetItem(item2);
			item3->setText(0,buf);
			item3->setData(0,Qt::UserRole,QVariant::fromValue(*p));
			p = dsb_iterator_next(&it);
		}
		dsb_iterator_end(&it);

		//Choose icon based upon values type.
		if (value.hasMac == 1)
		{
			if (item2->childCount() == 0)
			{
				item2->setIcon(0,QIcon(":/icons/tag_red.png"));
			}
			else
			{
				QFont tmpfont = item2->font(1);
				tmpfont.setStyle(QFont::StyleItalic);
				item2->setIcon(0,QIcon(":/icons/folder.png"));
				item2->setFont(1,tmpfont);
				item2->setForeground(1,QBrush(QColor("grey")));
			}
		}
		else
		{
			switch(value.t)
			{
			case NID_INTEGER:	item2->setIcon(0,QIcon(":/icons/tag_blue.png"));
								item2->setForeground(1,QBrush(QColor("blue")));
								break;
			case NID_SPECIAL:	item2->setIcon(0,QIcon(":/icons/tag_green.png")); break;
			default: break;
			}
		}
	}
}

void TreeView::collapsed(QTreeWidgetItem *item)
{
	QTreeWidgetItem *item2;

	for (int i=0; i<item->childCount(); i++)
	{
		item2 = item->child(i);
		item2->setExpanded(false);
		QList<QTreeWidgetItem*> children = item2->takeChildren();

		for (int j=0; j<children.count(); j++)
		{
			delete children.at(j);
		}
	}
}


