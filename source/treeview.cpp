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

Q_DECLARE_METATYPE(NID);

TreeView::TreeView()
 : QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_tree = new QTreeWidget();
	m_tree->setColumnCount(2);
	m_tree->setHeaderLabels(QStringList(QString("Object")) << QString("Value"));
	mainlayout->addWidget(m_tree);

	setWindowTitle("Tree Browser");
	resize(600,400);
	show();

	connect(m_tree,SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(collapsed(QTreeWidgetItem *)));
	connect(m_tree,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(expanded(QTreeWidgetItem *)));
}

TreeView::~TreeView()
{

}

void TreeView::setRoot(const NID_t &root)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *item2;

	item = new QTreeWidgetItem();
	item->setText(0,"root");
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
	}
}

void TreeView::collapsed(QTreeWidgetItem *item)
{

}


