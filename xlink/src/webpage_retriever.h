/*
 * webpage_retriever.h
 *
 *  Created on: Dec 31, 2009
 *      Author: monfee
 */

#ifndef WEBPAGE_RETRIEVER_H_
#define WEBPAGE_RETRIEVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "pattern_singleton.h"

namespace QLINK {
	struct MemoryStruct {
	  char *memory;
	  size_t size;
	};

	class webpage_retriever : public pattern_singleton<webpage_retriever>
	{
	private:
		  CURL *curl_handle;

		  struct MemoryStruct chunk;

	public:
		webpage_retriever();
		virtual ~webpage_retriever();

		char *retrieve(const char *url);

	private:
		void free_chunk();
	};

}

#endif /* WEBPAGE_RETRIEVER_H_ */
