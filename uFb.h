/*
 * uFb 0.2
 * The Lemon Man (C) 2010
 * A simple framebuffer-based console.
 * Using "msx" font stolen by PSPSDK :D
 * Contacts:
 * #HACKERCHANNEL @ freenode
 */
 
#ifndef UFB_H
#define UFB_H

#include "font.h"
#include <stdarg.h>

#define uFB_VERSION "0.2"

typedef struct 
{
	int  (*devInit) (unsigned int width, unsigned int height); /* Callback to the hardware init function */
	void (*devDraw) (unsigned int x, unsigned int y, unsigned int pixel); /* Callback to the pixel plotter */
	void (*devFlip) (); /* Callback to the flipping function */
	unsigned int width, height; /* Framebuffer size */
	unsigned int conX, conY; /* Console coords */
} _uFbDev;

static _uFbDev dev;

static int uFbInit (unsigned int width, unsigned int height, 
	int  (*devInit) (unsigned int width, unsigned int height),
	void (*devDraw) (unsigned int x, unsigned int y, unsigned int pixel),
	void (*devFlip) ()
)
{
	if (!devInit || !devDraw)
		return 0;
	
	dev.width = width;
	dev.height = height;
	
	dev.conX = dev.conY = 0;
	
	dev.devInit = devInit;
	dev.devDraw = devDraw;
	dev.devFlip = devFlip;
	
	return dev.devInit(width, height);
}

static void uFbDrawEnd ()
{
	if (dev.devFlip)
		dev.devFlip();
}

inline void _scrClr ()
{
	int row, col;
	
	for (row=0;row<dev.height;row++)
		for (col=0;col<dev.width;col++)
			dev.devDraw(col, row, 0x00000000);
}

void _setTermCoords (unsigned int x, unsigned int y)
{
	dev.conX = x;
	dev.conY = y;
}

inline void _pChar (unsigned char c)
{
	int bitPos, line;
		
	switch (c)
	{
		case '\r':
			return;
		case '\n':
			dev.conX = 0;
			dev.conY++;
			goto skipPrint;
		case '\t':
			dev.conX += 2;
			goto skipPrint;
		default:
			goto printChar;
	}

printChar:

	for (line = 0; line < 8; line ++)
		for (bitPos = 0; bitPos < 8; bitPos ++)
			dev.devDraw(dev.conX * 8 + bitPos, dev.conY * 8 + line, ((__uFbFont[c][line] >> (7 - bitPos)) & 1) ? 0xFFFFFFFF : 0x00000000);
				
	dev.conX++;
	
skipPrint:	
	
	if (dev.conX > (dev.width / 8) - 1)
	{
		dev.conX = 0;
		dev.conY++;
	}
	
	if (dev.conY > (dev.height / 8) - 1)
		dev.conY = 0;
}

void _pStr (char *str)
{
	while (*str)
		_pChar(*str++);
}

inline void _my_itoa(int n, char *s) 
{
	char i;
	int n1;
	
	if (n<0) 
	{
		n = -n;
		*s++ = '-';
	}
	
	do
	{
		n1=n;
		i=0;
		while (1) 
		{
			if (n1<=9)   
			{
				*s++ = n1+'0';
				break;
			}
			n1=n1/10;
			i++;
		}
		while (i)
		{
			i--;
			n1 = n1*10;
		}
		n -= n1;
	} while (n);
	
	*s++ = 0;
}


#define isDigit(c) (c >= '0' && c <= '9')

char *parseBracket (char *str)
{
	char *p = str;
	int args[4], argc = 0;
	
	do {
		if (*p == ';')
			*p++;
			
		args[argc] = 0;
			
		while (isDigit(*p))
		{
			args[argc] = args[argc] * 10 + (*p - '0');
			*p++;
		}
		
		if (argc > 4)
			return p;
			
		argc++;
	} while(*p == ';');
	
	if (!*p)
		return p;
		
	switch (*p)
	{
		case 'H':
			if (argc != 2)
				return p;
			_setTermCoords(args[0], args[1]);
			break;
		case 'J':
			_scrClr();
			_setTermCoords(0,0);
			break;
	}
	
	*p++;
	
	return p;
}

void fprint(char *fmt, ...)
{
	va_list argp;	
	char *s, *p, t; int i; char tmp[0x10];
	
	va_start(argp, fmt);

	for(p = fmt; *p != '\0'; p++)
	{
		if(*p == 0x1b)
		{
			if (*++p == '[')
			{
				*p++;
				p = parseBracket(p);
			}
		}
		
		if (!*p)
			break;
		
		if(*p != '%')
		{
			_pChar(*p);
			continue;
		}
		
		switch (*++p)
		{
			case 's':
				s = va_arg(argp, char *);
				_pStr(s);
				break;
			case 'c':
				t = va_arg(argp, int);
				_pChar(t);
				break;
			case 'i':
				i = va_arg(argp, int);
				_my_itoa(i, tmp);
				_pStr(tmp);
				break;
		}
	}
	
	va_end(argp);
}


#endif
