/*
 * eventgen.h
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

#ifndef EVENTGEN_H_
#define EVENTGEN_H_

#include <qt4/QtGui/QWidget>
#include <qt4/QtGui/QLineEdit>
#include <qt4/QtGui/QComboBox>
#include <qt4/QtGui/QPushButton>
#include <qt4/QtGui/QLabel>
#include <dsb/event.h>

class EventGenerator : public QWidget
{
		Q_OBJECT

public:
		EventGenerator();
		~EventGenerator();

private:
		QLabel *m_typelabel;
		QLabel *m_destlabel;
		QComboBox *m_type;
		QLineEdit *m_dest1;
		QLabel *m_dest1vl;
		QLabel *m_dest2vl;
		QLineEdit *m_dest2;

		QLabel *m_deflabel;
		QLineEdit *m_def;
		QComboBox *m_eval;

		QLabel *m_deplabel;
		QLineEdit *m_dep1;
		QLineEdit *m_dep2;
		QLabel *m_defvl;
		QLabel *m_dep1vl;
		QLabel *m_dep2vl;

		QPushButton *m_send;
		QPushButton *m_cancel;

		Event_t m_event;

public slots:
	void dest1changed(const QString &);
	void dest2changed(const QString &);
	void defchanged(const QString &);
	void dep1changed(const QString &);
	void dep2changed(const QString &);
	void sendclicked();
	void cancelclicked();
	void typechanged(int index);
	void evalchanged(int index);
};


#endif /* EVENTGEN_H_ */
