/*
 * eventgen.cpp
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

#include "eventgen.h"
#include <qt4/QtGui/QWidget>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <iostream>
#include <dsb/nid.h>
#include <dsb/errors.h>
#include <dsb/common.h>

extern "C"
{
int dsb_send(struct Event *,int);
}

EventGenerator::EventGenerator()
	: QWidget(0, Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QHBoxLayout *typelayout = new QHBoxLayout();
	QHBoxLayout *destlayout = new QHBoxLayout();
	QHBoxLayout *deflayout = new QHBoxLayout();
	QHBoxLayout *deplayout = new QHBoxLayout();
	QHBoxLayout *buttonlayout = new QHBoxLayout();

	mainlayout->addLayout(typelayout);
	mainlayout->addLayout(destlayout);
	mainlayout->addLayout(deflayout);
	mainlayout->addLayout(deplayout);
	mainlayout->addLayout(buttonlayout);
	setLayout(mainlayout);

	m_typelabel = new QLabel("Type:",this);
	typelayout->addWidget(m_typelabel);

	//Type combobox
	m_type = new QComboBox(this);
	typelayout->addWidget(m_type);
	m_type->addItem("Edit");
	m_type->addItem("Notify");
	m_type->addItem("Get");
	m_type->addItem("Link");

	//NIDs for Event destinations
	m_destlabel = new QLabel("Destination:");
	destlayout->addWidget(m_destlabel);
	m_dest1 = new QLineEdit();
	m_dest1->setAutoFillBackground(true);
	destlayout->addWidget(m_dest1);
	m_dest1vl = new QLabel();
	QPixmap tick(":/icons/tick.png");
	m_dest1vl->setPixmap(tick);
	destlayout->addWidget(m_dest1vl);
	m_dest2 = new QLineEdit();
	m_dest2->setAutoFillBackground(true);
	destlayout->addWidget(m_dest2);
	m_dest2vl = new QLabel();
	//QPixmap tick("./data/icons/greentick.png");
	m_dest2vl->setPixmap(tick);
	destlayout->addWidget(m_dest2vl);

	//Definition NID
	m_deflabel = new QLabel("Definition/Value:");
	deflayout->addWidget(m_deflabel);
	m_def = new QLineEdit();
	m_def->setAutoFillBackground(true);
	deflayout->addWidget(m_def);
	m_defvl = new QLabel();
	//QPixmap tick("./data/icons/greentick.png");
	m_defvl->setPixmap(tick);
	deflayout->addWidget(m_defvl);

	m_eval = new QComboBox();
	deflayout->addWidget(m_eval);
	m_eval->addItem("Constant");
	m_eval->addItem("Simple");
	m_eval->addItem("Agent");

	//Dependency NIDs
	m_deplabel = new QLabel("Dependency:");
	deplayout->addWidget(m_deplabel);
	m_dep1 = new QLineEdit();
	m_dep1->setAutoFillBackground(true);
	m_dep1->setEnabled(false);
	deplayout->addWidget(m_dep1);
	m_dep1vl = new QLabel();
	//QPixmap tick("./data/icons/greentick.png");
	m_dep1vl->setPixmap(tick);
	deplayout->addWidget(m_dep1vl);
	m_dep2 = new QLineEdit();
	m_dep2->setAutoFillBackground(true);
	m_dep2->setEnabled(false);
	deplayout->addWidget(m_dep2);
	m_dep2vl = new QLabel();
	//QPixmap tick("./data/icons/greentick.png");
	m_dep2vl->setPixmap(tick);
	deplayout->addWidget(m_dep2vl);

	m_send = new QPushButton("Send");
	buttonlayout->addWidget(m_send);
	m_cancel = new QPushButton("Done");
	buttonlayout->addWidget(m_cancel);

	m_event.type = EVENT_DEFINE;
	m_event.eval = 0;

	setWindowTitle("Event Generator");
	//resize(400,400);
	//show();

	connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(typechanged(int)));
	connect(m_dest1, SIGNAL(textChanged(const QString &)), this, SLOT(dest1changed(const QString &)));
	connect(m_dest2, SIGNAL(textChanged(const QString &)), this, SLOT(dest2changed(const QString &)));
	connect(m_def, SIGNAL(textChanged(const QString &)), this, SLOT(defchanged(const QString &)));
	connect(m_dep1, SIGNAL(textChanged(const QString &)), this, SLOT(dep1changed(const QString &)));
	connect(m_dep2, SIGNAL(textChanged(const QString &)), this, SLOT(dep2changed(const QString &)));
	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));
	connect(m_send, SIGNAL(clicked()), this, SLOT(sendclicked()));
}

EventGenerator::~EventGenerator()
{

}

void EventGenerator::dest1changed(const QString &txt)
{
	if (txt.length() == 0)
	{
		QPixmap tick(":/icons/tick.png");
		m_dest1vl->setPixmap(tick);
		return;
	}

	if (dsb_nid_fromStr((const char*)txt.toAscii().constData(),&(m_event.d1)) != SUCCESS)
	{
		QPixmap cross(":/icons/cross.png");
		m_dest1vl->setPixmap(cross);
	}
	else
	{
		QPixmap tick(":/icons/tick.png");
		m_dest1vl->setPixmap(tick);
	}
}

void EventGenerator::dest2changed(const QString &txt)
{
	if (txt.length() == 0)
	{
		QPixmap tick(":/icons/tick.png");
		m_dest2vl->setPixmap(tick);
		return;
	}

	if (dsb_nid_fromStr((const char*)txt.toAscii().constData(),&(m_event.d2)) != SUCCESS)
	{
		QPixmap cross(":/icons/cross.png");
		m_dest2vl->setPixmap(cross);
	}
	else
	{
		QPixmap tick(":/icons/tick.png");
		m_dest2vl->setPixmap(tick);
	}
}

void EventGenerator::defchanged(const QString &txt)
{
	if (txt.length() == 0)
	{
		QPixmap tick(":/icons/tick.png");
		m_defvl->setPixmap(tick);
		return;
	}

	if (dsb_nid_fromStr((const char*)txt.toAscii().constData(),&(m_event.def)) != SUCCESS)
	{
		QPixmap cross(":/icons/cross.png");
		m_defvl->setPixmap(cross);
	}
	else
	{
		QPixmap tick(":/icons/tick.png");
		m_defvl->setPixmap(tick);
	}
}

void EventGenerator::dep1changed(const QString &txt)
{
	if (txt.length() == 0)
	{
		QPixmap tick(":/icons/tick.png");
		m_dep1vl->setPixmap(tick);
		return;
	}

	if (dsb_nid_fromStr((const char*)txt.toAscii().constData(),&(m_event.dep1)) != SUCCESS)
	{
		QPixmap cross(":/icons/cross.png");
		m_dep1vl->setPixmap(cross);
	}
	else
	{
		QPixmap tick(":/icons/tick.png");
		m_dep1vl->setPixmap(tick);
	}
}

void EventGenerator::dep2changed(const QString &txt)
{
	if (txt.length() == 0)
	{
		QPixmap tick(":/icons/tick.png");
		m_dep2vl->setPixmap(tick);
		return;
	}

	if (dsb_nid_fromStr((const char*)txt.toAscii().constData(),&(m_event.dep2)) != SUCCESS)
	{
		QPixmap cross(":/icons/cross.png");
		m_dep2vl->setPixmap(cross);
	}
	else
	{
		QPixmap tick(":/icons/tick.png");
		m_dep2vl->setPixmap(tick);
	}
}

void EventGenerator::sendclicked()
{
	//Actually send the event here.
	m_event.eval = 0;
	m_event.flags = 0;
	dsb_send(&m_event,1);
}

void EventGenerator::cancelclicked()
{
	hide();
}

void EventGenerator::typechanged(int index)
{
	switch(index)
	{
	case 0:	m_event.type = EVENT_DEFINE;
			m_def->setEnabled(true);
			m_eval->setEnabled(true);
			m_dep1->setEnabled(false);
			m_dep2->setEnabled(false);
			break;
	case 1: m_event.type = EVENT_NOTIFY;
			m_def->setEnabled(false);
			m_eval->setEnabled(false);
			m_dep1->setEnabled(false);
			m_dep2->setEnabled(false);
			break;
	case 2: m_event.type = EVENT_GET;
			m_def->setEnabled(false);
			m_eval->setEnabled(false);
			m_dep1->setEnabled(false);
			m_dep2->setEnabled(false);
			break;
	case 3: m_event.type = EVENT_DEP;
			m_def->setEnabled(false);
			m_eval->setEnabled(false);
			m_dep1->setEnabled(true);
			m_dep2->setEnabled(true);
			break;
	default: break;
	}
}

