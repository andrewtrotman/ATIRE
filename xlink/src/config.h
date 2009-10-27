/*
 * config.h
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <spec/property.h>

namespace QLINK {
	/*
	 *
	 */
	class config {
	public:
		typedef stpl::PropertiesFile::string_type	string_type;

	protected:
		stpl::PropertiesFile	properties_;
		std::string				name_; // the full path name of the config file

	public:
		config(const char *name);
		virtual ~config();

		void load();
		void show();
		string_type	get_value(string_type name);
	};

}
#endif /* CONFIG_H_ */
