/*
	WORDNET.H
	---------
*/
#ifndef WORDNET_H_
#define WORDNET_H_

#include "../../source/fundamental_types.h"
#include "thesaurus.h"

class ANT_file;
class ANT_thesaurus_rootnode;

/*
	class ANT_WORDNET
	-----------------
*/
class ANT_wordnet : public ANT_thesaurus
{
public:
	static const uint32_t ANT_ID_THESAURUS_SIGNATURE_MAJOR = 0x54505341;	//"ASPT" (Intel Byte Order)
	static const uint32_t ANT_ID_THESAURUS_SIGNATURE_MINOR = 0x00000003;	// version 0.3		BCD Major / Minor version number
	static const uint64_t ANT_ID_THESAURUS_WORDNET = 0x54454e44524f5700;	//"WORDNET"(Intel Byte Order)

private:
	ANT_file *file;						// a pointer to the disk file (which might be in-memory)
	char *root_buffer;					// the contents of the root of the tree
	ANT_thesaurus_rootnode *root;		// the root of the thesaurus tree
	
public:
	ANT_wordnet(char *filename);
} ;


#endif /* WORDNET_H_ */
