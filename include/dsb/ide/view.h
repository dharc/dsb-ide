/*
 * view.h
 *
 *  Created on: 15 Jun 2013
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

#ifndef VIEW_H_
#define VIEW_H_

/** @file view.h */

#include <qt4/QtGui/QMainWindow>
#include <dsb/nid.h>

#define DSBVIEW(A) static const char *type() { return #A; } \
	static A *create() { return new A(); }

class DSBView : public QMainWindow
{
	Q_OBJECT

public:
	DSBView();
	virtual ~DSBView();

	static const char *type() { return "DSBView"; }

	virtual const char *title()=0;
	virtual void addHARC(const NID_t &t1, const NID_t &t2, const NID_t &h)=0;
	virtual void clearHARCs()=0;

	template <typename T>
	static void registerView()
	{
		registerViewInternal((DSBView*(*)())T::create, T::type());
	}

	template <typename T>
	static void mapView(int pat)
	{
		mapViewInternal(T::type(), pat);
	}

	static DSBView *createView(const char *type);
	static DSBView *createView(int pat);

private:
	static void registerViewInternal(DSBView *(*creator)(), const char *name);
	static void mapViewInternal(const char *name, int pat);
};

#endif /* VIEW_H_ */
