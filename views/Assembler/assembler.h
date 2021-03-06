/*
 * assember.h
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

#ifndef IDEASSEMBLER_H_
#define IDEASSEMBLER_H_

#include "dsb/core/vm.h"
#include "dsb/core/nid.h"
#include "dsb/ide/view.h"

class QAction;
class QToolBar;
class QTextEdit;
class QTableWidget;
class QLabel;
class QTabWidget;
class QPushButton;
class QLineEdit;
class QCheckBox;

class Assembler;

class SaveObject : public QWidget
{
	Q_OBJECT

public:
	SaveObject(Assembler *a);
	~SaveObject();

private:
	QLineEdit *m_obj;
	QCheckBox *m_source;
	QPushButton *m_save;
	QPushButton *m_cancel;
	Assembler *m_asm;

public slots:
	void saveclicked();
	void cancelclicked();
};

class Assembler : public DSBView
{
	Q_OBJECT

public:
	DSBVIEW(Assembler);

	Assembler();
	~Assembler();

	const char *title() { return "Assembler"; }
	void clearHARCs();
	void addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h, int mode);

	//void setObject(const NID_t &obj);
	void saveObject(const NID_t &obj, bool incsrc);

private:
	QToolBar *m_bar;
	QTextEdit *m_asm;
	QTableWidget *m_regs;
	QTableWidget *m_mem;
	QTableWidget *m_labs;
	QLabel *m_result;
	QAction *m_play;
	QAction *m_debug;
	QAction *m_step;
	QTabWidget *m_tabs;
	SaveObject *m_saveobj;
	NID_t m_def;
	NID_t m_obj;
	NID_t m_key;
	int m_mode;

	bool m_running;
	struct VMContext m_ctx;
	int m_ipline[1000];

	void start_debug();
	void step_debug();
	void save_asm();
	void load_asm();
	void stop();
	void start();

public slots:
	void toolclick(QAction *);
};

#endif /* ASSEMBER_H_ */
