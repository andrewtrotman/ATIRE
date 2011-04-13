/*
 * config.h
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <spec/property.h>

#include "pattern_singleton.h"

namespace QLINK {
	/*
	 *
	 */
	class config : public pattern_singleton<config> {
	public:
		typedef stpl::PropertiesFile::string_type	string_type;

	protected:
		stpl::PropertiesFile	properties_;
		std::string				name_; // the full path name of the config file

	public:
		config() {};
		config(const char *name);
		virtual ~config();

		void load();
		void load(const char *name);
		void show();
		string_type	get_value(string_type name);
	};

}
#endif /* CONFIG_H_ */
