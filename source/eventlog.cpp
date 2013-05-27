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
#include "msglog.h"
#include "eventgen.h"
#include "dsb/net.h"
#include "dsb/net_protocol.h"
#include "dsb/errors.h"
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QAction>
#include <qt4/QtCore/QTimer>
#include <iostream>
#include <stdio.h>

static QTableWidgetItem *itemlist[MAX_READLIST];
extern MessageLogger *msglogger;

int net_cb_result(int sock, void *data)
{
	struct DSBNetEventResult *res = (struct DSBNetEventResult*)data;

	msglogger->addMessage(DSBNET_EVENTRESULT,data);

	//Call original event handler.
	//TODO, should check that this is the current event handler.
	dsb_net_cb_result(sock,data);

	if (itemlist[res->id] != 0)
	{
		char buf[100];
		dsb_nid_toStr(&(res->res), buf, 100);
		itemlist[res->id]->setText(buf);
		itemlist[res->id]->setIcon(QIcon(":/icons/tick.png"));
	}

	return SUCCESS;
}

EventLogger::EventLogger()
	: QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	make_toolbar(mainlayout);
	make_table(mainlayout);

	m_gen = new EventGenerator();

	setWindowTitle("Event Logger");
	resize(600,400);
	show();

	//Create the network polling timer.
	QTimer *nettimer = new QTimer(this);
	connect(nettimer, SIGNAL(timeout()), this, SLOT(netpoll()));
	nettimer->start(20);

	//Replace the default result handler to intercept.
	dsb_net_callback(DSBNET_EVENTRESULT,net_cb_result);
}

EventLogger::~EventLogger()
{

}

void EventLogger::make_table(QLayout *l)
{
	m_table = new QTableWidget();
	m_table->setColumnCount(3);
	m_table->setShowGrid(false);
	QStringList headers;
	headers.append("");
	headers.append("Destination");
	headers.append("Data");
	m_table->setHorizontalHeaderLabels(headers);
	m_table->setColumnWidth(2, 300);
	l->addWidget(m_table);
}

void EventLogger::make_toolbar(QLayout *l)
{
	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/add.png"),"Send");
	m_bar->addAction(QIcon(":/icons/bin.png"),"Clear");
	m_bar->addAction(QIcon(":/icons/control_pause_blue.png"),"Pause");
	l->addWidget(m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));
}

void EventLogger::updateEvent(int id, NID_t *res)
{

}

void EventLogger::addEvent(Event_t *evt)
{
	QTableWidgetItem *item;
	char buf[100];
	int pos;
	int row = m_table->rowCount();
	m_table->setRowCount(row+1);

	//item = new QTableWidgetItem(QIcon(":/icons/email_go.png"),"");
	//m_table->setItem(row,0,item);

	switch(evt->type)
	{
	case EVENT_DEFINE:	item = new QTableWidgetItem(QIcon(":/icons/chart_line_edit.png"),"Edit");
						m_table->setItem(row,0,item);
						dsb_nid_toStr(&(evt->def),buf,100);
						pos = strlen(buf);
						sprintf(&(buf[pos])," (%d)", evt->eval);
						item = new QTableWidgetItem(buf);
						m_table->setItem(row,2,item);
						break;
	case EVENT_GET:		item = new QTableWidgetItem(QIcon(":/icons/chart_line.png"),"Get");
						m_table->setItem(row,0,item);
						item = new QTableWidgetItem(QIcon(":/icons/hourglass.png"),"waiting...");
						m_table->setItem(row,2,item);
						itemlist[evt->eval] = item;
						break;
	case EVENT_NOTIFY:	item = new QTableWidgetItem(QIcon(":/icons/chart_line_error.png"),"Notify");
						m_table->setItem(row,0,item);
						break;
	case EVENT_DEP:		item = new QTableWidgetItem(QIcon(":/icons/chart_line_link.png"),"Link");
						m_table->setItem(row,0,item);
						dsb_nid_toStr(&(evt->dep1),buf,100);
						pos = strlen(buf);
						buf[pos++] = ',';
						buf[pos++] = ' ';
						dsb_nid_toStr(&(evt->dep2),&(buf[pos]),100-pos);
						item = new QTableWidgetItem(buf);
						m_table->setItem(row,2,item);
						break;
	default:			item = new QTableWidgetItem("Unknown");
						m_table->setItem(row,0,item);
						break;
	}


	dsb_nid_toStr(&(evt->d1),buf,100);
	pos = strlen(buf);
	buf[pos++] = ',';
	dsb_nid_toStr(&(evt->d2),&(buf[pos]),100-pos);
	item = new QTableWidgetItem(buf);
	m_table->setItem(row,1,item);

	//Save the event somewhere if it is a GET event that needs updating
	if (evt->type == EVENT_GET)
	{
		//struct EventRecord *erec = new EventRecord;
		//erec->evt = evt;
		//erec->item = row;
		//s_syncget.push_back(erec);
		//evt->eval = s_syncget.size();
	}
}

void EventLogger::toolclick(QAction *a)
{
	if (a->text() == "Send")
	{
		m_gen->show();
	}
	else if (a->text() == "Clear")
	{
		m_table->clear();
		QStringList headers;
		headers.append("");
		headers.append("Destination");
		headers.append("Result");
		headers.append("Data");
		m_table->setHorizontalHeaderLabels(headers);
		m_table->setRowCount(0);
	}
}

void EventLogger::netpoll()
{
	dsb_net_poll(0);
}
