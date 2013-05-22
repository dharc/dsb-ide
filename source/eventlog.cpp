/*
 * eventlog.cpp
 *
 *  Created on: 22 May 2013
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


#include "eventlog.h"
#include "eventgen.h"
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QAction>
#include <iostream>


EventLogger::EventLogger()
	: QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/email.png"),"Send");
	m_bar->addAction(QIcon(":/icons/bin.png"),"Clear");
	m_bar->addAction(QIcon(":/icons/control_pause_blue.png"),"Pause");
	mainlayout->addWidget(m_bar);

	m_table = new QTableWidget();
	m_table->setColumnCount(5);
	QStringList headers;
	headers.append("");
	headers.append("Type");
	headers.append("Destination");
	headers.append("Result");
	headers.append("Data");
	m_table->setHorizontalHeaderLabels(headers);
	mainlayout->addWidget(m_table);

	m_gen = new EventGenerator();

	setWindowTitle("Event Logger");
	resize(600,400);
	show();

	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
}

EventLogger::~EventLogger()
{

}

void EventLogger::addEvent(Event_t *evt)
{
	QTableWidgetItem *item;
	int row = m_table->rowCount();
	m_table->setRowCount(row+1);

	item = new QTableWidgetItem(QIcon(":/icons/email_go.png"),"");
	m_table->setItem(row,0,item);

	switch(evt->type)
	{
	case EVENT_DEFINE:	item = new QTableWidgetItem("DEFINE");
						break;
	case EVENT_GET:		item = new QTableWidgetItem("GET");
						break;
	case EVENT_NOTIFY:	item = new QTableWidgetItem("NOTIFY");
						break;
	case EVENT_DEP:		item = new QTableWidgetItem("DEP");
						break;
	default:			item = new QTableWidgetItem("UNKNOWN");
						break;
	}
	m_table->setItem(row,1,item);

	char buf[100];
	dsb_nid_toStr(&(evt->d1),buf,100);
	int pos = strlen(buf);
	buf[pos++] = ',';
	dsb_nid_toStr(&(evt->d2),&(buf[pos]),100-pos);
	item = new QTableWidgetItem(buf);
	m_table->setItem(row,2,item);
}

void EventLogger::toolclick(QAction *a)
{
	if (a->text() == "Send")
	{
		m_gen->show();
	}
}
