/*
 * displayview.h
 *
 *  Created on: 17 Jun 2013
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

#ifndef DISPLAYVIEW_H_
#define DISPLAYVIEW_H_

#include "dsb/ide/view.h"
#include <dsb/core/nid.h>

class DisplayView : public DSBView
{
	Q_OBJECT

public:
	DSBVIEW(DisplayView);

	DisplayView();
	~DisplayView();

	void addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h, int mode);
	void clearHARCs();
	const char *title() { return "Display"; }

	void paintEvent(QPaintEvent *event);

	void drawElement(QPainter &painter, const NID_t *ele);
	void drawLine(QPainter &painter, const NID_t *ele);
	void drawText(QPainter &painter, const NID_t *ele);
	void drawImage(QPainter &painter, const NID_t *ele);

	const NID_t *getObject() { return &m_obj; }

private:
	int m_width;
	int m_height;
	NID_t m_obj;
};


#endif /* DISPLAYVIEW_H_ */
