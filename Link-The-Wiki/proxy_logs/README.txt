        README for proxy_logs directory:

This directory is for programs/files related to the use of proxy logs
for determining link strength for the INEX Link-the-Wiki topic.

For confidentiality reasons, our actual proxy logs will not be stored
in the Subversion repository (even though we have taken reasonable
measures to remove all personally identifiable information from them),
but the programs used to process the logs will be stored here.

The current contents of the directory are:

    - anonymise.c: takes a log file (of the type used by the Otago
      University student proxy), removes the IP addresses, and puts
      the usernames through a one-way hash. This allows us to track
      users' sessions without knowing the identities of the users
      themselves. Rather than sending us the raw logs, ITS will run
      this program on the logs and give us the output.

    - make_title_list.cpp: takes a list of files (such as the ones in
      the 2009 INEX Wikipedia collection) and makes a list of their
      titles and article IDs. This is a rather long process --- largely
      because of all the file-opening --- which is why we don't want
      to repeat it for every log file that we process. This program
      doesn't sort the list of articles, but that is a much faster
      process and can be done later on in the toolchain.

    - add_ids_to_log.cpp: takes the output file of the make_title_list
      program (on the command line) and uses it to determine the
      article ID being accessed in each log entry given on stdin. (The
      log entries should be in the format produced by anonymise.c.)
      The current implementation of this is to read the title/ID list
      into memory, sort it by article title, and perform a binary
      search on it in order to look up the article ID corresponding to
      each article name appearing in the log. The article names are
      taken from the URLs using this regular expression:
        http://en.wikipedia.org/wiki/([^ ?]*)
      If a URL doesn't match the regular expression, it is ignored, as
      it is assumed to be a request for something other than a
      Wikipedia article.
