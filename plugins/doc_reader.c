#include "doc_reader.h"

#include <stpl/xml/stpl_xml.h>
#include <stpl/stpl_stream.h>

#include <stpl/misc/stpl_trec.h>
#include <algorithm>

using namespace stpl;
using namespace std;

DocumentReader* DocumentReader::create(UNISEQ_settings::Format format) {
	switch (format) {
	case UNISEQ_settings::TEXT:
		return reinterpret_cast<DocumentReader*>(new TextReader);

	case UNISEQ_settings::XML:
		return reinterpret_cast<DocumentReader*>(new XmlReader);

	case UNISEQ_settings::TREC:
		return reinterpret_cast<DocumentReader*>(new TrecReader);

	default:
		break;
	}
	return NULL;
}

void TextReader::read(const char* filename, string_type& stream) {
	stpl::FileStream file_stream(filename);
	string_type temp(file_stream.begin(), file_stream.end());
	stream = temp;
	//std::copy(file_stream.begin(), file_stream.end(), stream.begin());
	//cout << "stream: " << stream << endl;
}

void XmlReader::read(const char* filename, string_type& stream) {
	stpl::FileStream file_stream(filename);

	stpl::XML::XParser<string_type, FileStream::iterator> parser(file_stream.begin(), file_stream.end());
	//typedef XML::XParser<string, FileStream::iterator>::root_type element_type;

	parser.parse();

	std::map<std::string, bool> nm;
	nm.insert(make_pair(string("languagelink"), false));
	nm.insert(make_pair(string("TEXT"), true));
	nm.insert(make_pair(string("article"), true));

	//parser.root()->all_text(counter.stream());
	parser.root()->all_text(stream, nm, false, true);
	if (UNISEQ_settings::instance().do_debug()) {
		cout << __FILE__ << ":" << __LINE__ << endl;
		cout << "stream: " << stream << endl;
	}
}

void TrecReader::read(const char* filename, string_type& stream) {
	typedef	Document<StringBound<string_type, FileStream::iterator> >			trec_document;
	typedef TREC::TrecDocGrammar<trec_document>									trec_grammar;
	typedef Parser<trec_grammar, trec_document>									trec_parser;

	FileStream file_stream(filename);

	trec_parser parser(file_stream.begin(), file_stream.end());
	trec_parser::document_type& doc = parser.doc();
	parser.parse();

	trec_parser::document_type::entity_iterator iter;
	typedef trec_parser::string_type	string_type;
	typedef	trec_grammar::node_type	node_type;
	for (iter = doc.iter_begin(); iter != doc.iter_end(); iter++) {
		node_type* node_ptr = static_cast<node_type*>((*iter));

		/// getting some of the text we want
		std::map<string_type, bool> nm;
		nm.insert(make_pair(string_type("TEXT"), true));
		node_ptr->all_text(stream, nm);
	}
}
