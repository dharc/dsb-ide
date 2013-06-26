/*
 * view.cpp
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

#include <dsb/core/module.h>
#include <dsb/core/nid.h>
#include "dsb/ide/view.h"
#include <dsb/wrap.h>
#include <dsb/patterns/string.h>
#include <dsb/patterns/pattern_types.h>
#include <dsb/patterns/pattern.h>
#include <dsb/algorithms/iterator.h>
#include <QPainter>

#include <cstdio>

#include "displayview.h"

DisplayView::DisplayView()
 : DSBView()
{
	m_height = 240;
	m_width = 320;
}

DisplayView::~DisplayView()
{

}

void DisplayView::addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h)
{
	//dsb_get(&t1,&t2,&m_obj);
	m_obj = h;
	dsb_getnzi(&m_obj,"width",&m_width);
	dsb_getnzi(&m_obj,"height",&m_height);

	if (m_width == 0) m_width = 100;
	if (m_height == 0) m_height = 100;

	repaint();
}

void DisplayView::clearHARCs()
{

}

void DisplayView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setBrush(Qt::black);
	painter.drawRect(0,0,m_width,m_height);

	//Loop over all child elements.
	struct DSBIterator it;
	const NID_t *cur;
	NID_t value;
	dsb_iterator_begin(&it,&m_obj);
	cur = dsb_iterator_next(&it);
	while (cur != 0)
	{
		dsb_get(&m_obj,cur,&value);
		drawElement(painter,&value);
		cur = dsb_iterator_next(&it);
	}
	dsb_iterator_end(&it);
}

void DisplayView::drawElement(QPainter &painter, const NID_t *ele)
{
	unsigned int type = dsb_pattern_what(ele);

	switch(type)
	{
	case DSB_PATTERN_SHAPE_LINE:	drawLine(painter,ele); break;
	case DSB_PATTERN_SHAPE_TEXT:	drawText(painter,ele); break;
	default: break;
	}
}

void DisplayView::drawLine(QPainter &painter, const NID_t *ele)
{
	int x1;
	int x2;
	int y1;
	int y2;
	dsb_getnzi(ele,"x1",&x1);
	dsb_getnzi(ele,"x2",&x2);
	dsb_getnzi(ele,"y1",&y1);
	dsb_getnzi(ele,"y2",&y2);

	painter.setPen(Qt::red);
	painter.drawLine(x1,y1,x2,y2);
}

void DisplayView::drawText(QPainter &painter, const NID_t *ele)
{
	int tx = 0;
	int ty = 0;
	char buf[500];
	NID_t text;
	dsb_getnzi(ele,"x",&tx);
	dsb_getnzi(ele,"y",&ty);
	dsb_getnzn(ele,"text",&text);
	dsb_string_ntoc(buf,500,&text);

	painter.setPen(Qt::red);
	painter.drawText(tx,ty,buf);
}

static struct Module dispmod;

static int disp_init(const NID_t *base)
{
	DSBView::registerView<DisplayView>();
	DSBView::mapView<DisplayView>(DSB_PATTERN_DISPLAY);
	return 0;
}

static int disp_final()
{
	return 0;
}


extern "C"
{
/*
 * Module registration structure.
 */
struct Module *dsb_displayview_info()
{
	dispmod.init = disp_init;
	dispmod.update = 0;
	dispmod.final = disp_final;
	dispmod.ups = 0;
	return &dispmod;
}
};

