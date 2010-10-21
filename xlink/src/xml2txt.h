/*
 * xml2txt.h
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#ifndef XML2TXT_H_
#define XML2TXT_H_

#include "xml2txt_client.h"

namespace QLINK {

	class xml2txt
	{
	private:
		xml2txt_client	client_;
		bool			connected_;

	public:
		xml2txt();
		virtual ~xml2txt();

		static xml2txt& instance();
		char *convert(char *xml);
		char *gettext(long docid, char *xml = 0);
		char *gettearatext(const char *name, char *xml = 0);
		char *clean_tags(char *xml, long lowercase = 0);

	private:
		char *gettext(const char *xmlfile, const char *txtfile, char *xml);
	};

	inline xml2txt& xml2txt::instance() {
		static xml2txt inst;
		return inst;
	}
}

#endif /* XML2TXT_H_ */
