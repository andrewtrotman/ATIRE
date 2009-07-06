#include "uniseg_settings.h"

#include <cstdlib>
#include <utils/fs.h>
#include <utilities.h>
#include <utils/fs.h>

using namespace std;

const char UNISEQ_settings::DEFAULT_HOME[] = {"."};
const char UNISEQ_settings::DEFAULT_HOME_V_NAME[] = {"QLINK_HOME"};
const char UNISEQ_settings::DEFAULT_VAR[] = {"var"};
const char UNISEQ_settings::DEFAULT_INDEX[] = {"index"};
const char UNISEQ_settings::DEFAULT_ZH[] = {"zh"};
const char UNISEQ_settings::DEFAULT_EN[] = {"en"};

const char UNISEQ_settings::GOOD[] = {"good"};

QConf* UNISEQ_settings::qconf_ptr_ = NULL;

UNISEQ_settings::QConf() {
	load_ = false;
	verbose_ = false;

	to_skip_ = 1;
	skip_high_ = 0;
	skip_low_ = 0;

	lang_ = stpl::UNKNOWN;
	format_ = XML;

	reward_ = false;
	mean_ = 1;
	mi_ = 0;

	max_ = 5;

	sep_[0] = FILESYSTEM::File<>::SEPARATOR;
	sep_[1] = '\0';

	this->add_usage();
	this->options();

	setup_home();
}

UNISEQ_settings::~QConf() {

}

QConf* UNISEQ_settings::instance() {
	if (!qconf_ptr_) {
		/**
		 * TODO throw an exception here
		 */
		 cerr << "QConf must be assigned an instance address"  << endl;
		 exit(1);
	}
	return qconf_ptr_;
	/*
	static QConf inst;
	return inst;
	*/
}

void UNISEQ_settings::usage() {
	if(opt_.hasOptions())
		opt_.printUsage();
	else
		std::cout << "Not help information for " << app_name_ << " is implemented" << std::endl;
	exit(0);
}

void UNISEQ_settings::options() {
    opt_.setFlag(  "help", 'h' );   /* a flag (takes no argument), supporting long and short form */
    opt_.setFlag(  "verbose", 'v' );
    opt_.setFlag(  "debug" );
    opt_.setFlag(  "optimize" );

    opt_.setOption( "format", 'f' );
    opt_.setOption( "lang" );

	opt_.setOption( "skip", 'k' );
	opt_.setOption( "skip-low" );
	opt_.setOption( "skip-high" );
	opt_.setOption( "to-skip" );

    opt_.setOption(  "max", 'm' );

	opt_.setOption( "mean" );
	opt_.setOption( "mi" );
	opt_.setFlag(  "reward" );
}

void UNISEQ_settings::process(int argc, char** argv) {
	opt_.processCommandArgs( argc, argv );

	app_name_ = argv[0];

	process_others();
	process_lang();
	process_format();
	process_skipping();

	setup_wd();
}

void UNISEQ_settings::process_lang() {
	if( opt_.getValue( "lang" ) != NULL ) {
	   	std::string lang(opt_.getValue( "lang" ));
	   	//cout << "lang: " << lang << endl;
	   	if (lang == "zh")
	   		lang_ = stpl::CHINESE;
	   	else if (lang == "en") {
	   		lang_ = stpl::ENGLISH;
	   	}
	   	//else
	   	//	lang_ = stpl::UNKNOWN;
	}

	if (lang_ == stpl::UNKNOWN) {
		cout << "You must specifiy the language" << endl;
		this->usage();
	}
}

void UNISEQ_settings::process_format() {
	if( opt_.getValue( "format" ) != NULL || opt_.getValue( 'f' ) != NULL  ) {
		std::string format(opt_.getValue( "format" ));
		if (format == "xml")
			format_ = XML;
		else if (format == "text")
			format_ = TEXT;
		else if (format == "trec")
			format_ = TREC;
	} else  {
		cout << "You must specifiy the document format" << endl;
		this->usage();
	}
}

void UNISEQ_settings::process_skipping() {

	if( opt_.getValue( 'k' ) != NULL  || opt_.getValue( "skip" ) != NULL  )
		chars_to_array(static_cast<const char*>(opt_.getValue( "skip" )), skip_);

	if( opt_.getValue( "to-skip" ) != NULL  )
	    to_skip_ =  atoi(opt_.getValue( "to-skip" ));

	if( opt_.getValue( "skip-high" ) != NULL  )
	    skip_high_ =  atoi(opt_.getValue( "skip-high" ));

	if( opt_.getValue( "skip-low" ) != NULL  )
	    skip_low_ =  atoi(opt_.getValue( "skip-low" ));
}

