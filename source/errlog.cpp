/*
 * errlog.cpp
 *
 *  Created on: 18 Jun 2013
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

#include <dsb/errors.h>
#include "errlog.h"
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QListWidget>

ErrorLogger::ErrorLogger()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);
	setAutoFillBackground(true);

	m_list = new QListWidget();
	mainlayout->addWidget(m_list);

	show();
}

ErrorLogger::~ErrorLogger()
{

}

void ErrorLogger::addError(unsigned short type, const char *data)
{
	QListWidgetItem *item = 0;
	int cat = type & 0xFF000;

	if (cat == ERR_ERROR)
	{
		item = new QListWidgetItem(QIcon(":/icons/exclamation.png"),QString(dsb_log_str(type)) + ": " + data);
	}
	else if (cat == WARN_START)
	{
		item = new QListWidgetItem(QIcon(":/icons/error.png"),QString(dsb_log_str(type)) + ": " + data);
	}
	else if (cat == INFO_START)
	{
		item = new QListWidgetItem(QIcon(":/icons/information.png"),QString(dsb_log_str(type)) + ": " + data);
	}
	else if (cat == DEBUG_START)
	{
		item = new QListWidgetItem(QIcon(":/icons/information.png"),QString(dsb_log_str(type)) + ": " + data);
	}
	else
	{
		item = new QListWidgetItem(QIcon(":/icons/error.png"),QString(dsb_log_str(type)) + ": " + data);
	}

	if (item != 0)
	{
		m_list->insertItem(0,item);
	}
}

