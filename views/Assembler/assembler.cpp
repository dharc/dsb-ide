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
#include "dsb/core/vm.h"
#include "dsb/core/nid.h"
#include "dsb/patterns/array.h"
#include "dsb/errors.h"
#include "dsb/wrap.h"
#include "dsb/globals.h"
#include "dsb/patterns/string.h"
#include "dsb/assembler.h"
#include "dsb/patterns/array.h"
#include "dsb/ide/ide.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>
#include <QTextEdit>
#include <QTableWidget>
#include <QSplitter>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QCheckBox>
#include <iostream>

extern DSBIde *ide;

SaveObject::SaveObject(Assembler *a)
	: QWidget(0,Qt::Dialog)
{
	m_asm = a;

	QVBoxLayout *mainlayout = new QVBoxLayout();
	setLayout(mainlayout);

	m_obj = new QLineEdit();
	m_obj->setAutoFillBackground(true);
	mainlayout->addWidget(m_obj);

	m_source = new QCheckBox("Include source");
	mainlayout->addWidget(m_source);

	QHBoxLayout *buttonlayout = new QHBoxLayout();
	mainlayout->addLayout(buttonlayout);
	m_save = new QPushButton("Save");
	buttonlayout->addWidget(m_save);
	m_cancel = new QPushButton("Cancel");
	buttonlayout->addWidget(m_cancel);

	setWindowTitle("Save Script to Object");
	resize(600,400);

	connect(m_save, SIGNAL(clicked()), this, SLOT(saveclicked()));
	connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelclicked()));
}

SaveObject::~SaveObject()
{

}

void SaveObject::saveclicked()
{
	NID_t n;
	if (dsb_nid_fromStr(m_obj->text().toLatin1().constData(),&n) == 0)
	{
		m_asm->saveObject(n,m_source->isChecked());
		hide();
	}
}

void SaveObject::cancelclicked()
{
	hide();
}

Assembler::Assembler()
 : DSBView()
{
	QWidget *central = new QWidget();
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QSplitter *split = new QSplitter(Qt::Horizontal);
	central->setLayout(mainlayout);
	setCentralWidget(central);

	central->setAutoFillBackground(true);

	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/page.png"),"New");
	m_bar->addAction(QIcon(":/icons/folder_page.png"),"Open");
	m_bar->addAction(QIcon(":/icons/page_save.png"),"Save");
	m_bar->addSeparator();
	//m_bar->addAction(QIcon(":/icons/table_edit.png"),"Edit Object");
	m_bar->addAction(QIcon(":/icons/script_save.png"),"Save Object");
	m_bar->addSeparator();
	m_play = m_bar->addAction(QIcon(":/icons/control_play_blue.png"),"Run");
	m_debug = m_bar->addAction(QIcon(":/icons/bug.png"),"Debug");
	m_step = m_bar->addAction(QIcon(":/icons/resultset_next.png"),"Step");
	m_step->setEnabled(false);

	//mainlayout->addWidget(m_bar);
	//ide->addToolBar(m_bar);
	addToolBar(Qt::LeftToolBarArea,m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));

	mainlayout->addWidget(split);
	m_asm = new QTextEdit();
	m_asm->setFont(QFont("Courier New", 10));
	Syntax *syn = new Syntax(m_asm->document());
	split->addWidget(m_asm);

	m_tabs = new QTabWidget();

	m_regs = new QTableWidget();
	m_regs->setColumnCount(2);
	m_regs->setColumnWidth(1,200);
	m_regs->setRowCount(16);
	QStringList headers;
	headers.append("Name");
	headers.append("Value");
	m_regs->setHorizontalHeaderLabels(headers);
	m_tabs->addTab(m_regs, "Variables");

	m_labs = new QTableWidget();
	m_labs->setColumnCount(2);
	m_labs->setColumnWidth(1,200);
	m_labs->setRowCount(16);
	//QStringList headers;
	headers.clear();
	headers.append("Name");
	headers.append("Location");
	m_labs->setHorizontalHeaderLabels(headers);
	m_tabs->addTab(m_labs, "Labels");

	m_mem = new QTableWidget();
	m_mem->setColumnCount(1);
	m_mem->setRowCount(16);
	headers.clear();
	headers.append("Code");
	m_mem->setHorizontalHeaderLabels(headers);
	m_tabs->addTab(m_mem, "Code");

	split->addWidget(m_tabs);
	QList<int> sizes;
	sizes.append(2000);
	sizes.append(800);
	split->setSizes(sizes);

	m_result = new QLabel("Result = null");
	m_result->setMaximumHeight(50);
	mainlayout->addWidget(m_result);

	setWindowTitle("DSB Assembler");
	setMinimumSize(750,380);
	//show();

	m_running = false;
	m_ctx.code = new NID_t[1000];
	m_ctx.result = new NID_t;

	m_saveobj = new SaveObject(this);
}

