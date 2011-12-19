/*
	INDEXER_PARAM_BLOCK_PREGEN.H
	--------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_PREGEN_H_
#define INDEXER_PARAM_BLOCK_PREGEN_H_

#include "pregen.h"
#include "pregen_field_type.h"

#define MAX_PREGENS 100

/*
	class ANT_INDEXER_PARAM_BLOCK_PREGEN
	----------------------------------
*/
class ANT_indexer_param_block_pregen
{
public:
	struct pregen_field_spec
	{
	char *field_name;
	ANT_pregen_field_type type;
	} ;

public:
	pregen_field_spec pregens[MAX_PREGENS];	// fields to use in generating pregens
	long num_pregen_fields;					// number of pregen fields

protected:
	virtual void help(void);

public:
	ANT_indexer_param_block_pregen();
	virtual ~ANT_indexer_param_block_pregen();

	virtual int add_pregen_field(char *field_name, char *field_type);
} ;

#endif /* INDEXER_PARAM_BLOCK_PREGEN_H_ */
