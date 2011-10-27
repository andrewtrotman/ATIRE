
#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include "memory.h"
#include "search_engine.h"
#include "btree_iterator.h"
#include "search_engine_btree_leaf.h"

#define FLUSH_CACHE_BEFORE_QUERY 0
#define USE_O_DIRECT 0

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define VOCAB_FILENAME "vocab_list.bin"

typedef enum DICT_TYPE {NONE=0, ALL, FIXED, STRING, BLOCKED, EMBED, EMBEDFRONT, EMBEDFIXED, EMBEDFIXED_NULL, EMBEDVAR, EMBEDVAR_V2, ORIG} dict_type_t;
typedef enum OPERATION {BUILD_OPT=1, SERIALISE_OPT} operation_t;
typedef enum SEARCH_OPT {ONDISK=1, INMEMORY} search_t;

extern int num_of_sectors;
extern int block_size;

#define PROBE_PHYSICAL_SECTOR_SIZE 0
extern size_t hd_sector_size;

//
// The fixed strucuture for a term has a size of 32 bytes, which is
// the max term length (24 bytes including the null terminator) plus the
// b bytes postings pointer.
//
// To evenly divide the fixed structure in a sector, we decided the max
// word length is 23 without including the null terminator.
//
#define MAX_WORD_LENGTH 23

#define POSTING_PTR_SIZE 8     // 8 bytes, 64 bits
#ifndef COMMON_PREFIX_SIZE
	#define COMMON_PREFIX_SIZE 4 // defined in the make file
#endif
#define COMMON_PREFIX_SIZE_ONE_LESS (COMMON_PREFIX_SIZE - 1)
typedef uint64_t node_ptr_t;    // 8 bytes
#define NULL_NODE_PTR 0xFFFFFFFFFFFFFFFF
typedef uint32_t node_length_t; // 4 bytes
typedef uint32_t vocab_loc_t;   // 4 bytes

#define DICT_FIXED_FILENAME "dict.fixed"
#define DICT_STRING_FILENAME "dict.string"
#define DICT_BLOCKED_FILENAME "dict.blocked"
#define DICT_EMBED_FILENAME "dict.embed"
#define DICT_EMBEDFRONT_FILENAME "dict.embedfront"
#define DICT_EMBEDFIXED_FILENAME "dict.embedfixed"
#define DICT_EMBEDFIXED_NULL_FILENAME "dict.embedfixed-null"
#define DICT_EMBEDVAR_FILENAME "dict.embedvar"
#define DICT_EMBEDVAR_V2_FILENAME "dict.embedvar_v2"

#define SERIALISE_FIXED_FILENAME "serialised.fixed"
#define SERIALISE_ORIG_FILENAME "serialised.orig"
#define SERIALISE_STRING_FILENAME "serialised.string"
#define SERIALISE_BLOCKED_FILENAME "serialised.blocked"
#define SERIALISE_EMBED_FILENAME "serialised.embed"
#define SERIALISE_EMBEDFRONT_FILENAME "serialised.embedfront"
#define SERIALISE_EMBEDFIXED_FILENAME "serialised.embedfixed"
#define SERIALISE_EMBEDFIXED_NULL_FILENAME "serialised.embedfixed-null"
#define SERIALISE_EMBEDVAR_FILENAME "serialised.embedvar"
#define SERIALISE_EMBEDVAR_V2_FILENAME "serialised.embedvar_v2"

typedef struct stats {
	unsigned long long  io_time;
	unsigned long long  search_time;
	struct timeval start, end;
	unsigned long long bytes_read;
} stats_t;

extern stats_t stats;

extern void add_io_time();
extern void add_search_time();
extern void add_bytes_read(unsigned long long);


typedef struct term {
	char term[MAX_WORD_LENGTH+1];
	// used in embedvar
	char is_single;
} term_t;

typedef struct dict_fixed {
	char term[MAX_WORD_LENGTH+1];
	char dummy_postings_ptr[POSTING_PTR_SIZE];
} dict_fixed_t;

typedef struct dict_fixed_header {
	char term[MAX_WORD_LENGTH+1];
	node_ptr_t node_ptr;
} dict_fixed_header_t;


typedef struct dict_string {
	char term[MAX_WORD_LENGTH+1];
	node_ptr_t node_ptr;
} dict_string_t;

typedef struct dict_blocked {
	char term[MAX_WORD_LENGTH+1];
	node_ptr_t node_ptr;
} dict_blocked_t;

typedef struct dict_embed {
	char term[MAX_WORD_LENGTH+1];
	node_ptr_t node_ptr;
} dict_embed_t;

typedef struct dict_embedfront {
	char term[MAX_WORD_LENGTH+1];
	node_ptr_t node_ptr;
} dict_embedfront_t;

typedef struct dict_embedfixed {
    char prefix[COMMON_PREFIX_SIZE];
    node_ptr_t node_ptr;
} dict_embedfixed_t;

typedef struct dict_embedfixed_null {
    char *prefix;
    long long prefix_length;
    node_ptr_t node_ptr;
} dict_embedfixed_null_t;

typedef struct dict_embedvar {
    char prefix[MAX_WORD_LENGTH+1];
    node_ptr_t node_ptr;
} dict_embedvar_t;

extern void probe_hd_sector_size();

extern void build_fixed();
extern void build_string();
extern void build_blocked();
extern void build_embed();
extern void build_embedfront();
extern void build_embedfixed();
extern void build_embedfixed_null();
extern void build_embedvar();
extern void build_embedvar_v2();

extern void serialise_fixed();
extern void serialise_string();
extern void serialise_blocked();
extern void serialise_embed();
extern void serialise_embedfront();
extern void serialise_embedfixed();
extern void serialise_embedfixed_null();
extern void serialise_embedvar();
extern void serialise_embedvar_v2();

extern void search_fixed_ondisk_init();
extern int search_fixed_ondisk(char *);
extern void search_fixed_inmemory_init();
extern int search_fixed_inmemory(char *);

extern void search_string_ondisk_init();
extern int search_string_ondisk(char *);
extern void search_string_inmemory_init();
extern int search_string_inmemory(char *);

extern void search_blocked_ondisk_init();
extern int search_blocked_ondisk(char *);
extern void search_blocked_inmemory_init();
extern int search_blocked_inmemory(char *);

extern void search_embed_ondisk_init();
extern int search_embed_ondisk(char *);
extern void search_embed_inmemory_init();
extern int search_embed_inmemory(char *);

extern void search_embedfront_ondisk_init();
extern int search_embedfront_ondisk(char *);
extern void search_embedfront_inmemory_init();
extern int search_embedfront_inmemory(char *);

extern void search_embedfixed_ondisk_init();
extern int search_embedfixed_ondisk(char *);
extern void search_embedfixed_inmemory_init();
extern int search_embedfixed_inmemory(char *);

extern void search_embedfixed_null_ondisk_init();
extern int search_embedfixed_null_ondisk(char *);
extern void search_embedfixed_null_inmemory_init();
extern int search_embedfixed_null_inmemory(char *);

//extern void search_embedvar_ondisk_init();
//extern int search_embedvar_ondisk(char *);

//extern void search_embedvar_v2_ondisk_init();
//extern int search_embedvar_v2_ondisk(char *);

int run_command(const char *the_command);

#endif
