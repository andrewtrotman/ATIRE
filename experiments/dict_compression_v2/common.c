
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>
#if (PROBE_PHYSICAL_SECTOR_SIZE == 1)
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <cctype>
	#include <linux/fs.h>
#endif
#include "common.h"

stats_t stats;

// the default is to use one sector
int num_of_sectors = 1;
// the default is to use 2 blocks
int block_size = 2;

size_t hd_sector_size = 0;

void probe_hd_sector_size() {
#if (PROBE_PHYSICAL_SECTOR_SIZE == 1)
	int fd = open("/dev/sda", O_RDONLY | O_NONBLOCK);
	ioctl(fd, BLKSSZGET, &hd_sector_size);
	close(fd);
#else
	hd_sector_size = 512;
#endif
}

void add_io_time() {
	// in milliseconds
	stats.io_time += (((stats.end.tv_sec * 1e6) + stats.end.tv_usec) - ((stats.start.tv_sec * 1e6) + stats.start.tv_usec));
}

void add_search_time() {
	// in milliseconds
	stats.search_time += (((stats.end.tv_sec * 1e6) + stats.end.tv_usec) - ((stats.start.tv_sec * 1e6) + stats.start.tv_usec));
}

void add_bytes_read(unsigned long long size_in_bytes) {
	stats.bytes_read += size_in_bytes;
}

int run_command(const char *the_command) {
	int child_id, status;

	child_id = vfork();
	if (child_id == 0) {
		// this is the child
		status = execl("/bin/sh", "sh", "-c", the_command, (char *)NULL);
	} else if (child_id > 0) {
		// this is the parent
		while(waitpid(child_id, &status, WNOHANG) == 0) {
			//sleep(1);
		}
	} else {
		// error
		status = -1;
	}

	return status;
}
