/*
 * wikipedia.h
 *
 *  Created on: Oct 7, 2009
 *      Author: monfee
 */

#ifndef WIKIPEDIA_H_
#define WIKIPEDIA_H_

#include <string>
#include <utility>

namespace QLINK {

	struct wiki_entry{
		long 			id;
		//std::string 	title;
		std::string		description;
	};

	class wikipedia
	{
	public:
		wikipedia();
		virtual ~wikipedia();

		static std::pair<std::string, std::string> process_title(std::string& orig, bool lowercase, bool english_only);
	};

}

#endif /* WIKIPEDIA_H_ */
