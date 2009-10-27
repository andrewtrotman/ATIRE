/*
 * algorithm_bep.h
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_BEP_H_
#define ALGORITHM_BEP_H_

#include "algorithm.h"
#include "porter.h"
#include <string>
#include <map>
#include <set>

namespace QLINK {
	class ltw_topic;

	class algorithm_bep: public QLINK::algorithm
	{
	public:
		enum bep_algorithm { BEP_NONE = 0,
							 BEP_MATCH = 1,
							  BEP_GRAPH = 2
							  };
		typedef std::map<std::string, bep_algorithm>	bep_algorithm_map;

	public:
		static bep_algorithm_map	alorithm_names;
		static const long			SIGNATURE_HALF_SIZE = 100;
		static const long			SIGNATURE_SIZE = SIGNATURE_HALF_SIZE * 2;

	private:
		bep_algorithm			algor_ind_;
		const char 				*source_;
		char					signature_[3 * SIGNATURE_HALF_SIZE];
		long 					anchor_offset_;
		std::set<std::string> 	signature_set_;
		ANT_porter				stemmer_;

	public:
		algorithm_bep(std::string& name);
		virtual ~algorithm_bep();

		long find_bep(ltw_topic *topic, const char *title);
		long find_bep(const char *text, const char *title);

	    long get_anchor_offset() const { return anchor_offset_; }

	    const char *getsource() const { return source_;  }

	    void setsource(const char *source) { source_ = source; }

	    void set_anchor_offset(long  anchor_offset) {  this->anchor_offset_ = anchor_offset;  }

	    void create_signature();

	private:
		long find_bep_in_graph(const char *text);
		void create_term_set(std::set<std::string>& term_set, const char *source);
		int compare_term_set(std::set<std::string>& term_set1, std::set<std::string>& term_set2);
	};

}

#endif /* ALGORITHM_BEP_H_ */
