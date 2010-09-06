/*
 * webpage_retriever.cpp
 *
 *  Created on: Dec 31, 2009
 *      Author: monfee
 */

#include "webpage_retriever.h"

namespace QLINK {
	static void *myrealloc(void *ptr, size_t size);

	static void *myrealloc(void *ptr, size_t size)
	{
	  /* There might be a realloc() out there that doesn't like reallocing
		 NULL pointers, so we take care of it here */
	  if(ptr)
		return realloc(ptr, size);
	  else
		return malloc(size);
	}

	static size_t
	WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
	{
	  size_t realsize = size * nmemb;
	  struct MemoryStruct *mem = (struct MemoryStruct *)data;

	  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
	  if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	  }
	  return realsize;
	}

	webpage_retriever::webpage_retriever()
	{
		  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
		  chunk.size = 0;    /* no data at this point */

		  curl_global_init(CURL_GLOBAL_ALL);

		  /* init the curl session */
		  curl_handle = curl_easy_init();

		  /* send all data to this function  */
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		  /* we pass our 'chunk' struct to the callback function */
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

		  /* some servers don't like requests that are made without a user-agent
			 field, so we provide one */
		  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "firefox/3.5.1");
	}

	webpage_retriever::~webpage_retriever()
	{
		free_chunk();

		/* cleanup curl stuff */
		curl_easy_cleanup(curl_handle);

		/* we're done with libcurl, so clean it up */
		curl_global_cleanup();
	}

	void webpage_retriever::free_chunk()
	{
		  if(chunk.memory) {
			free(chunk.memory);
			chunk.memory = NULL;
		  }
		  chunk.size = 0;
	}

	char *webpage_retriever::retrieve(const char *url)
	{
		free_chunk();

	  /* specify URL to get */
	  curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	  /* get it! */
	  curl_easy_perform(curl_handle);

	  /*
	   * Now, our chunk.memory points to a memory block that is chunk.size
	   * bytes big and contains the remote file.
	   *
	   * Do something nice with it!
	   *
	   * You should be aware of the fact that at this point we might have an
	   * allocated data block, and nothing has yet deallocated that data. So when
	   * you're done with it, you should free() it as a nice application.
	   */
	  return chunk.memory;
	}

}
