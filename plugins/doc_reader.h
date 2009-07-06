#ifndef DOC_READER_H_
#define DOC_READER_H_

#include "uniseg_settings.h"
#include "uniseg_types.h"

class DocumentReader {
public:
	static DocumentReader* create(UNISEQ_settings::Format);
	virtual void read(const char* filename, string_type& stream) = 0;
};

class TextReader : public DocumentReader {
public:
	void read(const char* filename, string_type& stream);
};


class XmlReader : public DocumentReader {
public:
	void read(const char* filename, string_type& stream);
};

class TrecReader : public DocumentReader {
public:
	void read(const char* filename, string_type& stream);
};

#endif
