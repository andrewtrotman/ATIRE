#
# Created : 09-Aug-2010
#  Author : xjianz@gmail.com
#
# This script is used to collect profiling stats for accumulators initialisation.
#
#
import sys
import os
import subprocess
import re
import csv
import time

class Profile:
	POSTINGS_LIST_LENGTH_STR = 'Postings List Length'
	NUMBER_OF_TERMS_STR = 'Number of Terms'
	WIDTH_IN_BITS_STR = 'Width in Bits'
	WIDTH_IN_VARIABLE = 'Width in Variable'
	ARRAY_TIME_STR = 'Array Time'
	POW2_TIME_STR = 'Pow2 Time'
	VARIABLE_TIME_STR = 'Variable Time'

	ARRAY_TIME_PATTERN = re.compile(r'\s*Array\s*:\s*([0-9.]*)', re.IGNORECASE)
	POW2_TIME_PATTERN = re.compile(r'\s*2D_POW2\s*:\s*([0-9.]*)', re.IGNORECASE)
	VARIABLE_TIME_PATTERN = re.compile(r'\s*2D_VARIABLE\s*:\s*([0-9.]*)', re.IGNORECASE)
	VARIABLE_WIDTH_PATTERN = re.compile(r'\s*width_in_variable\s*:\s*([0-9.]*)', re.IGNORECASE)

	def __init__(self):
		self.postings_list_legnth = 0
		self.number_of_terms = 0
		self.width_in_bits = 0;
		self.width_in_variable = 0;
		self.array_time = 0
		self.pow2_time = 0;
		self.variable_time = 0;
	def __str__(self):
		output = "postings_list_length: %d\nnumber_of_terms: %d\nwidth_in_bits: %d\nwidth_in_varialbe: %d\narray_time: %d\npow2_time: %d\nvariable_time\n"
		return  output % (self.postings_list_legnth, self.number_of_terms, self.width_in_bits, self.width_in_variable, self.array_time, self.pow2_time, self.variable_time)
	def get_headers():
		return tuple((Profile.POSTINGS_LIST_LENGTH_STR, Profile.NUMBER_OF_TERMS_STR, Profile.WIDTH_IN_BITS_STR, Profile.WIDTH_IN_VARIABLE,
						Profile.ARRAY_TIME_STR, Profile.POW2_TIME_STR, Profile.VARIABLE_TIME_STR))
	get_headers = staticmethod(get_headers)
	def get_values(self):
		return tuple((self.postings_list_legnth, self.number_of_terms,
						self.width_in_bits, self.width_in_variable, self.array_time, self.pow2_time, self.variable_time))
pass # end of class profile


def run_benchmark(exe_file, prof_list, postings_list_length, number_of_terms, width_in_bits, global_seed):
	# we want to catch the last 14 lines for the total stats summary
	args = list([exe_file])
	args.extend(['--postings_list_length',str( postings_list_length)])
	args.extend(['--num_of_terms', str(number_of_terms)])
	args.extend(['--width_in_bits', str(width_in_bits)])
	args.extend(['--seed', str(global_seed)])
	if (width_in_bits == 1):
		args.extend(['--optimal_width'])

	#print "the command is: %s\n" % args
	process = subprocess.Popen(args, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

	# catch the output of the search engine
	(child_stdout, child_stderr) = process.communicate()

	# check if there is an error before collecting the stats
	if (child_stderr != None):
		print child_stderr
		sys.exit(2)

	# create a new profile for the current run
	prof = Profile()
	prof.postings_list_legnth = postings_list_length
	prof.number_of_terms = number_of_terms
	prof.width_in_bits = width_in_bits

	# split the output string and process line by line
	for l in child_stdout.split('\n'):
		result = Profile.ARRAY_TIME_PATTERN.search(l)
		if result != None:
			prof.array_time = float(result.group(1))
		result = Profile.POW2_TIME_PATTERN.search(l)
		if result != None:
			prof.pow2_time = float(result.group(1))
		result = Profile.VARIABLE_TIME_PATTERN.search(l)
		if result != None:
			prof.variable_time = float(result.group(1))
		print l
		result = Profile.VARIABLE_WIDTH_PATTERN.search(l)
		if result != None:
			prof.width_in_variable = int(result.group(1))
	pass #end of for l in

	prof_list.append(prof)
	#print prof
pass # end of def run_benchmark


def export_profile_data(prof_list, output_file="output.csv"):
	f = open(output_file, 'w')
	export = csv.writer(f, delimiter=',')
	export.writerow(Profile.get_headers())
	for p in prof_list:
		export.writerow(p.get_values())
	f.close()
pass # end of def export_profile_date


def usage(prog_name):
	print "Usage : %s [--exe the-executable] [--outfile output-file]" % os.path.basename(prog_name)
	sys.exit(2)
pass # end of def usgae(prog_name)


def test():
	print Profile.get_headers()
	sys.exit(2)
pass # end of def test()



def main():

	output_file = "output.csv"
	exe_file = "/home/fei/source-devel/git/ant_mydevel/experiments/init_accumulators/init_accumulators"
	#test()

	i = 1;
	while i < len(sys.argv):
		if sys.argv[i] == "-h" or sys.argv[i] == "--help":
			usage(sys.argv[0])
			i+=1;
		elif sys.argv[i] == "--exe":
			i+=1
			exe_file = os.path.abspath(sys.argv[i])
			i+=1
		elif sys.argv[i] == "--outfile":
			i+=1;
			output_file = sys.argv[i]
			i +=1;
		else:
			print "un-recognished option: %s" % sys.argv[i]
			sys.exit(2)
		pass
	pass

	#
	# all profiling data is stored in this variable
	#
	prof_list = list([])

	#
	# generate the parameters and call run_benchmark
	#
	global_seed = 0
	postings_list_length = 1
	for postings_list_length in [10, 100, 1000, 10000, 100000, 1000000, 2000000, 3000000]:
		for number_of_terms in [1, 2, 3, 10]:
			# test the optimal case when width_in_bits is 1
			# only need to test the optimal width once for the same query with the same number of terms
			for width_in_bits in xrange(1, 21):
				run_benchmark(exe_file, prof_list, postings_list_length, number_of_terms, width_in_bits, global_seed)
				global_seed += 1
				#time.sleep(1)
				#print "%d, %d, %d" % (postings_list_length, number_of_terms, width_in_bits)
			pass
		pass # end for number_of_terms
	pass

	#
	# save results to file
	#
	export_profile_data(prof_list, output_file)

pass #end of def main()


if __name__ == '__main__':
	main()
