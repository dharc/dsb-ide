/*
 * msglog.cpp
 *
 *  Created on: 27 May 2013
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


#include "msglog.h"
#include "dsb/net.h"
#include "dsb/net_protocol.h"
#include "dsb/nid.h"
#include "dsb/errors.h"
#include <qt4/QtGui/QTreeWidget>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>

extern MessageLogger *msglogger;

int net_cb_error(void *sock, void *data)
{
	msglogger->addMessage(DSBNET_ERROR,data);
	return 0;
}

int net_cb_debugevent(void *sock, void *data)
{
	msglogger->addMessage(DSBNET_DEBUGEVENT,data);
	return 0;
}

MessageLogger::MessageLogger()
	: QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_tree = new QTreeWidget();
	m_tree->setColumnCount(2);
	m_tree->setHeaderLabels(QStringList(QString("Name")) << QString("Value"));
	m_tree->setColumnWidth(0,250);
	mainlayout->addWidget(m_tree);

	setWindowTitle("Message Log");
	resize(600,400);
	show();

	dsb_net_callback(DSBNET_ERROR,net_cb_error);
}

MessageLogger::~MessageLogger()
{

}

void MessageLogger::make_tree(QLayout *l)
{

}

void MessageLogger::addMessage(unsigned short type, void *data)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *item2;
	Event_t evt;
	char buf[100];

	switch(type)
	{
	case DSBNET_DEBUGEVENT:
		//Unpack the event
		dsb_event_unpack((const char*)data,&evt);
		item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("DSBNET_DEBUGEVENT")));
		item->setIcon(0,QIcon(":/icons/email_go.png"));
		item2 = new QTreeWidgetItem(item, QStringList(QString("Type")) << QString("0x%1").arg(evt.type,0,16));
		dsb_nid_toStr(&(evt.d1),buf,100);
		item2 = new QTreeWidgetItem(item, QStringList(QString("Dest1")) << QString(buf));
		dsb_nid_toStr(&(evt.d2),buf,100);
		item2 = new QTreeWidgetItem(item, QStringList(QString("Dest2")) << QString(buf));

		switch(evt.type)
		{
		case EVENT_DEFINE:
			item2 = new QTreeWidgetItem(item, QStringList(QString("Evaluator")) << QString("%1").arg(evt.eval));
			dsb_nid_toStr(&(evt.def),buf,100);
			item2 = new QTreeWidgetItem(item, QStringList(QString("Definition")) << QString(buf));
			break;
		case EVENT_GET:
			item2 = new QTreeWidgetItem(item, QStringList(QString("ID")) << QString("%1").arg(evt.resid));
			break;
		default: break;
		}

		m_tree->insertTopLevelItem(0,item);
		item->setExpanded(true);
		break;

	case DSBNET_SENDEVENT:
		//Unpack the event
		dsb_event_unpack((const char*)data,&evt);
		item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("DSBNET_SENDEVENT")));
		item->setIcon(0,QIcon(":/icons/email_go.png"));
		item2 = new QTreeWidgetItem(item, QStringList(QString("Type")) << QString("0x%1").arg(evt.type,0,16));
		dsb_nid_toStr(&(evt.d1),buf,100);
		item2 = new QTreeWidgetItem(item, QStringList(QString("Dest1")) << QString(buf));
		dsb_nid_toStr(&(evt.d2),buf,100);
		item2 = new QTreeWidgetItem(item, QStringList(QString("Dest2")) << QString(buf));

		switch(evt.type)
		{
		case EVENT_DEFINE:
			item2 = new QTreeWidgetItem(item, QStringList(QString("Evaluator")) << QString("%1").arg(evt.eval));
			dsb_nid_toStr(&(evt.def),buf,100);
			item2 = new QTreeWidgetItem(item, QStringList(QString("Definition")) << QString(buf));
			break;
		case EVENT_GET:
			item2 = new QTreeWidgetItem(item, QStringList(QString("ID")) << QString("%1").arg(evt.resid));
			break;
		default: break;
		}

		m_tree->insertTopLevelItem(0,item);
		item->setExpanded(true);
		break;

	case DSBNET_EVENTRESULT:
		evt.resid = *((int*)data);
		dsb_nid_unpack((const char*)data+sizeof(int),&evt.d1);
		item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("DSBNET_EVENTRESULT")));
		item->setIcon(0,QIcon(":/icons/email_open.png"));
		item2 = new QTreeWidgetItem(item, QStringList(QString("ID")) << QString("%1").arg(evt.resid));
		dsb_nid_toStr(&(evt.d1),buf,100);
		item2 = new QTreeWidgetItem(item, QStringList(QString("Result")) << QString(buf));
		m_tree->insertTopLevelItem(0,item);
		item->setExpanded(true);
		break;

	case DSBNET_ERROR:
			item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("DSBNET_ERROR")));
			item->setIcon(0,QIcon(":/icons/email_open.png"));
			item2 = new QTreeWidgetItem(item, QStringList(QString("Error")) << QString(dsb_log_str(evt.err)));
			m_tree->insertTopLevelItem(0,item);
			item->setExpanded(true);
			break;
	default:				break;
	}
}
