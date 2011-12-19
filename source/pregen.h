/*
	PREGEN.H
	--------
*/
#ifndef PREGEN_H_
#define PREGEN_H_

#include <stdio.h>
#include "file.h"
#include "pregen_field_type.h"
#include "pregen_t.h"

/*
	class ANT_PREGEN
	----------------
*/
class ANT_pregen
{
protected:
	ANT_file file;

public:
	char *field_name;
	ANT_pregen_field_type type;

	ANT_pregen_t *scores;
	uint32_t doc_count;

public:
	ANT_pregen() : field_name(NULL), scores(NULL) {};
	ANT_pregen(ANT_pregen_field_type type, const char * name);
	virtual ~ANT_pregen();

	int read(const char * filename);

	static const char *pregen_type_to_str(ANT_pregen_field_type type);
} ;

#endif /* PREGEN_H_ */
