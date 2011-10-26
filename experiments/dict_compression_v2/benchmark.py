#! /usr/bin/env python
#
#
#  Author: Xiangfei Jia, xjianz@gmail.com, fei@cs.otago.ac.nz
# Created: 02-May-2011
#
#
#
import sys
import os
import subprocess
import re
import csv
import time


class Profile:
	DICT_TYPE_STR = 'Dictionary Type'
	SECTOR_SIZE_STR = 'Sector Size'
	NUM_OF_SECTORS_STR = 'Number of Sectors'
	BLOCK_SIZE_STR = 'Block Size'
	NODE_MAX_SIZE_STR = 'Node Max Size'
	COMMON_PREFIX_STR = 'Common Prefix Size'
	TOTAL_WASTED_STR = 'Total Wasted'
	TOTAL_FILE_SIZE_STR = 'Total File Size'
	IO_TIME_STR  = 'IO Time'
	SEARCH_TIME_STR  = 'Search Time'
	TOTAL_TIME_STR  = 'Total Time'
	BYTES_READ_STR = 'Bytes Read'
	SECTOR_SIZE_PATTERN = re.compile(r'\s*sector size\s*:\s*([0-9.]*)', re.IGNORECASE)
	NUM_OF_SECTORS_PATTERN = re.compile(r'\s*number of sectors\s*:\s*([0-9.]*)', re.IGNORECASE)
	BLOCK_SIZE_PATTERN = re.compile(r'\s*the block size\s*:\s*([0-9.]*)', re.IGNORECASE)
	NODE_MAX_SIZE_PATTERN = re.compile(r'\s*node_max_size\s*:\s*([0-9.]*)', re.IGNORECASE)
	COMMON_PREFIX_PATTERN = re.compile(r'\s*common_prefix_size\s*:\s*([0-9.]*)', re.IGNORECASE)
	TOTAL_WASTED_PATTERN = re.compile(r'\s*total_wasted\s*:\s*([0-9.]*)', re.IGNORECASE)
	TOTAL_FILE_SIZE_PATTERN = re.compile(r'\s*total file size\s*:\s*([0-9.]*)', re.IGNORECASE)
	IO_TIME_PATTERN = re.compile(r'\s*io time\s*:\s*([0-9.]*)', re.IGNORECASE)
	SEARCH_TIME_PATTERN = re.compile(r'\s*search time\s*:\s*([0-9.]*)', re.IGNORECASE)
	TOTAL_TIME_PATTERN = re.compile(r'\s*total time\s*:\s*([0-9.]*)', re.IGNORECASE)
	BYTES_READ_PATTERN = re.compile(r'\s*bytes read\s*:\s*([0-9.]*)', re.IGNORECASE)

	def __init__(self):
		self.dict_type = ''
		self.sector_size = 0;
		self.num_of_sectors = 0
		self.block_size = 0;
		self.node_max_size = 0;
		self.common_prefix = 0;
		self.total_wasted = 0
		self.total_file_size = 0
		self.io_time = 0
		self.search_time = 0
		self.total_time = 0
		self.bytes_read = 0
	def __str__(self):
		output = "dict_type: %s\nsector_size: %d\nnum_of_sectors: %d\nblock_size: %d\nnode_max_size: %d\ncommon_prefix_size: %d\ntotal_wasted: %d\ntotal_file_size: %d\nio_time: %f\nsearch_time: %f\ntotal_time: %f\nbytes_read: %d\n"
		return  output % (self.dict_type, self.sector_size, self.num_of_sectors, self.block_size, self.node_max_size, self.common_prefix, self.total_wasted, self.total_file_size, self.io_time, self.search_time, self.total_time, self.bytes_read)
	def get_headers():
		return tuple((Profile.DICT_TYPE_STR, Profile.SECTOR_SIZE_STR, Profile.NUM_OF_SECTORS_STR, Profile.BLOCK_SIZE_STR,
					Profile.NODE_MAX_SIZE_STR, Profile.COMMON_PREFIX_STR, Profile.TOTAL_WASTED_STR, Profile.TOTAL_FILE_SIZE_STR, Profile.IO_TIME_STR, Profile.SEARCH_TIME_STR, Profile.TOTAL_TIME_STR, Profile.BYTES_READ_STR))
	get_headers = staticmethod(get_headers)
	def get_values(self):
		return tuple((self.dict_type, self.sector_size, self.num_of_sectors, self.block_size, self.node_max_size, self.common_prefix, self.total_wasted, self.total_file_size, self.io_time, self.search_time, self.total_time, self.bytes_read))
