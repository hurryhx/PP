#!/usr/bin/python2

import numpy as np
import matplotlib.pyplot as plt
import argparse,os,re

font = {'family' : 'serif',
		'color'  : 'darkred',
		'weight' : 'normal',
		'size'   : 16,
		}

def get_args():
	description = "get data"

	parser = argparse.ArgumentParser(description =description)

	parser.add_argument('-d','--data-dir',
			help = 'source file directory',
			required = True)
	parser.add_argument('-o','--output-dir',
			help = 'output file directory',
			required = True)
	args = parser.parse_args();

	return args;

def main():
	args = get_args()

	data = []
	for file_name in os.listdir(args.data_dir):
		if re.match('output',file_name) != None:
			f=open(file_name)
			for line in f.readlines():
				if re.match("Data size|Processor numbers|Sorting time" , line) != None:
					data.append(line);
			f.close()


	def to_int(x):
		try:
			return int(x)
		except:
			return int(x[0:-2])
	tuples = []
	for index, line in enumerate(data):
		line = line.rstrip();
		if line.startswith('Data size'):
			tuples.append([
				to_int(last_term.split()[-1]) \
						for last_term in data[index:index+3]])

	tuples = sorted(tuples, key = lambda nums:nums[0])


	a = """
	for index,processors in enumerate([24,36,48,60,72,84,96,108,120]):
		plt.subplot(3,3,index +1)
		temp1 = []
		temp2 = []
		print ("processors ={}".format(processors))
		for ele in tuples:
			if ele[1] == processors:
				temp1.append(ele[0])
				temp2.append(ele[2])
		plt.plot(temp1,temp2,'yo-')
		plt.xlabel("Dataset scale)")
		plt.ylabel("Execution time(ms)")
		plt.title("Number of processors = {}".format(processors))



		print temp1
	plt.show()"""

	tuples = sorted(tuples,key = lambda nums:nums[1])
	print tuples
	for index,size in enumerate([1280000,640000,320000,160000,80000,40000,20000,10000]):
		plt.subplot(4,2,index+1)
		print ("size = {}".format(size));
		temp1 =[]
		temp2 =[]
		for ele in tuples:
			if ele[0] == size:
				temp1.append(log(ele[-1]))
				temp2.append(float(ele[-2]))
		a = 'N = ' + str(size)

		plt.ylabel('Execution time(ms)')
		plt.xlabel('Number of processors')
		plt.title('N = {}'.format(size))



		plt.plot(temp2,temp1,'yo-')

	plt.show()

	return
	with open(args.output_dir,'w')as f:
		for line in data:
			f.write(line)

if __name__ == '__main__':
	main()