void UNISEQ_settings::process_others() {
    if( opt_.getFlag( "help" ) || opt_.getFlag( 'h' ) ) {
        usage();
        exit(0);
    }

	if( opt_.getFlag( "verbose" ) || opt_.getFlag( 'v' ))
		verbose_ = true;

	if( opt_.getValue( "max" ) != NULL || opt_.getValue( 'm' ) != NULL  ) {
	    max_ = atoi(opt_.getValue( "max" ));
//	    if (max_ > 5)
//	    	max_ = 5;
	}

	if ( opt_.getValue( "mean" ) != NULL ) {
		mean_ = atoi(opt_.getValue("mean"));
		//if (mean_ <= 0 || mean_ > 4)
		//	mean_ = 1;
	}

	if ( opt_.getValue( "mi" ) != NULL ) {
		mi_ = atoi(opt_.getValue("mi"));
		//if (mean_ <= 0 || mean_ > 4)
		//	mean_ = 1;
	}

	switch(mean_) {
		case 1:
			cout << "using harmonic mean to calculate the segmentation score" << endl;
			break;
		case 2:
			cout << "using probability product to calculate the segmentation score" << endl;
			break;
		case 3:
			cout << "using geometric mean to calculate the segmentation score" << endl;
			break;
		case 4:
			cout << "using reverse mutual information to calculate the segmentation score" << endl;
			break;
		case 5:
			cout << "using reverse mutual information 2 with mi " << mi_ << " to calculate the segmentation score" << endl;
			break;
		case 6:
			cout << "using mutual information " << mi_ << " to calculate the segmentation score" << endl;
			break;
		default:
			cout << "no score caculation method specified, please choose between 1-6" << endl;
			break;
	}

	if( opt_.getFlag( "reward" ) ) {
		cout << "segmentation will get reward or penalty" << endl;
		reward_ = true;
	}
}

void UNISEQ_settings::process_ras() {
	if (ras_.size() == 0)
		for( int i = 0 ; i < opt_.getArgc() ; i++ )
			ras_.push_back(opt_.getArgv( i ));
}

void UNISEQ_settings::ras_to_files(std::vector<std::string>& files, bool recursive) {

	process_ras();

	for( int i = 0 ; i < ras_.size() ; i++ ){
		std::string filename(ras_[i]);

		FILESYSTEM::File<std::string> dir(filename);
		if (!dir.exist()) {
			cerr <<  dir.name() << ": No such file" << endl;
			continue;
		}

		int count;
		if ((count = dir.list(files, recursive)) >= 0) {
			cout << "found files: " << count << endl;
		} else {
			cerr <<  dir.name() << ": Not a directory" << endl;
			files.push_back(dir.name());
		}
	}
}

void UNISEQ_settings::setup_wd() {
	wd_ = string(home_);
	char ch[2];
	ch[0] = FILESYSTEM::File<>::SEPARATOR;
	ch[1] = '\0';
	string sep(ch);
	wd_.append(sep);
	wd_.append(DEFAULT_VAR);
	wd_.append(sep);

	switch(lang_) {
	case stpl::CHINESE:
		wd_.append(DEFAULT_ZH);
		break;
	case stpl::ENGLISH:
		wd_.append(DEFAULT_EN);
		break;
	default:
		break;
	}
	wd_.append(sep);
	//wd_.push_back(ch);
}

/**
 *  directory or path for holding "good" (seged) freq
 *
 */
std::string UNISEQ_settings::good() {
	string good(wd_);
	//good.push_back(FILESYSTEM::File<>::SEPARATOR);
	good.append(sep_);
	good.append(GOOD);
	good.append(sep_);
	//good.push_back(FILESYSTEM::File<>::SEPARATOR);
	return good;
}

void UNISEQ_settings::setup_home() {
	home_ = getenv(DEFAULT_HOME_V_NAME);
	if (!home_)
		home_ = DEFAULT_HOME;
}

void UNISEQ_settings::skip_level(const char* level) {
	chars_to_array(level, skip_);
}

bool UNISEQ_settings::is_skip(int size, int freq) {
	if (skip_.size() <= 0)
		return false;

	std::vector<int>::iterator ret = std::find(skip_.begin(), skip_.end(), size);
	if (ret != skip_.end()) {
		if (skip_low_ == 0 && skip_high_ != 0) {
			if (freq > skip_high_)
				return true;
		}
		else if (skip_high_ == 0 && skip_low_ != 0) {
			if (freq < skip_low_)
				return true;
		}
		else {
			if (freq > skip_low_ && freq < skip_high_)
				return true;
		}
	}
	return false;
}
