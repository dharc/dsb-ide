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
#include "dsb/ide/ide.h"

#include <dsb/nid.h>
#include <dsb/iterator.h>
#include <dsb/wrap.h>
#include <dsb/pattern.h>
#include <dsb/pattern_types.h>
#include <dsb/vm.h>
#include <dsb/globals.h>
#include <qt4/QtGui/QTreeWidget>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QToolBar>
#include <qt4/QtGui/QLineEdit>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QAction>
#include <qt4/QtGui/QMenu>
#include <qt4/QtGui/QHeaderView>

Q_DECLARE_METATYPE(NID);

extern DSBIde *ide;

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
	m_tree->header()->close();
	m_tree->setColumnWidth(0,220);
	m_tree->setIconSize(QSize(24,24));
	m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
	m_tree->setDragDropMode(QAbstractItemView::DragOnly);
	connect(m_tree,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));
	//m_tree->setAlternatingRowColors(true);
	mainlayout->addWidget(m_tree);

	m_treemenu = new QMenu();
	m_treemenu->addAction("View");
	m_treemenu->addMenu("View As");
	m_treemenu->addAction("Edit");
	m_treemenu->addAction("Edit Definition");
	m_treemenu->addAction("Insert");
	m_treemenu->addAction("Delete");
	connect(m_treemenu, SIGNAL(triggered(QAction*)), this, SLOT(menuclick(QAction*)));

	m_addobj = new AddObject();
	m_editobj = new EditObject();

	setWindowTitle("Tree Browser");
	//resize(600,400);
	show();

	connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(collapsed(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(expanded(QTreeWidgetItem *)));
	//connect(m_tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(doubleclick(QTreeWidgetItem*,int)));
}

TreeView::~TreeView()
{

}

