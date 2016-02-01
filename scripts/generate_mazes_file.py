#!/usr/bin/python
# author: tkornuta
# brief: generate a csv file containing m mazes of size w by h with random digits (from 0 to 9, with single 0 and 9).
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
	filename = "maze-3x3.csv"
	
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
		spamwriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
		spamwriter.writerow(['maze width', 'maze height'])
		spamwriter.writerow([width, height])
		for maze in range(mazes):
			tmp_maze = numpy.random.randint(10, size=(height*width))
			tmp_str = str(tmp_maze)
			# Concatenate brackets
			tmp_str = tmp_str[1:len(tmp_str)-1]
			# Change delimiter to coma.
			tmp_str.replace(", ,", ",")
			print tmp_str
			spamwriter.writerow(tmp_str)
	print "Successfuly generated ", filename, " containing ",mazes," random maze(s) of size (",height,"x",width,")."

def usage():
	print " -h print usage summary"
	print " -M(=1) sets number of mazes"
	print " -W(=3) sets maze width" 
	print " -H(=3) sets maze height" 
	print " -F(=maze-3x3.csv) sets filename" 

if __name__ == "__main__":
	main(sys.argv[1:])
    