Assembler::~Assembler()
{
	ide->removeToolBar(m_bar);
}

void Assembler::clearHARCs()
{

}

void Assembler::addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h, int mode)
{
	char buffer[10000];
	NID_t *array;
	int size;

	size = dsb_array_readalloc(&h,&array);
	if (size > 0)
	{
		dsb_disassemble(array,size,buffer,10000);
		m_asm->setText(buffer);
	}
	free(array);

	m_def = h;
	m_obj = t1;
	m_key = t2;
	m_mode = mode;
}

void Assembler::saveObject(const NID_t &n, bool incsrc)
{
	if (incsrc)
	{
		NID_t str;
		NID_t i;

		dsb_iton(100,&i);
		dsb_get(&n,&i,&str);
		if (dsb_nid_eq(&str,&Null) == 1)
		{
			dsb_new(&n,&str);
		}

		dsb_string_cton(&str,m_asm->toPlainText().toLatin1().constData());
	}

	m_ctx.codesize = dsb_assemble_array(m_asm->toPlainText().toLatin1().constData(),m_ctx.code,200);
	dsb_array_write(m_ctx.code,m_ctx.codesize,&n);
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
	else if (a->text() == "Save Object")
	{
		//m_saveobj->show();
		saveObject(m_def,false);

		if (m_mode == 1)
		{
			dsb_define(&m_obj,&m_key,&m_def);
		}
		else
		{
			dsb_set(&m_obj,&m_key,&m_def);
		}
	}
}

