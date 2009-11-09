#AM_CPPFLAGS = -std=c++0x  -pedantic -DREMOVE_ORPHAN_LINKS
AM_CPPFLAGS = -std=c++0x  -pedantic -I $(srcdir)/include/stpl

LTW_LIBS = -lant -lpthread

SRC_DIR = src

ltw_headers = \
	$(SRC_DIR)/run.h \
	$(SRC_DIR)/algorithm.h \
	$(SRC_DIR)/incoming_link.h \
	$(SRC_DIR)/link.h \
	$(SRC_DIR)/ant_link_term.h \
	$(SRC_DIR)/ant_link_posting.h \
	$(SRC_DIR)/ant_link.h \
	$(SRC_DIR)/ltw_run.h \
	$(SRC_DIR)/outgoing_link.h \
	$(SRC_DIR)/config.h \
	$(SRC_DIR)/run_config.h \
	$(SRC_DIR)/algorithm_config.h \
	$(SRC_DIR)/search_engine_loader.h \
	$(SRC_DIR)/algorithm_ant_link_this.h \
	$(SRC_DIR)/algorithm_ant_link_this_and_page_name.h \
	$(SRC_DIR)/algorithm_ltw_incoming_search_tn.h \
	$(SRC_DIR)/algorithm_ltw_incoming_a2b_search_tn.h \
	$(SRC_DIR)/algorithm_ltw_tang.h \
	$(SRC_DIR)/algorithm_bep.h \
	$(SRC_DIR)/algorithm_ltara_base.h \
	$(SRC_DIR)/cmdline_options.h \
	$(SRC_DIR)/sys_file.h \
	$(SRC_DIR)/sys_files.h \
	$(SRC_DIR)/ir_task.h \
	$(SRC_DIR)/ltw_task.h \
	$(SRC_DIR)/task_a2b.h \
	$(SRC_DIR)/task_f2f.h \
	$(SRC_DIR)/links.h \
	$(SRC_DIR)/outgoing_links.h \
	$(SRC_DIR)/incoming_links.h \
	$(SRC_DIR)/topic.h \
	$(SRC_DIR)/ltw_topic.h \
	$(SRC_DIR)/xml2txt_client.h \
	$(SRC_DIR)/xml2txt.h \
	$(SRC_DIR)/corpus.h \
	$(SRC_DIR)/corpus_txt.h \
	$(SRC_DIR)/system_call.h \
	$(SRC_DIR)/language.h \
	$(SRC_DIR)/search_engine.h \
	$(SRC_DIR)/search_engine_ant.h \
	$(SRC_DIR)/wikipedia.h \
	$(SRC_DIR)/string_utils.h \
	$(SRC_DIR)/str.h \
	$(SRC_DIR)/singleton.h \
	$(SRC_DIR)/template_algo.h \
	$(SRC_DIR)/system_info.h

noinst_HEADERS = $(ltw_headers)
	
ltw_sources = \
	$(SRC_DIR)/sys_file.cpp \
	$(SRC_DIR)/sys_files.cpp \
	$(SRC_DIR)/system_info.cpp	\
	$(SRC_DIR)/run.cpp \
	$(SRC_DIR)/algorithm.cpp \
	$(SRC_DIR)/incoming_link.cpp \
	$(SRC_DIR)/link.cpp \
	$(SRC_DIR)/ant_link_term.cpp \
	$(SRC_DIR)/ant_link_posting.cpp \
	$(SRC_DIR)/ltw_run.cpp \
	$(SRC_DIR)/outgoing_link.cpp \
	$(SRC_DIR)/config.cpp \
	$(SRC_DIR)/run_config.cpp \
	$(SRC_DIR)/algorithm_config.cpp \
	$(SRC_DIR)/search_engine_loader.cpp \
	$(SRC_DIR)/algorithm_ant_link_this.cpp \
	$(SRC_DIR)/algorithm_ant_link_this_and_page_name.cpp \
	$(SRC_DIR)/algorithm_ltw_tang.cpp \
	$(SRC_DIR)/algorithm_ltw_incoming_search_tn.cpp \
	$(SRC_DIR)/algorithm_ltw_incoming_a2b_search_tn.cpp \
	$(SRC_DIR)/algorithm_bep.cpp \
	$(SRC_DIR)/algorithm_ltara_base.cpp \
	$(SRC_DIR)/cmdline_options.cpp \
	$(SRC_DIR)/ltw_task.cpp \
	$(SRC_DIR)/task_a2b.cpp \
	$(SRC_DIR)/task_f2f.cpp \
	$(SRC_DIR)/links.cpp \
	$(SRC_DIR)/outgoing_links.cpp \
	$(SRC_DIR)/incoming_links.cpp \
	$(SRC_DIR)/topic.cpp \
	$(SRC_DIR)/ltw_topic.cpp \
	$(SRC_DIR)/xml2txt_client.cpp \
	$(SRC_DIR)/xml2txt.cpp \
	$(SRC_DIR)/corpus.cpp \
	$(SRC_DIR)/corpus_txt.cpp \
	$(SRC_DIR)/system_call.cpp \
	$(SRC_DIR)/language.cpp \
	$(SRC_DIR)/search_engine.cpp \
	$(SRC_DIR)/search_engine_ant.cpp \
	$(SRC_DIR)/proxy.cpp \
	$(SRC_DIR)/porter.cpp \
	$(SRC_DIR)/wikipedia.cpp \
	$(SRC_DIR)/ltw.cpp

noinst_PROGRAMS = ltw 

ltw_SOURCES = $(ltw_sources)
ltw_LDADD = ${LTW_LIBS}
#ltw_LDFLAGS = -static

