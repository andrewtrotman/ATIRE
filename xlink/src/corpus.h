/*
 * corpus.h
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#ifndef CORPUS_H_
#define CORPUS_H_

#include <map>
#include <string>

namespace QLINK {

	class corpus
	{
	public:
		typedef std::map<unsigned long, std::string> 	doc_map;
		typedef std::map<std::string, std::string> 		teara_map;

	private:
		const char 								*doclist_;

	protected:
		doc_map									doc_map_;
		teara_map								teara_map_;
		bool									load_;
		std::string								home_;
		std::string								base_;
		std::string								teara_home_;
		std::string								lang_;

	public:
		corpus();
		corpus(const char *doclist);
		virtual ~corpus();

		void home(std::string path) { home_ = path; }
		void base(std::string path) { base_ = path; }
		void teara_home(std::string path) { teara_home_ = path; }
		void load();
		virtual void load_teara_map();
		void lang(std::string what) { lang_ = what; }

		virtual std::string id2doc(unsigned long id);
		virtual std::string id2dir(unsigned long id);
		virtual std::string id2docpath(unsigned long id);
		virtual std::string name2docpath(const char *name);
		virtual std::string name2docpath(const std::string& home, const char *name);
		virtual std::string name2tearapath(const char *name);
		virtual std::string ext() { return ".xml"; }
		virtual std::string wildcard() { return "*.[xX][mM][lL]"; }

		std::string gettitle(unsigned long id);
		static corpus& instance();

	private:
		void init();
	};

	inline corpus& corpus::instance() {
		static corpus inst;
		return inst;
	}
}

#endif /* CORPUS_H_ */
