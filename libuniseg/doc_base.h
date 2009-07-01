/*
 * doc_base.h
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#ifndef DOC_BASE_H_
#define DOC_BASE_H_

#include <vector>
#include <list>
#include <fstream>
#include <utility>

#include "doc.h"
#include "file.h"
#include "qfreq.h"

class DocBase : public File {
public:
	typedef Doc::entity_iterator									entity_iterator;
	typedef std::vector<int>										empty_array;

protected:
	static const int 												UNAVAILABlE;
	static const int 												CLEAN;

protected:
	Doc*															doc_;
	empty_array														fil_;
	Freq*															freq_;

private:
	ostream* 														output_;
	int																wc_;

public:
	DocBase(Doc* doc);
	~DocBase();

	virtual void seg();
	virtual void seg_sentence(entity_iterator& begin, entity_iterator& end) = 0;

private:
	void init();
	void output(entity_iterator& begin, entity_iterator& end);

protected:
	void fil_init(unsigned int len);
	unsigned int fill(unsigned int left, unsigned int len);
	bool filled(unsigned int pos) { assert(pos < fil_.size()); return fil_[pos] > 0; }

	pair<int, int> get_first_unfil(int idx);
};

#endif /* DOC_BASE_H_ */
