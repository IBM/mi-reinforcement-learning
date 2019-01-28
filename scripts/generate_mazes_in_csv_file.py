#!/usr/bin/python
# file: generate_mazes_in_csv_file.py
# author: tkornuta
# brief: generate a csv file containing m mazes of size w by h with random digits (from 1 to 9, with a single 0 and 9).
# date: Feb 2, 2016

import csv
import sys
import getopt
import numpy


def main(argv):                     
	# Check arguments
	try:                                
		opts, args = getopt.getopt(argv, "h:M:W:H:F:", ["help", "mazes=", "width=", "height=", "filename="])
	except getopt.GetoptError:
		usage()
		sys.exit(2)
	# Set default values.
	mazes =1
	width = 3
	height = 3
	filename = "1maze-3x3.csv"
	
	# Parse arguments.
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			usage()
			sys.exit()
		elif opt in ("-M", "--mazes"):
			mazes = int(arg)
		elif opt in ("-W", "--width"):
			width = int(arg)
		elif opt in ("-H", "--height"):
			height = int(arg)
		elif opt in ("-F", "--filename"):
			filename = str(arg)
				
	# Generate mazes and write them to a file.
	print "Generating mazes - please wait..."
	with open(filename, 'wb') as csvfile:
		spamwriter = csv.writer(csvfile, delimiter=',', quotechar=' ', quoting=csv.QUOTE_MINIMAL)
		spamwriter.writerow(['maze width', 'maze height'])
		spamwriter.writerow([width, height])
		spamwriter.writerow(['mazes'])
		for maze in range(mazes):
			# Random maze of digits.
			tmp_maze = numpy.random.randint(1,9, size=(height*width))
			# Randomly overwrite a single 0 and 9.
			zero_pos = numpy.random.randint(height*width);
			tmp_maze[zero_pos] = 0;
			nine_pos = zero_pos
			while (nine_pos == zero_pos):
				nine_pos = numpy.random.randint(height*width);
			tmp_maze[nine_pos] = 9;
			# Write maze to file.
			spamwriter.writerow(tmp_maze)
	print "Successfuly generated ", filename, " containing ",mazes," random maze(s) of size (",height,"x",width,")."

def usage():
	print " -h print usage summary"
	print " -M(=1) sets number of mazes"
	print " -W(=3) sets maze width" 
	print " -H(=3) sets maze height" 
	print " -F(=1maze-3x3.csv) sets filename" 

if __name__ == "__main__":
	main(sys.argv[1:])
    