void TreeView::make_toolbar(QLayout *l)
{
	m_bar = new QToolBar();
	//m_acts[ACTION_NONE] = m_bar->addAction(QIcon(":/icons/cursor.png"),"None");
	m_acts[ACTION_ADD] = m_bar->addAction(QIcon(":/icons/chart_organisation_add.png"),"Add");
	m_acts[ACTION_DELETE] = m_bar->addAction(QIcon(":/icons/chart_organisation_delete.png"),"Delete");
	m_acts[ACTION_EDIT] = m_bar->addAction(QIcon(":/icons/pencil.png"),"Edit");
	m_acts[ACTION_EDITDEF] = m_bar->addAction(QIcon(":/icons/script_edit.png"),"Edit Definition");
	m_acts[ACTION_VIEW] = m_bar->addAction(QIcon(":/icons/eye.png"),"View");
	m_acts[ACTION_CLONE] = m_bar->addAction(QIcon(":/icons/page_copy.png"),"Clone");
	m_acts[ACTION_MOVE] = m_bar->addAction(QIcon(":/icons/cut.png"),"Move");
	m_acts[ACTION_LINK] = m_bar->addAction(QIcon(":/icons/link.png"),"Connect");
	m_acts[ACTION_HELP] = m_bar->addAction(QIcon(":/icons/help.png"),"Help");

	//l->addWidget(m_bar);
	ide->addToolBar(Qt::LeftToolBarArea,m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
}

void TreeView::toolclick(QAction *a)
{
	unsigned int act = 0;
	QTreeWidgetItem *curitem = m_tree->currentItem();

	if (curitem == 0) return;

	//Find which action it is.
	for (act = 0; act<ACTION_END; act++)
	{
		if (m_acts[act] == a) break;
	}

	if (act == ACTION_ADD)
	{
		m_addobj->showAddObject(curitem);
	}
	else if (act == ACTION_EDITDEF)
	{
		if (curitem->parent() == 0) return;
		NID_t def;
		NID_t obj = curitem->parent()->data(1,Qt::UserRole).value<NID>();
		NID_t key = curitem->data(0,Qt::UserRole).value<NID>();
		int eval;
		dsb_getdef(&obj,&key,&def,&eval);

		//If no definition then make one.
		if (dsb_nid_eq(&def,&Null) == 1)
		{
			NID_t vmop;
			dsb_nid(NID_VMOP,VM_RET(0),&vmop);
			dsb_new(&PRoot,&def);
			dsb_setnni(&def,&Size,1);
			dsb_setnin(&def,0,&vmop);
			dsb_define(&obj,&key,&def,2);
		}

		ide->newView(obj,key,def);
	}
	else if (act == ACTION_VIEW)
	{
		if (curitem->parent() == 0) return;
		//m_editobj->showEditObject(item);
		NID_t d1 = curitem->parent()->data(1,Qt::UserRole).value<NID>();
		NID_t d2 = curitem->data(1,Qt::UserRole).value<NID>();
		NID_t v = curitem->data(1,Qt::UserRole).value<NID>();
		ide->newView(d1,d2,v);
	}

	//if (a->text() == "Add")
	//{
	//	m_addobj->showAddObject(m_tree->currentItem());
	//}
}

void TreeView::menuclick(QAction *a)
{
	if (m_menuitem == 0) return;

	if (a->text() == "Edit Definition")
	{
		if (m_menuitem->parent() == 0) return;

		NID_t def;
		NID_t obj = m_menuitem->parent()->data(1,Qt::UserRole).value<NID>();
		NID_t key = m_menuitem->data(0,Qt::UserRole).value<NID>();
		int eval;
		dsb_getdef(&obj,&key,&def,&eval);

		//If no definition then make one.
		if (dsb_nid_eq(&def,&Null) == 1)
		{
			NID_t vmop;
			dsb_nid(NID_VMOP,VM_RET(0),&vmop);
			dsb_new(&PRoot,&def);
			dsb_setnni(&def,&Size,1);
			dsb_setnin(&def,0,&vmop);

			dsb_define(&obj,&key,&def,2);
		}

		ide->newView(obj,key,def);
	}
}

void TreeView::doubleclick(QTreeWidgetItem *item, int col)
{
	/*if (item != 0)
	{
		if (item->parent() == 0) return;
		//m_editobj->showEditObject(item);
		NID_t d1 = item->parent()->data(1,Qt::UserRole).value<NID>();
		NID_t d2 = item->data(1,Qt::UserRole).value<NID>();
		NID_t v = item->data(1,Qt::UserRole).value<NID>();
		ide->newView(d1,d2,v);
	}*/
}

void TreeView::showContextMenu(const QPoint &pnt)
{
	QPoint globalPos = m_tree->mapToGlobal(pnt);
	m_menuitem = m_tree->itemAt(pnt);
	m_treemenu->exec(globalPos);
}

void TreeView::setRoot(const NID_t &root, QString &name)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *item2;

	item = new QTreeWidgetItem();
	item->setText(0,name);
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

void TreeView::clear()
{
	m_tree->clear();
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

		int type;
		int eval;
		bool definition = false;

		type = dsb_pattern_what(&value);
		dsb_getdef(&root,&key,&value,&eval);

		if (dsb_pattern_isA(&value,DSB_PATTERN_BYTECODE))
		{
			item2->setForeground(0,QBrush(QColor("blue")));
			definition = true;
		}

		switch (type)
		{
		case DSB_PATTERN_INTEGER:	if (definition)
									{
										item2->setIcon(0,QIcon(":/icons/blob-int-blue.png"));
									}
									else
									{
										item2->setIcon(0,QIcon(":/icons/blob-int.png"));
									}
									item2->setForeground(1,QBrush(QColor("blue")));
									break;
		case DSB_PATTERN_BOOLEAN:	item2->setIcon(0,QIcon(":/icons/blob-bool.png"));
									{
										QFont tmpfont = item2->font(1);
										tmpfont.setBold(true);
										item2->setFont(1,tmpfont);
									}
									break;
		default:					if (item2->childCount() == 0)
									{
										item2->setIcon(0,QIcon(":/icons/blob-label.png"));
									}
									else
									{
										QFont tmpfont = item2->font(1);
										tmpfont.setStyle(QFont::StyleItalic);
										item2->setIcon(0,QIcon(":/icons/blob-harc.png"));
										item2->setFont(1,tmpfont);
										item2->setForeground(1,QBrush(QColor("grey")));
									}
									break;
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


