/*
 * seg.h
 *
 *  Created on: Nov 29, 2008
 *      Author: monfee
 */

#ifndef DOC_SEG_H_
#define DOC_SEG_H_

#include <string>
#include <vector>
#include <set>
#include "qtypes.h"
#include "doc.h"
#include "doc_freq.h"


class DocSeger {
public:
	typedef Doc::entity_iterator	iterator;

private:
	std::string stream_;  // for the incoming document which will be segmented
	Doc			doc_;
	//Freq 		doc_freq_;
	//FreqCounter doc_counter_;

	// for each item in vector is seged word
	std::vector<std::string>		article_;
	array_type						wa_;
	static std::set<string_type>	seged_;

public:
	DocSeger(std::string name);
	~DocSeger();

	const std::string& stream() const { return stream_; }
	std::string& stream() { return stream_; }

	void start();
	void output();

	void cal_doc_freq();
	void show_doc_freq();
	static bool find(string_type word);
	static void insert(string_type word);
	static void write();

private:
	void seg();
	void build();

	void longest_seg(); /// from left to right, finding longest matched frequent string from table
	void me_seg(); /// 	Maximum frequent string matching Extraction(me) anywhere in the doc, one bye one
	void mi_seg();
	void mi2_seg();
	void q_seg(); /// use qzer method to seg it
	word_ptr_type find_longest_freq_string(const word_ptr_type w_ptr, iterator& begin, iterator end);
};

#endif /* DOC_SEG_H_ */
