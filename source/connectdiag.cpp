/*
 * connectdiag.cpp
 *
 *  Created on: 24 May 2013
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

#include "connectdiag.h"
#include <qt4/QtGui/QLineEdit>
#include <qt4/QtGui/QLabel>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include "dsb/net.h"
#include "dsb/net_protocol.h"
#include "dsb/string.h"

#include <iostream>

extern void *hostsock;

ConnectDialog::ConnectDialog()
	: QWidget(0, Qt::Dialog)
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QHBoxLayout *msglayout = new QHBoxLayout();
	QHBoxLayout *hostlayout = new QHBoxLayout();
	QHBoxLayout *userlayout = new QHBoxLayout();
	QHBoxLayout *passlayout = new QHBoxLayout();
	QHBoxLayout *buttonlayout = new QHBoxLayout();

	mainlayout->addLayout(msglayout);
	mainlayout->addLayout(hostlayout);
	mainlayout->addLayout(userlayout);
	mainlayout->addLayout(passlayout);
	mainlayout->addLayout(buttonlayout);

	m_message = new QLabel("Enter location of a DSBD instance:");
	msglayout->addWidget(m_message);

	m_urllabel = new QLabel("Host [:port]:");
	m_host = new QLineEdit();
	m_host->setAutoFillBackground(true);
	m_host->setMinimumWidth(200);
	hostlayout->addWidget(m_urllabel);
	hostlayout->addWidget(m_host);

	m_userlabel = new QLabel("Username:");
	m_user = new QLineEdit();
	m_user->setAutoFillBackground(true);
	userlayout->addWidget(m_userlabel);
	userlayout->addWidget(m_user);

	m_passlabel = new QLabel("Password:");
	m_pass = new QLineEdit();
	m_pass->setAutoFillBackground(true);
	passlayout->addWidget(m_passlabel);
	passlayout->addWidget(m_pass);

	m_ok = new QPushButton("Connect");
	m_cancel = new QPushButton("Cancel");
	buttonlayout->addWidget(m_ok);
	buttonlayout->addWidget(m_cancel);

	setLayout(mainlayout);
	setWindowTitle("Connect");
	show();

	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));
	connect(m_ok, SIGNAL(clicked()), this, SLOT(connectclicked()));
}

ConnectDialog::~ConnectDialog()
{

}

void ConnectDialog::connectclicked()
{
	hostsock = dsb_net_connect(m_host->text().toAscii().constData());
	if (hostsock == 0)
	{
		m_message->setText("Could not connect to host!");
	}
	else
	{
		//dsb_net_send_debugger(hostsock,NET_DEBUG_QUEUES);

		hide();
	}
}

void ConnectDialog::cancelclicked()
{
	hide();
}

