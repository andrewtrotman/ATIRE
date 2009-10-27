/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * (c) Copyright 2007-2008 TYONLINE TECHNOLOGY PTY. LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU LESSER GENERAL PUBLIC LICENSE, Version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *******************************************************************************
 *
 * @author				Ling-Xiang(Eric) Tang
 *
 *******************************************************************************/

#ifndef ARCHIVE_H_
#define ARCHIVE_H_

#include <iostream>
#include <string>
#include <archive.h>
#include <archive_entry.h>
#include <libgen.h>

#include <stdlib.h> // for exit function

namespace ARCHIVE {
	class Archive {
		protected:
			struct archive *a_;
			struct archive_entry *entry_;

		public:
			typedef std::pair<bool, std::string> result_type;

		public:
			Archive() { init(); }
			~Archive() { clearup(); }

			void open(std::string filename) {
				int r;
				if ((r = archive_read_open_file(a_, filename.c_str(), 10240))) {
					std::cerr << archive_error_string(a_) << std::endl;
					exit(r);
				}
			}

			void close() {
				archive_read_close(a_);
				archive_read_finish(a_);
			}

			result_type find(std::string entryname) {
				int r;
				char *bname;

				for (;;) {
					r = archive_read_next_header(a_, &entry_);
					if (r == ARCHIVE_EOF)
						break;
					if (r != ARCHIVE_OK) {
						std::cout << archive_error_string(a_) << std::endl;
						exit(1);
					}

					//cout << archive_entry_pathname(entry) << endl;
					char *name = (char*)archive_entry_pathname(entry_);
					bname = basename(name);
					//if (string(archive_entry_pathname(entry)).find(entryname) != string::npos) {
					if (std::string(bname) == std::string(entryname)) {

						const void *buff;
						size_t size;
						off_t offset;

						for (;;) {
							r = archive_read_data_block(a_, &buff, &size, &offset);
							if (r != ARCHIVE_EOF && r== ARCHIVE_OK) {
								//cout << static_cast<const char*>(buff);
								return make_pair(true, std::string(static_cast<const char*>(buff)));
							}else {
								std::cout << archive_error_string(a_) << std::endl;
								break;
							}
						}
					}

				} // for

				return make_pair(false, std::string(""));
			}

		private:
			void init() {
				a_ = archive_read_new();
			}

			void clearup() {
			}
	};

	class TgzArchive: public Archive {
		public:
			TgzArchive() : Archive(){
				init();
			}
			~TgzArchive() {}

		private:
			void init() {
				archive_read_support_compression_gzip(a_);
				archive_read_support_format_tar(a_);
			}


	};


}
#endif /*ARCHIVE_H_*/
