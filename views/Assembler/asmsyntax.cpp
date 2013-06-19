/*
 * asmsyntax.cpp
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

#include "asmsyntax.h"

static const char *keywords[] = {
		"add",
		"sub",
		"div",
		"mul",
		"shl",
		"shr",
		"and",
		"or",
		"xor",
		"neg",
		"inc",
		"dec",
		"clr",
		"int",
		"flt",
		"cpy",
		"ret",
		"jmp",
		"jeq",
		"jne",
		"jle",
		"jge",
		"jlt",
		"jgt",
		"get",
		"def",
		"new",
		"del",
		"path",
		"pathd",
		"jisa",
		"jisnt",
		"sclone",
		"dclone",
		"const",
		0
};

Syntax::Syntax(QTextDocument *textEdit, QListWidget *kw)
	: QSyntaxHighlighter(textEdit)
{
	m_keylist = kw;

	font_label.setFont(QFont("Courier New", 11, QFont::Bold));
	font_comment.setFont(QFont("Courier New", 11));
	font_normal.setFont(font_comment.font());
	font_number.setFont(font_normal.font());
	font_char.setFont(font_number.font());
	font_op.setFont(font_label.font());
	font_reg.setFont(font_number.font());

	font_label.setForeground(QColor(38,140,223));
	font_comment.setForeground(QColor(130,130,130));
	font_normal.setForeground(QColor("black"));
	font_number.setForeground(QColor("blue"));
	font_char.setForeground(QColor(255,25,241));
	font_op.setForeground(QColor("black"));
	font_reg.setForeground(QColor("darkred"));
}

Syntax::~Syntax()
{

}

void Syntax::highlightBlock ( const QString & text )
{
	int pos = 0;
	int beg = 0;
	//bool istring;
	unsigned int i;
	const char *text2 = (const char*)text.toAscii();
	const char *kword;

	//bool start = true;

	//char buffer[1000];
	//buffer[0] = 0;

	while (pos < text.length())
	{
		//Remove all white space from the buffer, reguardless of whether its in peek mode.
		beg = pos;
		while ((pos < text.length()) && (text2[pos] == ' ' || text2[pos] == '\n' || text2[pos] == '\r' || text2[pos] == '\t'))
		{
			pos++;
		}
		if (beg != pos) setFormat(beg,pos-beg, font_normal);

		if (pos == text.length()) return;
		if (text2[pos] == 0) return;

		switch(text2[pos])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':	beg = pos;
					pos++;
					while (text2[pos] >= '0' && text2[pos] <= '9') pos++;
					setFormat(beg,pos-beg,font_number);
					break;

		case '$':	beg = pos;
					pos++;
					while (text2[pos] != 0 && text2[pos] != ' ' && text2[pos] != '\n' && text2[pos] != '\t' && text2[pos] != '+' && text2[pos] != '-') pos++;
					setFormat(beg,pos-beg,font_reg);
					break;

		case '#':	beg = pos;
					pos++;
					while (text2[pos] != 0 && text2[pos] != '\n') pos++;
					setFormat(beg,pos-beg,font_comment);
					break;

		case ':':	beg = pos;
					pos++;
					while (text2[pos] != 0 && text2[pos] != ' ' && text2[pos] != '\n' && text2[pos] != '\t' && text2[pos] != '+' && text2[pos] != '-') pos++;
					setFormat(beg,pos-beg,font_label);
					break;

		default:	kword = keywords[0];
					i = 0;
					while (kword != 0)
					{
						int len = strlen(kword);
						if ((text.length()-pos >= len) && (strncmp(&text2[pos],kword,len) == 0))
						{
							beg = pos;
							pos += len;
							setFormat(beg, pos-beg, font_op);
							break;
						}
						kword = keywords[++i];
					}
					break;
		}

		pos++;
	}
}

