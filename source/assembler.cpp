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
#include <qt4/QtGui/QFileDialog>
#include <qt4/QtCore/QTextStream>
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
	m_play = m_bar->addAction(QIcon(":/icons/control_play_blue.png"),"Run");
	m_debug = m_bar->addAction(QIcon(":/icons/bug.png"),"Debug");
	m_step = m_bar->addAction(QIcon(":/icons/resultset_next.png"),"Step");
	m_step->setEnabled(false);

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
	m_ctx.code = new NID_t[1000];
	m_ctx.result = new NID_t;
}

Assembler::~Assembler()
{

}


void Assembler::toolclick(QAction *a)
{
	if (a->text() == "New")
	{
		m_asm->clear();
		m_running = false;
	}
	else if (a->text() == "Open")
	{
		load_asm();
	}
	else if (a->text() == "Save")
	{
		save_asm();
	}
	else if (a->text() == "Run")
	{
		//m_debug->setEnabled(false);
		start();
		//m_debug->setEnabled(true);
	}
	else if (a->text() == "Stop")
	{
		m_play->setIcon(QIcon(":/icons/control_play_blue.png"));
		m_play->setText("Play");
		m_step->setEnabled(false);
		m_debug->setEnabled(true);
		stop();
	}
	else if (a->text() == "Debug")
	{
		m_debug->setEnabled(false);
		m_play->setIcon(QIcon(":/icons/control_stop_blue.png"));
		m_play->setText("Stop");
		m_step->setEnabled(true);
		start_debug();
	}
	else if (a->text() == "Step")
	{
		step_debug();
	}
}

void Assembler::start_debug()
{
	int ret;
	m_ctx.timeout = 1;
	int line=0;
	int lip;

	int ip = 0;
	struct VMLabel *labels = new VMLabel[MAX_LABELS];
	const char *source = m_asm->toPlainText().toAscii().constData();

	//For every line
	while(1)
	{
		lip = ip;
		dsb_assemble_line(labels,source,m_ctx.code,&ip);
		//Store which line corresponds to each instructions.
		m_ipline[lip] = line++;
		lip = ip;
		//Move to next line if there is one.
		source = strchr(source,'\n');
		if (source == 0) break;
		source++;
	}

	delete [] labels;

	if (m_running == false)
	{
		m_ctx.ip = 0;
		m_running = true;
		dsb_nid_null(m_ctx.result);
	}

	QTextEdit::ExtraSelection highlight;
	highlight.cursor = QTextCursor(m_asm->document()->findBlockByLineNumber(m_ipline[0]));
	highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
	highlight.format.setBackground( QColor(255,184,107) );

	QList<QTextEdit::ExtraSelection> extras;
	extras << highlight;
	m_asm->setExtraSelections( extras );
}

void Assembler::step_debug()
{
	int ret;
	QTableWidgetItem *item;
	m_ctx.timeout = 1;

	ret = dsb_vm_interpret_ctx(&m_ctx);

	if (ret == -1) {
		m_play->setIcon(QIcon(":/icons/control_play_blue.png"));
		m_play->setText("Play");
		m_step->setEnabled(false);
		m_debug->setEnabled(true);
		stop();
		m_ctx.ip++;

		char buf[100];
		dsb_nid_toStr(m_ctx.result, buf, 100);
		m_result->setText(QString("Result = %1").arg(buf));

		QTextEdit::ExtraSelection highlight;
		highlight.cursor = m_asm->textCursor();
		highlight.format.setProperty(QTextFormat::FullWidthSelection, false);

		QList<QTextEdit::ExtraSelection> extras;
		extras << highlight;
		m_asm->setExtraSelections( extras );
	}
	else
	{
		char buf[100];
		//Update register table.
		for (int i=0; i<16; i++)
		{
			dsb_nid_toStr(&m_ctx.reg[i],buf,100);
			item = new QTableWidgetItem(buf);
			m_regs->setItem(i,0,item);
		}

		QTextEdit::ExtraSelection highlight;
		highlight.cursor = QTextCursor(m_asm->document()->findBlockByLineNumber(m_ipline[m_ctx.ip]));
		highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
		highlight.format.setBackground( QColor(255,184,107) );

		QList<QTextEdit::ExtraSelection> extras;
		extras << highlight;
		m_asm->setExtraSelections( extras );
	}
}

void Assembler::save_asm()
{
	QString filename = QFileDialog::getSaveFileName(this,"Save Assembly",".","DSB Assembly (*.dsba)");
	QFile file(filename);
	if (file.open(QFile::WriteOnly | QFile::Text))
	{
		QTextStream s(&file);
		s << m_asm->toPlainText();
	}
}

void Assembler::load_asm()
{
	QString filename = QFileDialog::getOpenFileName(this,"Open Assembly",".","DSB Assembly (*.dsba)");
	QFile file(filename);
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream s(&file);
		m_asm->setPlainText(s.readAll());
	}
}

void Assembler::stop()
{
	m_running = false;
}

void Assembler::start()
{
	QTableWidgetItem *item;
	m_ctx.ip = 0;
	m_ctx.timeout = 10000;

	m_ctx.codesize = dsb_assemble(m_asm->toPlainText().toAscii().constData(),m_ctx.code,200);
	dsb_vm_interpret_ctx(&m_ctx);

	char buf[100];
	dsb_nid_toStr(m_ctx.result, buf, 100);
	m_result->setText(QString("Result = %1").arg(buf));

	//Update register table.
	for (int i=0; i<16; i++)
	{
		dsb_nid_toStr(&m_ctx.reg[i],buf,100);
		item = new QTableWidgetItem(buf);
		m_regs->setItem(i,0,item);
	}
}

