/*
 * main.cpp
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

#include <qt4/Qt/QtGui>
#include "eventlog.h"
#include "connectdiag.h"
#include "msglog.h"
#include "assembler.h"
#include "dsb/event.h"
#include "dsb/wrap.h"
#include "dsb/net.h"
#include "dsb/net_protocol.h"
#include "dsb/nid.h"
#include "dsb/names.h"
#include "dsb/globals.h"
#include "dsb/common.h"
#include <iostream>

MessageLogger *msglogger = 0;
void *hostsock = 0;

extern "C"
{
int dsb_send(Event_t *evt, int async)
{
	int res;
	char buf[200];

	//Record the event.
	dsb_event_pack(evt,buf,100);
	msglogger->addMessage(DSBNET_SENDEVENT,buf);

	res = dsb_net_send_event(hostsock, evt, async);


	if (((evt->type >> 8) != 0x1) && ((evt->flags & EVTFLAG_FREE) != 0))
	{
		dsb_event_free(evt);
	}

	return res;
}
}

int net_cb_result(void *sock, void *data)
{
	//int resid = *((int*)data);
	NID_t res;

	dsb_nid_unpack((const char*)((char*)data+sizeof(int)), &res);

	msglogger->addMessage(DSBNET_EVENTRESULT,data);

	//Call original event handler.
	//TODO, should check that this is the current event handler.
	dsb_net_cb_result(sock,data);

	return SUCCESS;
}

int net_cb_error(void *sock, void *data)
{
	msglogger->addMessage(DSBNET_ERROR,data);
	return 0;
}

int net_cb_base(void *sock, void *data)
{
	int count;

	msglogger->addMessage(DSBNET_BASE,data);

	//Update roots
	count = dsb_nid_unpack((const char*)data,&Root);
	//dsb_names_update("root",&Root);
	dsb_nid_unpack(&(((const char*)data)[count]),&PRoot);
	//dsb_names_update("proot",&PRoot);


	//dsb_names_rebuild();
	msglogger->delayedNamesRebuild();

	return SUCCESS;
}

int net_cb_debugevent(void *sock, void *data)
{
	msglogger->addMessage(DSBNET_DEBUGEVENT,data);
	return 0;
}

int net_cb_event(void *sock, void *data)
{
	msglogger->addMessage(DSBNET_SENDEVENT,data);
	return 0;
}

int main(int argc, char *argv[])
{
	QApplication qtapp(argc,argv);

	dsb_common_init();

	dsb_net_callback(DSBNET_EVENTRESULT,net_cb_result);
	dsb_net_callback(DSBNET_ERROR,net_cb_error);
	dsb_net_callback(DSBNET_BASE,net_cb_base);
	dsb_net_callback(DSBNET_SENDEVENT,net_cb_event);
	dsb_net_callback(DSBNET_DEBUGEVENT,net_cb_debugevent);

	msglogger = new MessageLogger();
	new ConnectDialog();
	new Assembler();

	QApplication::exec();

	dsb_common_final();

	return 0;
}