void Assembler::start_debug()
{
	m_ctx.timeout = 1;
	int line=0;
	bool error = false;
	int lip;

	struct AsmContext ctx;
	const char *source = m_asm->toPlainText().toLatin1().constData();
	const char *oldsource = source;

	//Initialise first assembly context
	ctx.ip = 0;
	ctx.labels = new VMLabel[MAX_LABELS];
	ctx.output = m_ctx.code;
	ctx.line = 1;
	ctx.error = 0;

	//Insert default labels.
	dsb_assemble_labels(ctx.labels,source);

	//For every line
	while(1)
	{
		lip = ctx.ip;
		//Assemble the line.
		dsb_assemble_line(&ctx,source);
		//Store which line corresponds to each instructions.
		m_ipline[lip] = line++;
		lip = ctx.ip;

		//Oops, highlight line as error.
		if (ctx.error == 1)
		{
			error = true;
			ctx.error = 0;
			QTextEdit::ExtraSelection highlight;
			highlight.cursor = QTextCursor(m_asm->document()->findBlockByLineNumber(ctx.line-1));
			highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
			highlight.format.setBackground( QColor(251,226,226) );
			QList<QTextEdit::ExtraSelection> extras;
			extras << highlight;
			m_asm->setExtraSelections( extras );
		}


		//Move to next line if there is one.
		source = strchr(source,'\n');
		if (source == 0) break;
		source++;
		ctx.line++;
	}

	//If error, don't bother to patch compile
	if (error == false)
	{
		//Now do a real compile with valid labels.
		source = oldsource;
		dsb_assemble_compile(&ctx, source);
	}

	m_ctx.codesize = ctx.ip;
	m_mem->setRowCount(ctx.ip);

	//Update memory table
	char buf[100];
	QTableWidgetItem *item;
	for (int i=0; i<m_ctx.codesize; i++)
	{
		dsb_nid_toStr(&m_ctx.code[i],buf,100);
		item = new QTableWidgetItem(buf);
		m_mem->setItem(i,0,item);
	}

	//Update variable names
	int labcount = 0;
	m_regs->setRowCount(16);
	for (int i=0; i<MAX_LABELS; i++)
	{
		if (ctx.labels[i].lip != -1 && ctx.labels[i].mode == 1)
		{
			item = new QTableWidgetItem(ctx.labels[i].label);
			m_regs->setItem(labcount,0,item);
			labcount++;
		}
	}
	m_regs->setRowCount(labcount);

	//Initialise default variables for the VM
	m_ctx.vars[0] = m_obj;
	m_ctx.vars[1] = m_key;
	m_ctx.vars[2] = m_def;
	//Update built-in variable table.
	for (int i=0; i<3; i++)
	{
		dsb_nid_toStr(&m_ctx.vars[i],buf,100);
		item = new QTableWidgetItem(buf);
		m_regs->setItem(i,1,item);
	}

	//Update label names
	labcount = 0;
	m_labs->setRowCount(16);
	for (int i=0; i<MAX_LABELS; i++)
	{
		if (ctx.labels[i].lip != -1 && ctx.labels[i].mode == 0)
		{
			item = new QTableWidgetItem(ctx.labels[i].label);
			m_labs->setItem(labcount,0,item);
			item = new QTableWidgetItem(QString().setNum(ctx.labels[i].lip));
			m_labs->setItem(labcount,1,item);
			labcount++;
		}
	}
	m_labs->setRowCount(labcount);

	delete [] ctx.labels;

	m_mem->setCurrentCell(0,0);

	if (error) return;

	if (m_running == false)
	{
		m_ctx.ip = 0;
		m_running = true;
		dsb_nid_null(m_ctx.result);
	}

	QTextEdit::ExtraSelection highlight;
	highlight.cursor = QTextCursor(m_asm->document()->findBlockByLineNumber(m_ipline[0]));
	highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
	highlight.format.setBackground( QColor(226,234,251) );

	QList<QTextEdit::ExtraSelection> extras;
	extras << highlight;
	m_asm->setExtraSelections( extras );
}

void Assembler::step_debug()
{
	int ret;
	QTableWidgetItem *item;
	m_ctx.timeout = 1;

	ret = dsb_vm_interpret(&m_ctx);

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
		//Update variable table.
		for (int i=0; i<16; i++)
		{
			dsb_nid_toStr(&m_ctx.vars[i],buf,100);
			item = new QTableWidgetItem(buf);
			m_regs->setItem(i,1,item);
		}

		QTextEdit::ExtraSelection highlight;
		highlight.cursor = QTextCursor(m_asm->document()->findBlockByLineNumber(m_ipline[m_ctx.ip]));
		highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
		highlight.format.setBackground( QColor(226,234,251) );

		m_mem->setCurrentCell(m_ctx.ip,0);

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

	m_ctx.codesize = dsb_assemble_array(m_asm->toPlainText().toLatin1().constData(),m_ctx.code,200);
	dsb_vm_interpret(&m_ctx);

	char buf[100];
	dsb_nid_toStr(m_ctx.result, buf, 100);
	m_result->setText(QString("Result = %1").arg(buf));

	//Update register table.
	for (int i=0; i<16; i++)
	{
		dsb_nid_toStr(&m_ctx.vars[i],buf,100);
		item = new QTableWidgetItem(buf);
		m_regs->setItem(i,0,item);
	}
}

