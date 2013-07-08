/*
 * script.cpp
 *
 *  Created on: 8 Jul 2013
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

#include "dsb/ide/view.h"
#include "dsb/core/module.h"
#include "dsb/core/nid.h"
#include "dsb/patterns/pattern_types.h"
#include "script.h"
#include <QTextEdit>
#include <QToolBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QAction>
#include <iostream>

#include "lexer.c"
#include "scriptsyntax.h"

extern "C" int yyparse(void);
extern int yydsbvarid;

ScriptView::ScriptView()
 : DSBView()
{
	QWidget *central = new QWidget();
	QVBoxLayout *mainlayout = new QVBoxLayout();
	QSplitter *split = new QSplitter(Qt::Horizontal);
	central->setLayout(mainlayout);
	setCentralWidget(central);

	central->setAutoFillBackground(true);

	m_bar = new QToolBar();
	m_bar->addAction(QIcon(":/icons/script_save.png"),"Save");
	addToolBar(Qt::LeftToolBarArea,m_bar);
	connect(m_bar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolclick(QAction*)));

	mainlayout->addWidget(split);

	m_script = new QTextEdit();
	m_script->setFont(QFont("Courier New", 10));
	ScriptSyntax *syn = new ScriptSyntax(m_script->document());
	split->addWidget(m_script);

	setWindowTitle("DSB Script");
	setMinimumSize(750,380);
}

ScriptView::~ScriptView()
{

}

void ScriptView::addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h, int mode)
{

}

void ScriptView::clearHARCs()
{

}

void ScriptView::toolclick(QAction *a)
{
	if (a->text() == "Save")
	{
		yydsbvarid = 0;
		YY_BUFFER_STATE buf = yy_scan_string(m_script->toPlainText().toLatin1().constData());
		yyparse();
		std::cout << "Parsing: " << m_script->toPlainText().toLatin1().constData() << "\n";
		yy_delete_buffer(buf);
	}
}

static struct Module scriptmod;

static int script_init(const NID_t *base)
{
	DSBView::registerView<ScriptView>();
	DSBView::mapView<ScriptView>(DSB_PATTERN_BYTECODE);
	return 0;
}

static int script_final()
{
	return 0;
}


extern "C"
{
/*
 * Module registration structure.
 */
struct Module *dsb_scriptview_info()
{
	scriptmod.init = script_init;
	scriptmod.update = 0;
	scriptmod.final = script_final;
	scriptmod.ups = 0;
	return &scriptmod;
}
};


