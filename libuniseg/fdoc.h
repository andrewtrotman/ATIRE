/*
 * fdoc.h
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#ifndef FDOC_H_
#define FDOC_H_

#include "mdoc.h"

/**
 * This class is used to realize the Comparative forward matching word segmentation algorithm by Dai and Loh
 */

class FDoc : public MDoc {

public:
	FDoc(Doc* doc);
	~FDoc();

	void extract_bigram(int start, int end);
	void cal_assc_strength();
};

#endif /* FDOC_H_ */
