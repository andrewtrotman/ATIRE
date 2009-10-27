#AM_CPPFLAGS = -std=c++0x  -pedantic -DREMOVE_ORPHAN_LINKS
AM_CPPFLAGS = -std=c++0x  -pedantic

LTW_LIBS = -lant -ldl

ltw_headers = \
	run.h \
	algorithm.h \
	incoming_link.h \
	link.h \
	ant_link_term.h \
	ant_link_posting.h \
	ant_link.h \
	ltw_run.h \
	outgoing_link.h \
	config.h \
	run_config.h \
	algorithm_config.h \
	search_engine_loader.h \
	algorithm_ant_link_this.h \
	algorithm_ant_link_this_and_page_name.h \
	algorithm_ltw_incoming_search_tn.h \
	algorithm_ltw_incoming_a2b_search_tn.h \
	algorithm_ltw_tang.h \
	algorithm_bep.h \
	algorithm_ltara_base.h \
	cmdline_options.h \
	sys_file.h \
	sys_files.h \
	ir_task.h \
	ltw_task.h \
	task_a2b.h \
	task_f2f.h \
	links.h \
	outgoing_links.h \
	incoming_links.h \
	topic.h \
	ltw_topic.h \
	xml2txt_client.h \
	xml2txt.h \
	corpus.h \
	corpus_txt.h \
	system_call.h \
	language.h \
	search_engine.h \
	search_engine_ant.h \
	wikipedia.h \
	string_utils.h \
	str.h \
	singleton.h \
	template_algo.h \
	system_info.h
	
noinst_HEADERS = $(ltw_headers)
	
ltw_sources = \
	sys_file.cpp \
	sys_files.cpp \
	system_info.cpp	\
	run.cpp \
	algorithm.cpp \
	incoming_link.cpp \
	link.cpp \
	ant_link_term.cpp \
	ant_link_posting.cpp \
	ltw_run.cpp \
	outgoing_link.cpp \
	config.cpp \
	run_config.cpp \
	algorithm_config.cpp \
	search_engine_loader.cpp \
	algorithm_ant_link_this.cpp \
	algorithm_ant_link_this_and_page_name.cpp \
	algorithm_ltw_tang.cpp \
	algorithm_ltw_incoming_search_tn.cpp \
	algorithm_ltw_incoming_a2b_search_tn.cpp \
	algorithm_bep.cpp \
	algorithm_ltara_base.cpp \
	cmdline_options.cpp \
	ltw_task.cpp \
	task_a2b.cpp \
	task_f2f.cpp \
	links.cpp \
	outgoing_links.cpp \
	incoming_links.cpp \
	topic.cpp \
	ltw_topic.cpp \
	xml2txt_client.cpp \
	xml2txt.cpp \
	corpus.cpp \
	corpus_txt.cpp \
	system_call.cpp \
	language.cpp \
	search_engine.cpp \
	search_engine_ant.cpp \
	proxy.cpp \
	porter.cpp \
	wikipedia.cpp \
	ltw.cpp
	
noinst_PROGRAMS = ltw 

ltw_SOURCES = $(ltw_sources)
ltw_LDADD = ${LTW_LIBS}
ltw_LDFLAGS = -static

