/*
 * ide.h
 *
 *  Created on: 14 Jun 2013
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

#ifndef IDE_H_
#define IDE_H_

#include <QMainWindow>

class QToolBar;
class TreeView;
class QTabWidget;
class MessageLogger;
class ErrorLogger;
class ConnectDialog;
class QAction;
class QSplashScreen;
typedef struct NID NID_t;

class DSBIde : public QMainWindow
{
	Q_OBJECT

public:
	DSBIde();
	~DSBIde();

	void connected();
	MessageLogger *messageLogger() { return m_msglogger; };
	ErrorLogger *errorLogger();
	TreeView *tree() { return m_treeview; };
	void showSplash();
	void hideSplash();
	void newView(const NID_t &d1, const NID_t &d2, const NID_t &nid);

private:
	void make_toolbar();
	void make_menu();

	QToolBar *m_bar;
	QAction *m_bar_connect;
	TreeView *m_treeview;
	QTabWidget *m_tabviews;
	QTabWidget *m_tabsys;
	MessageLogger *m_msglogger;
	ErrorLogger *m_errlogger;
	ConnectDialog *m_connect;
	QSplashScreen *m_splash;
	QAction *m_act_showtree;
	QAction *m_act_split;

public slots:
	void toolclick(QAction *);
	void closeView(int index);
	void viewTabChanged(int);
	void showtreeToggle(bool val);
};


#endif /* IDE_H_ */
