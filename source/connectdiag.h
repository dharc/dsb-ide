/*
 * connectdiag.h
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

#ifndef CONNECTDIAG_H_
#define CONNECTDIAG_H_

#include <qt4/QtGui/QWidget>
#include <qt4/QtGui/QLineEdit>

class QPushButton;
class QLabel;

class ConnectDialog : public QWidget
{
		Q_OBJECT

public:
		ConnectDialog();
		~ConnectDialog();

		QString getHost() { return m_host->text(); }

private:
		QLabel *m_message;
		QLabel *m_urllabel;
		QLabel *m_userlabel;
		QLabel *m_passlabel;
		QLineEdit *m_host;
		QLineEdit *m_user;
		QLineEdit *m_pass;
		//QLineEdit *m_port;
		QPushButton *m_ok;
		QPushButton *m_cancel;

public slots:
		void connectclicked();
		void cancelclicked();
};



#endif /* CONNECTDIAG_H_ */
