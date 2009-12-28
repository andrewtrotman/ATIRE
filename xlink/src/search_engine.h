/*
 * search_engine.h
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#ifndef SEARCH_ENGINE_H_
#define SEARCH_ENGINE_H_

typedef	void * SE;

class search_engine
{
protected:
	char **docids_;
	long long hits_;

private:
	SE	instance_;

public:
	search_engine();
	virtual ~search_engine();

	long long hits() { return hits_; }
	char **docids() const { return docids_; }
	virtual void search(const char *term) = 0;
};

#endif /* SEARCH_ENGINE_H_ */
