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
#include <dsb/event.h>
#include <dsb/wrap.h>
#include <dsb/net.h>
#include <dsb/net_protocol.h>
#include <dsb/nid.h>
#include <iostream>

EventLogger *evtlogger = 0;
int hostsock = 0;

int net_msg_result(int sock, void *data)
{
	struct DSBNetEventResult *res = (struct DSBNetEventResult*)data;
	evtlogger->updateEvent(res->id, &(res->res));
	return 0;
}

extern "C"
{
int dsb_send(Event_t *evt, int async)
{
	int count = 0;
	int ix;
	//struct DSBNetEventSend msg;

	//Record the event.
	evtlogger->addEvent(evt);

	//msg.evt = *evt;
	//Actually send the event
	dsb_net_send(hostsock, DSBNET_SENDEVENT, evt);

	//Block if we need a response.
	if ((async == 0) && (evt->type == EVENT_GET))
	{
		while (((evt->flags & EVTFLAG_DONE) == 0) && count < 10)
		{
			dsb_net_poll(100);
			count++;
		}

		//TODO Report a timeout failure
	}

	return 0;
}
}

int main(int argc, char *argv[])
{
	QApplication qtapp(argc,argv);

	dsb_net_init();
	dsb_net_callback(DSBNET_EVENTRESULT,net_msg_result);
	hostsock = dsb_net_connect("localhost");
	evtlogger = new EventLogger();

	NID_t a;
	dsb_nid(NID_INTEGER,2,&a);
	dsb_get(&a,&a,&a);


	QApplication::exec();

	dsb_net_final();

	return 0;
}
