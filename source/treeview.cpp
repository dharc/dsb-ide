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
#include <dsb/clone.h>
#include <qt4/QtGui/QTreeWidget>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QToolBar>
#include <qt4/QtGui/QLineEdit>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QAction>
#include <qt4/QtGui/QMenu>
#include <qt4/QtGui/QHeaderView>

#include <iostream>

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

	ide->tree()->clearChildren(m_item);
	ide->tree()->addHiddenChildren(m_item,root);

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

	setWindowTitle("Edit Value");

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
	m_value->selectAll();
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

	bool wasexpanded = m_item->isExpanded();
	ide->tree()->updateItemDetails(m_item);
	if (wasexpanded) m_item->setExpanded(true);

	hide();
}

void PasteObject::cancelclicked()
{
	hide();
}

//---------------------

PasteObject::PasteObject()
	: QWidget(0,Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_value = new QLineEdit();
	m_value->setAutoFillBackground(true);
	mainlayout->addWidget(m_value);

	QHBoxLayout *buttonlayout = new QHBoxLayout();
	mainlayout->addLayout(buttonlayout);
	m_save = new QPushButton("Paste");
	buttonlayout->addWidget(m_save);
	m_cancel = new QPushButton("Cancel");
	buttonlayout->addWidget(m_cancel);

	setWindowTitle("Paste Object");

	connect(m_save, SIGNAL(clicked()), this, SLOT(pasteclicked()));
	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));

	m_src = 0;
	m_dest = 0;
	m_copyact = 0;
}

PasteObject::~PasteObject()
{

}

void PasteObject::showPasteObject(QTreeWidgetItem *src, QTreeWidgetItem *dest, unsigned int copyact)
{
	m_src = src;
	m_dest = dest;
	m_copyact = copyact;
	m_value->setText(m_src->text(0));
	m_value->selectAll();
	show();
}

