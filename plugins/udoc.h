/*
 * udoc.h
 *
 *  Created on: Jan 28, 2009
 *      Author: monfee
 */

#ifndef UDOC_H_
#define UDOC_H_

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <set>

#include "doc.h"
#include "freq.h"
#include "doc_freq.h"
#include "qtypes.h"
#include "range.h"
#include "file.h"


/**
 * This is a sentence class, not "sent"
 */
class Sent {
public:
	typedef Doc::entity_iterator									entity_iterator;

private:
	entity_iterator	begin_;
	entity_iterator	end_;

public:
	Sent(entity_iterator begin, entity_iterator end) :
		begin_(begin), end_(end) {}
	~Sent() {}

	const entity_iterator& begin() const { return begin_; }
	const entity_iterator& end() const { return end_; }
};

/**
 * UDoc is a class that stands for a document which is waiting for being segmented
 */
class UDoc : public File {
public:
	typedef Doc::entity_iterator									entity_iterator;
	typedef std::map<Range*, word_ptr_type, range_compare>			marks_map;
	typedef std::vector<bool>										empty_array;

private:
	std::vector<Sent> 												sents_; /// sentences
	Doc*															doc_;
	Freq 															freq_;
	FreqCounter 													counter_;
	marks_map														mmap_;
	empty_array														fil_;
	pair<bool, bool>												ec_reg_;
	std::set<string_type>*											seged_ptr_;

public:
	UDoc(Doc* doc, std::set<string_type>* seged_ptr);
	~UDoc();

	void seg();
	void show();
	int find(word_ptr_type w_ptr, entity_iterator begin, entity_iterator end, unsigned int pos);
	word_ptr_type find(string_type str);
	//void collect(std::set<string_type>& seged);

private:
	array_type& build_freq_table();
	bool mark_check(unsigned int left, unsigned int right);
	bool empty_check(unsigned int left, unsigned int right);

	void resolve_conflict();
	void find_missing(const entity_iterator begin, const entity_iterator end);

	void init();
	void clear();
	unsigned int fill(unsigned int left, unsigned int right);
	void fil_init(unsigned int len);
	void collect();

	void save();
};

#endif /* UDOC_H_ */
