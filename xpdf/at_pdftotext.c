//========================================================================
//
// pdftotext.cc
//
// Copyright 1997-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "parseargs.h"
#include "GString.h"
#include "gmem.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "Error.h"
#include "config.h"

int main(int argc, char *argv[])
{
PDFDoc *doc;
GString *fileName;
GString *textFileName;
TextOutputDev *textOut;
int firstPage, lastPage;
char cfgFileName[256] = "";

// parse args
if (argc < 3 )
	exit(printf("Usage:%s <infile.pdf> <outfile.txt>\n", argv[0]));

fileName = new GString(argv[1]);

// read config file
globalParams = new GlobalParams(cfgFileName);
globalParams->setErrQuiet(gTrue);

doc = new PDFDoc(fileName, NULL, NULL);

// construct text file name
textFileName = new GString(argv[2]);

firstPage = 1;
lastPage = doc->getNumPages();

textOut = new TextOutputDev(textFileName->getCString(), gFalse, gFalse, gFalse);
if (textOut->isOk())
	doc->displayPages(textOut, firstPage, lastPage, 72, 72, 0, gFalse, gTrue, gFalse);

delete textOut;

delete textFileName;
delete doc;
delete globalParams;

// check for memory leaks
Object::memCheck(stderr);
gMemReport(stderr);

return 0;
}
