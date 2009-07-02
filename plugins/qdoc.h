/*
 * qdoc.h
 *
 *  Created on: Mar 12, 2009
 *      Author: monfee
 */

#ifndef QDOC_H_
#define QDOC_H_

#include <string>
#include <set>

#include "file.h"
#include "qtypes.h"
#include "doc.h"
#include "freq.h"

class QDoc : public File {

public:
	typedef Doc::entity_iterator									entity_iterator;

private:
	Doc*															doc_;
	std::vector<string_type> 										sents_; /// sentences
	std::set<string_type>*											seged_ptr_;
	Freq 															freq_;

public:
	QDoc(Doc* doc, std::set<string_type>* seged_ptr);
	~QDoc();

	void qzerit();
	void save(const array_type& list);
	void collect(const array_type& list);

private:
	void init();
};

#endif /* QDOC_H_ */
