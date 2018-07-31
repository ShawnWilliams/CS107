#!/usr/bin/env python

from __future__ import print_function
import random  # for seed, random
import sys    # for stdout
import copy

# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2, cache):
	# if one of the two strands is empty, then there is only
	# one possible alignment, and of course it's optimal
	def addToCache(alignment):
		#cache[key] = copy.deepcopy(alignment)
		cache[strand1][strand2] = copy.deepcopy(alignment)

	key1 = strand1 + '0'
	key2 = strand2 + '0'
	if key1 in cache.keys() and key2 in cache[key1].keys():
		return cache[key1][key2]
	#key = strand1 + '+' + strand2
	#if key in cache:
	#	return cache[key]

	if len(strand1) == 0: 
		alignment = {
			'strand1':' ' * len(strand2),
			'strand2':strand2,
			'score':len(strand2) * -2
		}
		addToCache(alignment)
		return alignment
	if len(strand2) == 0: 
		alignment = {
			'strand2':' ' * len(strand1),
			'strand1':strand1,
			'score':len(strand1) * -2
		}
		addToCache(alignment)
		return alignment

	# There's the scenario where the two leading bases of
	# each strand are forced to align, regardless of whether or not
	# they actually match.
	bestWith = findOptimalAlignment(strand1[1:], strand2[1:], cache)
	if strand1[0] == strand2[0]: 
		best = {
			'strand1':strand1[0] + bestWith['strand1'],
			'strand2':strand2[0] + bestWith['strand2'],
			'score':bestWith['score'] + 1
		}
		addToCache(best)
		return best # no benefit from making other recursive calls

	best = {
		'strand1':strand1[0] + bestWith['strand1'],
		'strand2':strand2[0] + bestWith['strand2'],
		'score':bestWith['score'] - 1
	}

	# It's possible that the leading base of strand1 best
	# matches not the leading base of strand2, but the one after it.
	bestWithout = findOptimalAlignment(strand1, strand2[1:], cache)
	bestWithout = {
		'strand1':' ' + bestWithout['strand1'],
		'strand2':strand2[0] + bestWithout['strand2'],
		'score':bestWithout['score'] - 2	# penalize for insertion of space
	}
	if bestWithout['score'] > best['score']:
		best = bestWithout

	# opposite scenario
	bestWithout = findOptimalAlignment(strand1[1:], strand2, cache)
	bestWithout = {
		'strand2':' ' + bestWithout['strand2'],
		'strand1':strand1[0] + bestWithout['strand1'],
		'score':bestWithout['score'] - 2	# penalize for insertion of space
	}
	if bestWithout['score'] > best['score']:
		best = bestWithout
	addToCache(best)
	return best

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def plusChar(s1, s2):
    if s1 == s2:
        return '1'
    else:
        return ' '

def minusChar(s1, s2):
    if s1 == ' ' or s2 == ' ':
        return '2'
    elif s1 != s2:
        return '1'
    else:
        return ' '

def printAlignment(alignment, out=sys.stdout):
    print("\nOptimal alignment score is", alignment['score'], "\n")
    print("+\t", ''.join([plusChar(s1, s2) for s1, s2
                          in zip(alignment['strand1'], alignment['strand2'])]))
    print("\t", alignment['strand1'])
    print("\t", alignment['strand2'])
    print("-\t", ''.join([minusChar(s1, s2) for s1, s2
                          in zip(alignment['strand1'], alignment['strand2'])]))

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.

def main():
	while (True):
		sys.stdout.write("Generate random DNA strands? ")
		answer = sys.stdin.readline()
		if answer == "no\n": break
		strand1 = generateRandomDNAStrand(50, 60)
		strand2 = generateRandomDNAStrand(50, 60)
		sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
		sys.stdout.write("                            " + strand2 + "\n")
		cache = {}
		alignment = findOptimalAlignment(strand1, strand2, cache)
		printAlignment(alignment)
		
if __name__ == "__main__":
  main()
