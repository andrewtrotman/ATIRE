/*
 * translation.h
 *
 *  Created on: Apr 18, 2011
 *      Author: monfee
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

#include <map>
#include <string>

#include "pattern_singleton.h"

namespace QLINK {
	class translation : public pattern_singleton<translation>
	{
	public:
		static const char *CACHE_FOLDER;

	private:
		bool								cache_it_;
		std::map<std::string, std::string>  translations_;

	public:
		translation();
		virtual ~translation();

		std::string translate(const char *word, const char *language_pair);
	};
}

#endif /* TRANSLATION_H_ */
