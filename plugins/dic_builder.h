/*
 * dic_builder.h
 *
 *  Created on: Feb 12, 2009
 *      Author: monfee
 */

#ifndef DIC_BUILDER_H_
#define DIC_BUILDER_H_

#include "uniseg_types.h"
#include "dic.h"

class DicBuilder {
private:
	Dic*			dic_;
	int				count_;

public:
	DicBuilder(Dic* dic);
	~DicBuilder() {}

	void build(string_type& text, stpl::Language lang);
	//void save(string_type filename, stpl::Language lang);
	//void load(string_type filename, stpl::Language lang);

	void add(string_type filename, stpl::Language lang);
	Dic* dic() { return dic_; }
	void set_dic(Dic* dic) { dic_ = dic; }

	int count() { return count_; }
};

#endif /* DIC_BUILDER_H_ */
