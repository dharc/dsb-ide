/*
 * treeview.h
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

#ifndef TREEVIEW_H_
#define TREEVIEW_H_

#include <qt4/QtGui/QWidget>

typedef struct NID NID_t;
class QTreeWidget;
class QTreeWidgetItem;
class QToolBar;
class QLineEdit;
class QPushButton;

class AddObject : public QWidget
{
	Q_OBJECT

public:
	AddObject();
	~AddObject();

	void showAddObject(QTreeWidgetItem *item);

private:
	QLineEdit *m_attr;
	QLineEdit *m_value;
	QPushButton *m_save;
	QPushButton *m_cancel;
	QTreeWidgetItem *m_item;

public slots:
	void addclicked();
	void cancelclicked();
};

class EditObject : public QWidget
{
	Q_OBJECT

public:
	EditObject();
	~EditObject();

	void showEditObject(QTreeWidgetItem *item);

private:
	QLineEdit *m_value;
	QPushButton *m_save;
	QPushButton *m_cancel;
	QTreeWidgetItem *m_item;

public slots:
	void editclicked();
	void cancelclicked();
};

class TreeView : public QWidget
{
	Q_OBJECT

public:
	TreeView();
	~TreeView();

	void setRoot(const NID_t &n);

private:
	QTreeWidget *m_tree;
	QToolBar *m_bar;
	AddObject *m_addobj;
	EditObject *m_editobj;

	void make_toolbar(QLayout *);

public slots:
	void expanded(QTreeWidgetItem *item);
	void collapsed(QTreeWidgetItem *item);
	void toolclick(QAction *);
	void doubleclick(QTreeWidgetItem *item, int col);
};

#endif /* TREEVIEW_H_ */
