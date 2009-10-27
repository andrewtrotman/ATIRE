/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * (c) Copyright 2007-2008 TYONLINE TECHNOLOGY PTY. LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU LESSER GENERAL PUBLIC LICENSE, Version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *******************************************************************************
 *
 * @author				Ling-Xiang(Eric) Tang
 *
 *******************************************************************************/

#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "../stpl/stpl_doc.h"
#include "../stpl/stpl_scanner.h"
#include "../stpl/stpl_grammar.h"
#include "../stpl/stpl_parser.h"

#include "../stpl/misc/stpl_trec.h"

#include "../stpl/stpl_unigrammar.h"

namespace stpl
{
	typedef	Document<>	DefaultDocument;

	namespace TREC
	{
		typedef TrecDocGrammar<DefaultDocument>					DefaultTrecDocGrammar;
		typedef Parser<DefaultTrecDocGrammar, DefaultDocument>	DefaultTrecDocParser;
	}

	namespace UNICODE {
		typedef ChineseGrammar<DefaultDocument>					DefaultChineseGrammar;
		typedef Parser<DefaultChineseGrammar, DefaultDocument>	DefaultChineseParser;
	}
}

#endif /* DOCUMENT_H_ */
