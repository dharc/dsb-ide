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

#include "dsb/ide/view.h"
#include <qt4/QtCore/QHash>

#include <cstdio>

static QHash<QString,DSBView*(*)()> views;
static QHash<int,QString> patterns;

DSBView::DSBView()
	: QWidget()
{

}

DSBView::~DSBView()
{

}

void DSBView::registerViewInternal(DSBView *(*creator)(), const char *name)
{
	views.insert(name,creator);
}

void DSBView::mapViewInternal(const char *name, int pat)
{
	printf("Map pattern %x to: %s\n",pat,name);
	patterns.insert(pat, name);
}

DSBView *DSBView::createView(const char *type)
{
	DSBView *(*creator)() = views.value(type,0);
	if (creator != 0)
	{
		return creator();
	}
	return 0;
}

DSBView *DSBView::createView(int pat)
{
	QString view = patterns.value(pat,"");

	return createView(view.toAscii().constData());
}