pass # end of class profile

def run_build_dict(prof, the_cmd):
	#the_cmd = list(['/home/fei/source-devel/git/ant_mydevel/experiments/dict_compression_v2/bin/build_dictionaries'])
	process = subprocess.Popen(the_cmd, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

	# catch the output of the search engine
	(child_stdout, child_stderr) = process.communicate()

	# check if there is an error before collecting the stats
	if (child_stderr != None):
		print child_stderr
		sys.exit(2)

	# split the output string and process line by line
	for l in child_stdout.split('\n'):
		result = Profile.SECTOR_SIZE_PATTERN.search(l)
		if result != None:
			prof.sector_size = int(result.group(1))
		result = Profile.NUM_OF_SECTORS_PATTERN.search(l)
		if result != None:
			prof.num_of_sectors = int(result.group(1))
		result = Profile.BLOCK_SIZE_PATTERN.search(l)
		if result != None:
			prof.block_size = int(result.group(1))
		result = Profile.NODE_MAX_SIZE_PATTERN.search(l)
		if result != None:
			prof.node_max_size = int(result.group(1))
		result = Profile.COMMON_PREFIX_PATTERN.search(l)
		if result != None:
			prof.common_prefix = int(result.group(1))
		result = Profile.TOTAL_WASTED_PATTERN.search(l)
		if result != None:
			prof.total_wasted = int(result.group(1))
		result = Profile.TOTAL_FILE_SIZE_PATTERN.search(l)
		if result != None:
			prof.total_file_size = int(result.group(1))
		print l
	pass #end of for l in

pass # end of def run_build_dict(...)


def run_search_dict(prof, the_cmd):
	process = subprocess.Popen(the_cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

	# catch the output of the search engine
	(child_stdout, child_stderr) = process.communicate()

	# check if there is an error before collecting the stats
	if (child_stderr != None):
		print child_stderr
		sys.exit(2)

	# split the output string and process line by line
	for l in child_stdout.split('\n'):
		result = Profile.IO_TIME_PATTERN.search(l)
		if result != None:
			prof.io_time = float(result.group(1))
		result = Profile.SEARCH_TIME_PATTERN.search(l)
		if result != None:
			prof.search_time = float(result.group(1))
		result = Profile.TOTAL_TIME_PATTERN.search(l)
		if result != None:
			prof.total_time = float(result.group(1))
		result = Profile.BYTES_READ_PATTERN.search(l)
		if result != None:
			prof.bytes_read = float(result.group(1))
		print l
	pass #end of for l in

pass # end of def run_search_dict(...)

def export_profile_data(prof_list, output_file="output.csv"):
	f = open(output_file, 'w')
	export = csv.writer(f, delimiter=',')
	export.writerow(Profile.get_headers())
	for p in prof_list:
		export.writerow(p.get_values())
	f.close()
pass # end of def export_profile_date


def usage(prog_name):
	print "This script is used to benchmark the runs which are intended to submit to INEX2010 Efficiency Track"
	print "Usage : %s <build | search | embedfixed>" % os.path.basename(prog_name)
	print "    build <dict-path>"
	print "    search <ondisk | inmemory> <dict-path> <query-file>"
	print "    embedfixed <dict-path> <query-file>"
	sys.exit(2)
pass # end of def usgae(prog_name)


def test():
	print Profile.get_headers()
	sys.exit(2)
pass # end of def test()


def profile_build(prof_list, build_bin, dict_path):
	print build_bin
	print dict_path
	dict_types = ["fixed", "string", "blocked-2", "blocked-4", "blocked-8", "embed", "embedfront", "embedfixed", "embedfixed-null"]
	num_of_sectors = [1, 2, 4, 8, 16, 32]

	for dt in dict_types:
		for sn in num_of_sectors:
			the_cmd = list([str(build_bin), '-b'])
			if dt.find("blocked") >= 0:
				bs = dt[-1:]
				the_cmd.extend(['-t', dt[:-2], '-ns', str(sn), '-bs', dt[-1]])
			else:
				the_cmd.extend(['-t', dt, '-ns',  str(sn)])
			pass
			print 'the running command: %s' % the_cmd
			prof = Profile()
			prof.dict_type = dt
			run_build_dict(prof, the_cmd)
			prof_list.append(prof)
			print prof
		pass
	pass
pass


def profile_search(prof_list, build_bin, search_bin, dict_path, query_file, disk_or_memory):
	dict_types = ["fixed", "string", "blocked-2", "blocked-4", "blocked-8", "embed", "embedfront", "embedfixed", "embedfixed-null"]
	num_of_sectors = [1, 2, 4, 8, 16, 32]
	#dict_types = ["fixed"]
	#num_of_sectors = [1, 2]

	for dt in dict_types:
		for sn in num_of_sectors:
			the_build_cmd = list([build_bin, '-b'])
			the_search_cmd = search_bin
			if dt.find("blocked") >= 0:
				bs = dt[-1:]
				the_build_cmd.extend(['-t', dt[:-2], '-ns', str(sn), '-bs', dt[-1]])
				the_search_cmd += ' -dt %s -st %s -ns %d -bs %s' % (dt[:-2], disk_or_memory, sn, dt[-1])
			else:
				the_build_cmd.extend(['-t', dt, '-ns',  str(sn)])
				the_search_cmd += ' -dt %s -st %s -ns %d' % (dt, disk_or_memory, sn)
			pass
			the_search_cmd += ' -qf %s' % query_file

			print 'the build command: %s' % the_build_cmd
			print 'the search command: %s' % the_search_cmd

			# create a new profile
			prof = Profile()
			prof.dict_type = dt
			# build the dictionary first
			run_build_dict(prof, the_build_cmd)
			# the run the search for profile
			#os.system("/home/fei/bin/scripts/flush_cache.sh")
			run_search_dict(prof, the_search_cmd)
			prof_list.append(prof)
			print prof
		pass
	pass
pass


def profile_search_avg(prof_list, build_bin, search_bin, dict_path, query_file, disk_or_memory, repeats):
	dict_types = ["fixed", "string", "blocked-2", "blocked-4", "blocked-8", "embed", "embedfront", "embedfixed", "embedfixed-null"]
	num_of_sectors = [1, 2, 4, 8, 16, 32]
	#dict_types = ["fixed"]
	#num_of_sectors = [1, 2]

	for dt in dict_types:
		for sn in num_of_sectors:
			the_build_cmd = list([build_bin, '-b'])
			the_search_cmd = search_bin
			if dt.find("blocked") >= 0:
				bs = dt[-1:]
				the_build_cmd.extend(['-t', dt[:-2], '-ns', str(sn), '-bs', dt[-1]])
				the_search_cmd += ' -dt %s -st %s -ns %d -bs %s' % (dt[:-2], disk_or_memory, sn, dt[-1])
			else:
				the_build_cmd.extend(['-t', dt, '-ns',  str(sn)])
				the_search_cmd += ' -dt %s -st %s -ns %d' % (dt, disk_or_memory, sn)
			pass
			the_search_cmd += ' -qf %s' % query_file

			print 'the build command: %s' % the_build_cmd
			print 'the search command: %s' % the_search_cmd

			# create a new profile
			prof = Profile()
			prof.dict_type = dt
			# build the dictionary first
			run_build_dict(prof, the_build_cmd)

			aggregated_io_time = 0.0;
			aggregated_search_time = 0.0;
			aggregated_total_time = 0.0;
			for rep in xrange(0, repeats):
				# the run the search for profile
				if disk_or_memory == 'ondisk':
					process = subprocess.Popen('/home/fei/bin/scripts/flush_cache.sh', shell=False)
					# catch the output of the search engine
					(child_stdout, child_stderr) = process.communicate()

					# check if there is an error before collecting the stats
					if (child_stderr != None):
						print child_stderr
						sys.exit(2)
					print 'repeat: %d' % rep
					process = subprocess.Popen('free', shell=False)
					(child_stdout, child_stderr) = process.communicate()
					time.sleep(2)
				pass
				run_search_dict(prof, the_search_cmd)
				aggregated_io_time += prof.io_time
				aggregated_search_time += prof.search_time
				aggregated_total_time += prof.total_time
			pass
			prof.io_time = aggregated_io_time / repeats
			prof.search_time = aggregated_search_time / repeats
			prof.total_time = aggregated_total_time /repeats
			prof_list.append(prof)
			print prof
		pass
	pass
pass


def profile_embedfixed(prof_list, source_path, build_bin, search_bin, dict_path, query_file, repeats):
	dict_types = ["embedfixed", "embedfixed-null"]
	common_prefixes = range(1, 11)

	for dt in dict_types:
		for cp in common_prefixes:
			the_build_cmd = list([build_bin, '-b', '-t', dt])
			the_search_cmd = search_bin + ' -dt %s -st inmemory -qf %s' % (dt, query_file)

			print 'the build command: %s' % the_build_cmd
			print 'the search command: %s' % the_search_cmd

			# make clean
			os.chdir(source_path)
			process = subprocess.Popen(['make', 'clean'], shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
			(child_stdout, child_stderr) = process.communicate()
			if (child_stderr != None):
				print child_stderr
				sys.exit(2)

			# make the binary fist with the specified common prefix size
			process = subprocess.Popen(['make', 'COMMON_PREFIX=-DCOMMON_PREFIX_SIZE=%d' % cp], shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
			(child_stdout, child_stderr) = process.communicate()
			if (child_stderr != None):
				print child_stderr
				sys.exit(2)
			os.chdir(dict_path)

			# create a new profile
			prof = Profile()
			prof.dict_type = dt

			# build the dictionary
			run_build_dict(prof, the_build_cmd)

			# run the search for profile
			aggregated_io_time = 0.0;
			aggregated_search_time = 0.0;
			aggregated_total_time = 0.0;
			for rep in xrange(0, repeats):
				print 'repeat: %d' % rep
				#os.system("/home/fei/bin/scripts/flush_cache.sh")
				run_search_dict(prof, the_search_cmd)
				aggregated_io_time += prof.io_time
				aggregated_search_time += prof.search_time
				aggregated_total_time += prof.total_time
				time.sleep(1)
			pass # end of for
			prof.io_time = aggregated_io_time / repeats
			prof.search_time = aggregated_search_time / repeats
			prof.total_time = aggregated_total_time /repeats
			prof_list.append(prof)
			print prof
		pass
	pass
pass

def main():
	#
	# read command-line options
	#
	if len(sys.argv) < 2:
		print usage(sys.argv[0])

	if sys.argv[1] == 'build':
		if len(sys.argv) < 3:
			print usage(sys.argv[0])
		dict_path = os.path.abspath(sys.argv[2])
		pass
	elif sys.argv[1] == 'search':
		if len(sys.argv) < 5:
			print usage(sys.argv[0])
		disk_or_memory = sys.argv[2]
		dict_path = os.path.abspath(sys.argv[3])
		query_file = os.path.abspath(sys.argv[4])
		pass
	elif sys.argv[1] == 'embedfixed':
		if len(sys.argv) < 4:
			print usage(sys.argv[0])
		dict_path = os.path.abspath(sys.argv[2])
		query_file = os.path.abspath(sys.argv[3])
		pass
	pass


	# remember where the script get executed
	current_path = os.getcwd()

	binary_dir = "/home/fei/source-devel/git/ant_mydevel/experiments/dict_compression_v2/bin"
	build_bin = '%s/%s' % (binary_dir, "build_dictionaries")
	search_bin ='%s/%s' % (binary_dir, "search_dictionaries")

	# the default output file
	output_file = "output.csv"

	# the source code path
	source_path = os.getcwd()

	# change to the dict path where the dict file is located
	os.chdir(dict_path)

	# initialise the profiling list
	prof_list = list([])


	# number of repeats
	repeats = 20

	#
	#
	#
	if sys.argv[1] == "build":
		profile_build(prof_list, build_bin, dict_path)
	elif sys.argv[1] == "search":
		profile_search_avg(prof_list, build_bin, search_bin, dict_path, query_file, disk_or_memory, repeats)
	elif sys.argv[1] == "embedfixed":
		profile_embedfixed(prof_list, source_path, build_bin, search_bin, dict_path, query_file, repeats)
	pass

	#
	# change back to where the script get executed
	#
	os.chdir(current_path)

	export_profile_data(prof_list, output_file)


pass #end of def main()


if __name__ == '__main__':
	main()
