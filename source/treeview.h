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

#include <QWidget>
#include <QDialog>

typedef struct NID NID_t;
class QTreeWidget;
class QTreeWidgetItem;
class QToolBar;
class QLineEdit;
class QPushButton;
class QMenu;
class QPoint;
class QString;

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

class PasteObject : public QWidget
{
	Q_OBJECT

public:
	PasteObject();
	~PasteObject();

	void showPasteObject(QTreeWidgetItem *src, QTreeWidgetItem *dest, unsigned int copyaction);

private:
	QLineEdit *m_value;
	QPushButton *m_save;
	QPushButton *m_cancel;
	QTreeWidgetItem *m_src;
	QTreeWidgetItem *m_dest;
	unsigned int m_copyact;

public slots:
	void pasteclicked();
	void cancelclicked();
};

class TreeView : public QWidget
{
	Q_OBJECT

public:
	TreeView();
	~TreeView();

	static const unsigned int ACTION_NONE = 0;
	static const unsigned int ACTION_ADD = 1;
	static const unsigned int ACTION_DELETE = 2;
	static const unsigned int ACTION_EDIT = 3;
	static const unsigned int ACTION_EDITDEF = 4;
	static const unsigned int ACTION_VIEW = 5;
	static const unsigned int ACTION_CLONE = 6;
	static const unsigned int ACTION_MOVE = 7;
	static const unsigned int ACTION_LINK = 8;
	static const unsigned int ACTION_HELP = 9;
	static const unsigned int ACTION_HOME = 10;
	static const unsigned int ACTION_PASTE = 11;
	static const unsigned int ACTION_COPYREF = 12;
	static const unsigned int ACTION_SHALLOW = 13;
	static const unsigned int ACTION_DEEP = 14;
	static const unsigned int ACTION_END = 15;

	void setRoot(const NID_t &n, QString &name, bool comp);
	void clear();
	void updateItemDetails(QTreeWidgetItem *item);
	void addHiddenChildren(QTreeWidgetItem *item, const NID &base);
	void clearChildren(QTreeWidgetItem *item);

private:

	QTreeWidget *m_tree;
	QToolBar *m_bar;
	AddObject *m_addobj;
	EditObject *m_editobj;
	PasteObject *m_pastediag;
	QMenu *m_treemenu;
	QTreeWidgetItem *m_menuitem;
	QTreeWidgetItem *m_copyitem;
	unsigned int m_copyact;

	QAction *m_acts[ACTION_END];
	unsigned int m_curact;

	void make_toolbar(QLayout *);

	void refreshItem(QTreeWidgetItem *item);
	void paste(QTreeWidgetItem *source, QTreeWidgetItem *dest, unsigned int copyaction);

public slots:
	void expanded(QTreeWidgetItem *item);
	void collapsed(QTreeWidgetItem *item);
	void toolclick(QAction *);
	void menuclick(QAction *);
	void doubleclick(QTreeWidgetItem *item, int col);
	void showContextMenu(const QPoint &pnt);
	void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *prev);
};

#endif /* TREEVIEW_H_ */
