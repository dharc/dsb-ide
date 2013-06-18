/*
 * ide.cpp
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

#include "dsb/ide/ide.h"
#include "treeview.h"
#include "msglog.h"
#include "errlog.h"
#include "connectdiag.h"
#include "dsb/ide/view.h"
#include <dsb/globals.h>
#include <dsb/pattern.h>
#include <dsb/net.h>
#include <dsb/net_protocol.h>
#include <qt4/QtGui/QToolBar>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QAction>
#include <qt4/QtGui/QSplitter>
#include <qt4/QtGui/QSplashScreen>
#include <qt4/QtCore/QCoreApplication>
#include <qt4/QtGui/QMenuBar>
#include <qt4/QtGui/QStatusBar>

#include <cstdio>

extern DSBIde *ide;
extern void *hostsock;

DSBIde::DSBIde()
 : QMainWindow()
{
	ide = this;

	//QVBoxLayout *mainlayout = new QVBoxLayout();
	QSplitter *split = new QSplitter(Qt::Horizontal);
	QSplitter *vsplit = new QSplitter(Qt::Vertical);
	//mainlayout->addWidget(split);
	setCentralWidget(split);

	//make_menu();
	make_toolbar();

	m_treeview = new TreeView();
	split->addWidget(m_treeview);

	m_tabviews = new QTabWidget();
	m_tabviews->setTabsClosable(true);
	connect(m_tabviews,SIGNAL(tabCloseRequested(int)),this,SLOT(closeView(int)));
	vsplit->addWidget(m_tabviews);
	m_tabsys = new QTabWidget();
	vsplit->addWidget(m_tabsys);

	m_msglogger = new MessageLogger();
	m_tabsys->addTab(m_msglogger,"Network");
	m_errlogger = new ErrorLogger();
	m_tabsys->addTab(m_errlogger,"Errors");

	split->addWidget(vsplit);

	QList<int> sizes;
	sizes.append(220);
	sizes.append(1000);
	split->setSizes(sizes);

	QList<int> sizes2;
	sizes2.append(1000);
	sizes2.append(300);
	vsplit->setSizes(sizes2);

	m_connect = new ConnectDialog();
	m_splash = new QSplashScreen(QPixmap(":/splash/cantronik.png"));
	m_splash->show();

	setWindowTitle("DSB Interactive Development Environment");
	setWindowState(Qt::WindowMaximized);
	//show();
}

DSBIde::~DSBIde()
{

}

void DSBIde::connected()
{
	QString name = m_connect->getHost();
	m_treeview->setRoot(Root, name, true);
	m_bar_connect->setIcon(QIcon(":/icons/disconnect.png"));
	m_bar_connect->setText("Disconnect");
	m_splash->hide();

	statusBar()->showMessage("Connected to "+name);

	//Register this IDE as a core debugger.
	dsb_net_send_debugger(hostsock,0);

	show();
}

void DSBIde::make_menu()
{
	QMenuBar *menu = menuBar();
	QMenu *filemenu;

	filemenu = menu->addMenu("File");
	filemenu->addAction("Quit");
}

void DSBIde::make_toolbar()
{
	m_bar = new QToolBar();
	m_bar_connect = m_bar->addAction(QIcon(":/icons/dharc-16.png"),"DHARC");
	m_bar->addSeparator();
	m_bar_connect = m_bar->addAction(QIcon(":/icons/connect.png"),"Connect");
	m_bar->addSeparator();
	m_act_showtree = m_bar->addAction(QIcon(":/icons/application_side_tree.png"), "Tree");
	m_act_split = m_bar->addAction(QIcon(":/icons/application_tile_horizontal.png"), "Split");
	m_bar->addAction(QIcon(":/icons/application_double.png"), "Undock");

	m_act_showtree->setCheckable(true);
	m_act_showtree->setChecked(true);
	m_act_split->setCheckable(true);

	addToolBar(m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
	connect(m_act_showtree,SIGNAL(toggled(bool)),this,SLOT(showtreeToggle(bool)));
}

void DSBIde::showtreeToggle(bool val)
{
	if (val)
	{
		m_treeview->show();
	}
	else
	{
		m_treeview->hide();
	}
}

void DSBIde::toolclick(QAction *a)
{
	if (a->text() == "Connect")
	{
		m_connect->show();
	}
	else if (a->text() == "Disconnect")
	{
		statusBar()->showMessage("Disconnected");
		dsb_net_disconnect(hostsock);
		m_treeview->clear();
		m_tabviews->clear();
		m_bar_connect->setIcon(QIcon(":/icons/connect.png"));
		m_bar_connect->setText("Connect");
	}
}

void DSBIde::closeView(int index)
{
	QWidget *cur = m_tabviews->widget(index);
	m_tabviews->removeTab(index);
	delete cur;
}

void DSBIde::viewTabChanged(int index)
{

}

void DSBIde::showSplash()
{
	m_splash->show();
	m_splash->showMessage("Connecting...",Qt::AlignHCenter | Qt::AlignBottom);
	QCoreApplication::processEvents();
}

void DSBIde::hideSplash()
{
	m_splash->hide();
}

void DSBIde::newView(const NID_t &d1, const NID_t &d2, const NID_t &nid)
{
	unsigned int pat = dsb_pattern_what(&nid);

	printf("Pattern: %x\n",pat);

	DSBView *nv = DSBView::createView(pat);
	if (nv != 0)
	{
		nv->addHARC(d1,d2,nid);
		m_tabviews->addTab(nv,nv->title());
		m_tabviews->setCurrentWidget(nv);
	}
}

