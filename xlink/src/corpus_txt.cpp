/*
 * corpus_txt.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: monfee
 */

#include "corpus_txt.h"
#include "system_call.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace QLINK;
using namespace std;

corpus_txt::corpus_txt()
{

}

corpus_txt::~corpus_txt()
{

}

//string corpus_txt::id2docpath(unsigned long id)
//{
//	if (load_)
//		return home_ + id2doc(id) + ".txt";
//
//
//	if (id == 22)
//		cout << "I caught you" << endl;
//	std::ostringstream stm;
//	stm << id;
//	string cmd("/home/monfee/workspace/qlink/src/scripts/doc2txtpath.sh ");
//	cmd.append(stm.str());
//
//	return system_call::instance().execute(cmd.c_str());
//}
