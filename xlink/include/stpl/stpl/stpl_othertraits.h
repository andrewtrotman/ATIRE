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

#ifndef STPL_OTHERTRAITS_H_
#define STPL_OTHERTRAITS_H_

#include "stpl_doc.h"
#include "stpl_grammar.h"

namespace stpl {
	
	template <typename DocumentT>
	struct ScannerTypeTrait {
		typedef  Scanner<DocumentT>	scanner_type;
	};
	
	template <typename DocumentT, typename ScannerT>
	struct GrammarTypeTrait {
		typedef Grammar<DocumentT, ScannerT>	grammar_type;
	};		
	
	/*
	template <PlainDocument<>, typename ScannerT>
	struct GrammarTypeTrait<PlainDocument, ScannerT> {
		typedef PlainTextGrammar<DocumentT, ScannerT>	grammar_type;
	};	
	*/
}

#endif /*STPL_OTHERTRAITS_H_*/
