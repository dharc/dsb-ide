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

#include <dsb/module.h>
#include <dsb/nid.h>
#include "dsb/ide/view.h"
#include <dsb/pattern_types.h>

#include "objectview.h"


ObjectView::ObjectView()
 : DSBView()
{

}

ObjectView::~ObjectView()
{

}

void ObjectView::addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h)
{

}

void ObjectView::clearHARCs()
{

}

static struct Module viewmod;

static int view_init(const NID_t *base)
{
	DSBView::registerView<ObjectView>();
	DSBView::mapView<ObjectView>(DSB_PATTERN_OBJECT);
	return 0;
}

static int view_final()
{
	return 0;
}


extern "C"
{
/*
 * Module registration structure.
 */
struct Module *dsb_objectview_info()
{
	viewmod.init = view_init;
	viewmod.update = 0;
	viewmod.final = view_final;
	viewmod.ups = 0;
	return &viewmod;
}
};

