/*
	HASH_TABLE.H
	------------
*/

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#define NCBI_HASH_TABLE_SIZE 0x100 // 0x00 - 0xFF inclusive

class NCBI_tree;

extern unsigned int NCBI_hash(const char *string, unsigned int seed);
extern unsigned int NCBI_hash(const char *string, long len, unsigned int seed);
extern unsigned int NCBI_random_hash(const char *string);
extern unsigned int NCBI_random_hash(const char *string, long len);
extern unsigned long NCBI_random_hash_16(const char *string);
extern unsigned long NCBI_random_hash_16(const char *string, long len);

/*
	class NCBI_HASH_TABLE
	---------------------
*/
class NCBI_hash_table
{
friend class NCBI_hash_table_iterator;

protected:
	NCBI_tree *chain[NCBI_HASH_TABLE_SIZE];

protected:
	virtual unsigned int random_hash(const char *text);

public:
	NCBI_hash_table();
	virtual ~NCBI_hash_table();

	virtual void *get(const char *name);
	virtual void add(const char *name, void *data);
	virtual void set(const char *name, void *data, int kill = 0, int kill_name = 0);
	virtual void set(const char *name, long value);
	virtual void set(const char *name, const char *data, int kill = 0, int kill_name = 0);
	virtual void remove(const char *name);
} ;

#endif __HASH_TABLE_H__