void PasteObject::pasteclicked()
{
	NID_t tmp;
	NID_t dobj;
	NID_t key;
	NID_t val;

	//Get the relevant NIDs from the items.
	dobj = m_dest->data(1,Qt::UserRole).value<NID>();
	//key = m_src->data(0,Qt::UserRole).value<NID>();
	dsb_nid_fromStr(m_value->text().toAscii().constData(),&key);
	val = m_src->data(1,Qt::UserRole).value<NID>();

	//Check for an existing key being overwritten.
	dsb_get(&dobj,&key,&tmp);
	if (dsb_nid_eq(&tmp,&Null) == 0)
	{
		//RENAME
	}

	if (m_copyact == TreeView::ACTION_COPYREF)
	{

		//OK to copy reference.
		dsb_set(&dobj,&key,&val);
		dsb_dict(&dobj,&key);
	}
	else if (m_copyact == TreeView::ACTION_SHALLOW)
	{
		//Clone first then put new reference in...
		dsb_new(&dobj,&tmp);
		dsb_clone_shallow(&val,&tmp);
		val = tmp;
		dsb_set(&dobj,&key,&tmp);
		dsb_dict(&dobj,&key);
	}

	//dest->setExpanded(false);

	bool wasexpanded = m_dest->isExpanded();

	ide->tree()->clearChildren(m_dest);
	ide->tree()->addHiddenChildren(m_dest,dobj);

	if (wasexpanded) m_dest->setExpanded(true);

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
	m_pastediag = new PasteObject();

	m_copyitem = 0;

	setWindowTitle("Tree Browser");
	//resize(600,400);
	show();

	connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(collapsed(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(expanded(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
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
	m_acts[ACTION_PASTE] = m_bar->addAction(QIcon(":/icons/page_paste.png"),"Paste");
	m_acts[ACTION_LINK] = m_bar->addAction(QIcon(":/icons/link.png"),"Connect");
	m_acts[ACTION_HELP] = m_bar->addAction(QIcon(":/icons/help.png"),"Help");
	m_acts[ACTION_HOME] = m_bar->addAction(QIcon(":/icons/house.png"),"Add Root");

	QMenu *menu;

	menu = new QMenu();
	menu->addAction("Default View");
	menu->addAction("View As");
	m_acts[ACTION_VIEW]->setMenu(menu);

	menu = new QMenu();
	menu->addAction("Default Editor");
	menu->addAction("Assembler");
	m_acts[ACTION_EDITDEF]->setMenu(menu);

	menu = new QMenu();
	m_acts[ACTION_COPYREF] = menu->addAction("Reference");
	m_acts[ACTION_SHALLOW] = menu->addAction("Shallow Clone");
	m_acts[ACTION_DEEP] = menu->addAction("Deep Clone");
	m_acts[ACTION_CLONE]->setMenu(menu);

	//l->addWidget(m_bar);
	ide->addToolBar(Qt::LeftToolBarArea,m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
	//connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(toolclick(QAction*)));
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
	else if (act == ACTION_EDIT)
	{
		m_editobj->showEditObject(curitem);
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
		NID_t d2 = curitem->data(0,Qt::UserRole).value<NID>();
		NID_t v = curitem->data(1,Qt::UserRole).value<NID>();
		ide->newView(d1,d2,v);
	}
	else if (act == ACTION_HOME)
	{
		NID_t r = curitem->data(1,Qt::UserRole).value<NID>();
		NID_t k = curitem->data(0,Qt::UserRole).value<NID>();
		char buf[100];
		dsb_nid_toStr(&k,buf,100);
		QString str(buf);
		setRoot(r,str,false);
	}
	else if (act == ACTION_PASTE)
	{
		paste(m_copyitem, curitem, m_copyact);
	}
	else if (act == ACTION_CLONE)
	{
		m_copyact = ACTION_COPYREF;
		m_copyitem = curitem;
		m_acts[ACTION_PASTE]->setEnabled(true);
	}
	else if (act == ACTION_COPYREF || act == ACTION_SHALLOW || act == ACTION_DEEP)
	{
		m_copyact = act;
		m_copyitem = curitem;
		m_acts[ACTION_PASTE]->setEnabled(true);
	}

	//if (a->text() == "Add")
	//{
	//	m_addobj->showAddObject(m_tree->currentItem());
	//}
}

void TreeView::paste(QTreeWidgetItem *source, QTreeWidgetItem *dest, unsigned int copyaction)
{
	m_pastediag->showPasteObject(source,dest,copyaction);
}

void TreeView::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev)
{
	//Decide what actions are valid for this item.

	if (current != 0)
	{
		m_acts[ACTION_ADD]->setEnabled(true);
		m_acts[ACTION_DELETE]->setEnabled(true);

		if (m_copyitem == 0)
		{
			m_acts[ACTION_PASTE]->setEnabled(false);
		}
		else
		{
			m_acts[ACTION_PASTE]->setEnabled(true);
		}

		//Are we a root.
		if (current->parent() == 0)
		{
			m_acts[ACTION_HOME]->setEnabled(false);
			m_acts[ACTION_EDIT]->setEnabled(false);
			m_acts[ACTION_EDITDEF]->setEnabled(false);
			m_acts[ACTION_VIEW]->setEnabled(false);
			m_acts[ACTION_MOVE]->setEnabled(false);
		}
		else
		{
			NID_t obj = current->parent()->data(1,Qt::UserRole).value<NID>();
			NID_t key = current->data(0,Qt::UserRole).value<NID>();
			NID_t def;
			int eval;

			m_acts[ACTION_VIEW]->setEnabled(true);
			m_acts[ACTION_MOVE]->setEnabled(true);

			dsb_getdef(&obj,&key,&def,&eval);

			if (dsb_pattern_isA(&def,DSB_PATTERN_BYTECODE))
			{
				m_acts[ACTION_EDIT]->setEnabled(false);
				m_acts[ACTION_EDITDEF]->setEnabled(true);
				m_acts[ACTION_EDITDEF]->setIcon(QIcon(":/icons/script_edit.png"));
			}
			else
			{
				m_acts[ACTION_EDIT]->setEnabled(true);
				m_acts[ACTION_EDITDEF]->setEnabled(true);
				m_acts[ACTION_EDITDEF]->setIcon(QIcon(":/icons/script_add.png"));
			}

			m_acts[ACTION_HOME]->setEnabled(true);
		}
	}
	else
	{
		for (unsigned int i=1; i<ACTION_END; i++)
		{
			m_acts[i]->setEnabled(false);
		}
	}
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

void TreeView::setRoot(const NID_t &root, QString &name, bool comp)
{
	QTreeWidgetItem *item;

	item = new QTreeWidgetItem();
	item->setText(0,name);
	if (comp)
	{
		item->setIcon(0,QIcon(":/icons/computer.png"));
	}
	else
	{
		item->setIcon(0,QIcon(":/icons/blob-harc.png"));
	}
	item->setData(1,Qt::UserRole,QVariant::fromValue(root));
	m_tree->insertTopLevelItem(0,item);

	addHiddenChildren(item,root);
}

void TreeView::addHiddenChildren(QTreeWidgetItem *item, const NID &base)
{
	QTreeWidgetItem *item2;
	struct DSBIterator it;
	const NID_t *p;
	dsb_iterator_begin(&it,&base);
	p = dsb_iterator_next(&it);
	while (p != 0)
	{
		//Make a new sparse tree item for each child
		char buf[100];
		dsb_nid_pretty(p,buf,100);
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

void TreeView::updateItemDetails(QTreeWidgetItem *item)
{
	char buf[100];
	NID_t key;
	NID_t value;
	NID_t root = item->parent()->data(1,Qt::UserRole).value<NID>();
	key = item->data(0,Qt::UserRole).value<NID>();

	//Update the items value.
	dsb_get(&root,&key,&value);
	dsb_nid_pretty(&value,buf,100);
	item->setText(1,buf);
	item->setData(1,Qt::UserRole,QVariant::fromValue(value));

	//Now generate partial children.
	clearChildren(item);
	addHiddenChildren(item,value);

	int type;
	int eval;
	bool definition = false;

	type = dsb_pattern_what(&value);
	dsb_getdef(&root,&key,&value,&eval);

	if (dsb_pattern_isA(&value,DSB_PATTERN_BYTECODE))
	{
		item->setForeground(0,QBrush(QColor("blue")));
		definition = true;
	}

	switch (type)
	{
	case DSB_PATTERN_INTEGER:	if (definition)
								{
									item->setIcon(0,QIcon(":/icons/blob-int-blue.png"));
								}
								else
								{
									item->setIcon(0,QIcon(":/icons/blob-int.png"));
								}
								item->setForeground(1,QBrush(QColor("blue")));
								break;
	case DSB_PATTERN_BOOLEAN:	item->setIcon(0,QIcon(":/icons/blob-bool.png"));
								{
									QFont tmpfont = item->font(1);
									tmpfont.setBold(true);
									item->setFont(1,tmpfont);
								}
								break;
	default:					if (item->childCount() == 0)
								{
									item->setIcon(0,QIcon(":/icons/blob-label.png"));
								}
								else
								{
									QFont tmpfont = item->font(1);
									tmpfont.setStyle(QFont::StyleItalic);
									item->setIcon(0,QIcon(":/icons/blob-harc.png"));
									item->setFont(1,tmpfont);
									item->setForeground(1,QBrush(QColor("grey")));
								}
								break;
	}
}

void TreeView::refreshItem(QTreeWidgetItem *item)
{
	QTreeWidgetItem *item2;
	NID_t root;

	root = item->data(1,Qt::UserRole).value<NID>();

	//Need to loop over all children and add their children
	for (int i=0; i<item->childCount(); i++)
	{
		item2 = item->child(i);
		updateItemDetails(item2);
	}
}

void TreeView::expanded(QTreeWidgetItem *item)
{
	refreshItem(item);
}

void TreeView::collapsed(QTreeWidgetItem *item)
{
	QTreeWidgetItem *item2;

	for (int i=0; i<item->childCount(); i++)
	{
		item2 = item->child(i);
		clearChildren(item2);
	}
}

void TreeView::clearChildren(QTreeWidgetItem *item)
{
	item->setExpanded(false);
	QList<QTreeWidgetItem*> children = item->takeChildren();

			for (int j=0; j<children.count(); j++)
	{
		delete children.at(j);
	}
}


