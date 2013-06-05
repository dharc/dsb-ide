/*
 * msggen.cpp
 *
 *  Created on: 30 May 2013
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

#include "msggen.h"
#include "dsb/event.h"
#include "dsb/nid.h"
#include "dsb/net.h"
#include "dsb/net_protocol.h"
#include <qt4/QtGui/QComboBox>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QLabel>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QTableWidget>

extern "C"
{
int dsb_send(struct Event *,int);
}

static NID_t dummy;
extern void *hostsock;

MessageGenerator::MessageGenerator()
	: QWidget(0, Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QHBoxLayout *typelayout = new QHBoxLayout();
	QHBoxLayout *tablelayout = new QHBoxLayout();
	QHBoxLayout *buttonlayout = new QHBoxLayout();

	mainlayout->addLayout(typelayout);
	mainlayout->addLayout(tablelayout);
	mainlayout->addLayout(buttonlayout);
	setLayout(mainlayout);

	m_typelabel = new QLabel("Type:",this);
	typelayout->addWidget(m_typelabel);

	//Type combobox
	m_type = new QComboBox(this);
	typelayout->addWidget(m_type);
	m_type->addItem("Send Event");
	m_type->addItem("Event Result");
	m_type->addItem("Error");
	m_type->addItem("Login");
	m_type->addItem("Root");
	m_type->addItem("Debugger");
	m_type->addItem("Debug Event");

	m_table = new QTableWidget();
	m_table->setColumnCount(2);
	m_table->setShowGrid(true);
	QStringList headers;
	headers.append("Field");
	headers.append("Value");
	m_table->setHorizontalHeaderLabels(headers);
	m_table->setColumnWidth(0, 100);
	m_table->setColumnWidth(1, 200);
	tablelayout->addWidget(m_table);

	m_send = new QPushButton("Send");
	buttonlayout->addWidget(m_send);
	m_done = new QPushButton("Done");
	buttonlayout->addWidget(m_done);

	setWindowTitle("Message Generator");
	resize(400,500);

	connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(typechanged(int)));
	connect(m_done, SIGNAL(clicked()), this, SLOT(doneclicked()));
	connect(m_send, SIGNAL(clicked()), this, SLOT(sendclicked()));

	template_sendevent();
}

MessageGenerator::~MessageGenerator()
{

}

void MessageGenerator::typechanged(int index)
{
	switch(index)
	{
	case 0:		template_sendevent(); break;
	case 1:		template_eventresult(); break;
	case 2:		template_error(); break;
	case 3:		template_login(); break;
	case 4:		template_root(); break;
	case 5:		template_debugger(); break;
	case 6:		template_debugevent(); break;
	default: break;
	}
}

void MessageGenerator::sendclicked()
{
	switch(m_type->currentIndex())
	{
	case 0:		send_sendevent(); break;
	case 1:		send_eventresult(); break;
	case 2:		send_error(); break;
	case 3:		send_login(); break;
	case 4:		send_root(); break;
	case 5:		send_debugger(); break;
	case 6:		send_debugevent(); break;
	default: break;
	}
}

void MessageGenerator::doneclicked()
{
	hide();
}

void MessageGenerator::template_sendevent()
{
	QTableWidgetItem *item;

	m_table->setRowCount(7);

	item = new QTableWidgetItem("Type");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(0,0,item);
	item = new QTableWidgetItem("Dest1");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(1,0,item);
	item = new QTableWidgetItem("Dest2");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(2,0,item);
	item = new QTableWidgetItem("ResID/Eval");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(3,0,item);
	item = new QTableWidgetItem("Def");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(4,0,item);
	item = new QTableWidgetItem("Dep1");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(5,0,item);
	item = new QTableWidgetItem("Dep2");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(6,0,item);
}

void MessageGenerator::template_eventresult()
{
	QTableWidgetItem *item;

	m_table->setRowCount(2);

	item = new QTableWidgetItem("ResID");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(0,0,item);
	item = new QTableWidgetItem("Result");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(1,0,item);
}

void MessageGenerator::template_error()
{
	QTableWidgetItem *item;

	m_table->setRowCount(1);

	item = new QTableWidgetItem("Error");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(0,0,item);
}

void MessageGenerator::template_login()
{
	QTableWidgetItem *item;

	m_table->setRowCount(2);

	item = new QTableWidgetItem("Username");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(0,0,item);
	item = new QTableWidgetItem("Password");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(1,0,item);
}

void MessageGenerator::template_root()
{

}

void MessageGenerator::template_debugger()
{
	QTableWidgetItem *item;

	m_table->setRowCount(1);

	item = new QTableWidgetItem("Flags");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	m_table->setItem(0,0,item);
}

void MessageGenerator::template_debugevent()
{

}

void MessageGenerator::send_sendevent()
{
	Event_t *e = dsb_event_allocate();
	e->type = (EventType)m_table->item(0,1)->text().toInt();
	dsb_nid_fromStr(m_table->item(1,1)->text().toAscii().constData(), &e->d1);
	dsb_nid_fromStr(m_table->item(2,1)->text().toAscii().constData(), &e->d2);

	switch(e->type)
	{
	case EVENT_GET:
		e->res = &dummy;
		break;
	case EVENT_ALLOCATE:
		e->res = &dummy;
		break;
	case EVENT_DEFINE:
		e->eval = m_table->item(3,1)->text().toUInt();
		dsb_nid_fromStr(m_table->item(4,1)->text().toAscii().constData(), &e->def);
		break;
	case EVENT_NOTIFY:
		break;
	case EVENT_DEP:
		dsb_nid_fromStr(m_table->item(5,1)->text().toAscii().constData(), &e->dep1);
		dsb_nid_fromStr(m_table->item(6,1)->text().toAscii().constData(), &e->dep2);
		break;
	default: break;
	}

	e->flags = EVTFLAG_FREE;
	dsb_send(e,1);
}

void MessageGenerator::send_eventresult()
{

}

void MessageGenerator::send_error()
{
	dsb_net_send_error(hostsock, m_table->item(0,1)->text().toInt());
}

void MessageGenerator::send_login()
{

}

void MessageGenerator::send_root()
{

}

void MessageGenerator::send_debugger()
{

}

void MessageGenerator::send_debugevent()
{

}
