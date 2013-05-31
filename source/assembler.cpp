/*
 * assember.cpp
 *
 *  Created on: 31 May 2013
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

#include "asmsyntax.h"
#include "assembler.h"
#include "dsb/assembler.h"
#include "dsb/vm.h"
#include "dsb/nid.h"
#include <qt4/QtGui/QHBoxLayout>
#include <qt4/QtGui/QVBoxLayout>
#include <qt4/QtGui/QAction>
#include <qt4/QtGui/QToolBar>
#include <qt4/QtGui/QTextEdit>
#include <qt4/QtGui/QTableWidget>
#include <qt4/QtGui/QSplitter>
#include <qt4/QtGui/QLabel>
#include <iostream>

Assembler::Assembler()
 : QWidget()
{
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QSplitter *split = new QSplitter(Qt::Horizontal);
	setLayout(mainlayout);

	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/script.png"),"New");
	m_bar->addAction(QIcon(":/icons/folder.png"),"Open");
	m_bar->addAction(QIcon(":/icons/disk.png"),"Save");
	m_bar->addSeparator();
	m_bar->addAction(QIcon(":/icons/table_edit.png"),"Edit Object");
	m_bar->addAction(QIcon(":/icons/table_save.png"),"Save Object");
	m_bar->addSeparator();
	m_bar->addAction(QIcon(":/icons/control_play_blue.png"),"Run");
	m_bar->addAction(QIcon(":/icons/bug.png"),"Debug");

	mainlayout->addWidget(m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));

	mainlayout->addWidget(split);
	m_asm = new QTextEdit();
	m_asm->setFont(QFont("Courier New", 10));
	Syntax *syn = new Syntax(m_asm->document());
	split->addWidget(m_asm);
	m_regs = new QTableWidget();
	m_regs->setColumnCount(1);
	m_regs->setRowCount(16);
	QStringList headers;
	headers.append("Registers");
	m_regs->setHorizontalHeaderLabels(headers);
	split->addWidget(m_regs);
	QList<int> sizes;
	sizes.append(200);
	sizes.append(20);
	split->setSizes(sizes);

	m_result = new QLabel("Result = null");
	mainlayout->addWidget(m_result);

	setWindowTitle("DSB Assembler");
	resize(700,400);
	show();

	m_running = false;
}

Assembler::~Assembler()
{

}

static NID_t reg[16];
static NID_t code[200];
static NID_t res;
static int codesize;
static int curip;

void Assembler::toolclick(QAction *a)
{
	if (a->text() == "New")
	{
		m_asm->clear();
	}
	else if (a->text() == "Run")
	{
		QTableWidgetItem *item;

		codesize = dsb_assemble(m_asm->toPlainText().toAscii().constData(),code,200);
		dsb_vm_interpret_reg(code,codesize,reg,0,0,&res);

		char buf[100];
		dsb_nid_toStr(&res, buf, 100);
		m_result->setText(QString("Result = %1").arg(buf));

		//Update register table.
		for (int i=0; i<16; i++)
		{
			dsb_nid_toStr(&reg[i],buf,100);
			item = new QTableWidgetItem(buf);
			m_regs->setItem(i,0,item);
		}
	}
	else if (a->text() == "Debug")
	{
		char buf[100];
		QTableWidgetItem *item;
		int tmp;

		codesize = dsb_assemble(m_asm->toPlainText().toAscii().constData(),code,200);

		if (m_running == false)
		{
			curip = 0;
			m_running = true;
		}

		tmp = dsb_vm_interpret_reg(&code[curip],1,reg,0,0,&res);

		if (tmp == -1)
		{
			m_running = false;
			curip = 0;
			dsb_nid_toStr(&res, buf, 100);
			m_result->setText(QString("Result = %1").arg(buf));
		}
		else
		{
			curip += tmp;
		}

		//Update register table.
		for (int i=0; i<16; i++)
		{
			dsb_nid_toStr(&reg[i],buf,100);
			item = new QTableWidgetItem(buf);
			m_regs->setItem(i,0,item);
		}
	}
}

