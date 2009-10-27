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

#ifndef STPL_TEXT_H_
#define STPL_TEXT_H_

#include <string>
#include "../stpl/stpl_doc.h"
#include "../stpl/stpl_word.h"
#include "../stpl/stpl_stream.h"
#include "../stpl/stpl_grammar.h"
#include "../stpl/stpl_parser.h"

namespace stpl {

	namespace TEXT {
		typedef Document<Word<> > PlainTextDocument;
		typedef GeneralGrammar<PlainTextDocument> PlainTextGrammar;

		typedef PlainTextDocument::entity_type PlainTextEntityType;
		typedef Scanner<PlainTextEntityType> PlainTextScanner;

		typedef Parser<PlainTextGrammar
						, PlainTextDocument
						, PlainTextEntityType
						, PlainTextScanner
						> PlainTextParser;


		typedef Word<std::string, FileStream<>::iterator> file_word;

		class TextFile {
			private:
				file_word line_;
				FileStream<>::iterator current_;
				FileStream<>::iterator end_;
				FileStream<> file_stream_;

			public:
				TextFile() {
					init();
				}
				TextFile(std::string filename) {
					init();
					read(filename);
				}
				~TextFile() {}

				void read(std::string filename) {
					file_stream_.read(filename);
					current_ = file_stream_.begin();
				}

				std::string next_line() {
					if (current_ != (end_ = file_stream_.end())) {
						line_.begin(current_);
						line_.end(end_);
						line_.match();
						current_ = line_.end();

						std::string rline(line_.begin(), line_.end());
						if (!(rline == "\r\n" || rline == "\n" || rline == "\n\r"))
							return rline;
					}
					return std::string("");
				}

			private:
				void init() {
					line_.separator("\n\r");
				}
		};
	}
}

#endif /*STPL_TEXT_H_*/
