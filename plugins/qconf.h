#ifndef QCONF_H_
#define QCONF_H_

#include "anyoption.h"

#include <vector>
#include <algorithm>
#include <string>

#include <stpl/characters/stpl_character.h>

class QConf {

public:
	static const char DEFAULT_HOME[];
	static const char DEFAULT_HOME_V_NAME[];
	static const char DEFAULT_VAR[];
	static const char DEFAULT_INDEX[];
	static const char DEFAULT_ZH[];
	static const char DEFAULT_EN[];

	static const char GOOD[];

	static const int MAX_CHARS = 100;
	enum Format {XML, TEXT, TREC};

protected:
	AnyOption opt_;

	//std::string filename_;
	const char* home_;  // working directory holding all the index and freq files

	bool load_;
	bool verbose_;

	std::vector<int> skip_;
	int skip_high_;
	int skip_low_;
	int to_skip_; // for all skipping

	int max_;

	stpl::Language lang_;
	Format format_;

	char* app_name_;
	std::string wd_;
	char  sep_[2];

	std::vector<const char*> ras_; /// the rest of arguments

	int mean_;
	int mi_;
	bool reward_;

private:
	static QConf* qconf_ptr_;

public:
	QConf();
	~QConf();

	static QConf* instance();
	static void instance(QConf* qconf_ptr) { qconf_ptr_ = qconf_ptr; }

	bool is_skip(int size, int freq);

	bool do_load() { return load_; }
	void do_load(bool load) { load_ = load; }

	bool verbose() { return verbose_; }
	void verbose(bool verbose) { verbose_ = verbose; }

	bool do_debug() { return opt_.getFlag( "debug" ); }
	bool do_optimize() { return opt_.getFlag( "optimize" ); }

	const std::vector<int>& skip_level() const { return skip_; }
	void skip_level(std::vector<int> skip) { std::copy(skip_.begin(), skip_.begin(), skip.begin()); }
	void skip_level(const char* level);

	const int skip_high() const { return skip_high_; }
	const int skip_low() const { return skip_low_; }
	void skip_high(int skip_high) { skip_high_ = skip_high; }
	void skip_low(int skip_low) { skip_low_ = skip_low; }

	const int to_skip() const { return to_skip_; }
	void to_skip(int to_skip) { to_skip_ = to_skip; }

	const int max() const { return max_; }

	const stpl::Language lang() const { return lang_; }
	void lang(stpl::Language lang) { lang_ = lang; }

	const char* home() const { return home_; }
	const char* app_name() const { return app_name_; }
	const std::string& wd() const { return wd_; }
	std::string good();

	const std::vector<const char*>& ras() const { return ras_; }
	void ras_to_files(std::vector<std::string>& files, bool recursive);

	int mean() { return mean_; }
	int whichmi() { return mi_; }
	bool reward() { return reward_; }

	Format format() {return format_; }

	const char* separator() const { return sep_; }

	virtual void process(int argc, char** argv);

protected:
	virtual void usage();
	virtual void add_usage() {}

	virtual void options();

	virtual void process_lang();
	virtual void process_format();
	virtual void process_skipping();

	void process_others();
	void process_ras();

	void setup_home();
	virtual void setup_wd();
};

#endif //QCONF_H_
