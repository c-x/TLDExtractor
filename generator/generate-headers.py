#!/usr/bin/env python

import sys
import os
import re

from  urllib import urlopen


# ./generate-headers.py
# -> Get IANA and Mozilla list to generate a C Header

def printCfunction(TLDList):

	rowSize = 10
	nTLD    = len(TLDList)

	# generate small code part to update the C macro.
	cCode = "char *TLD[%s] = {\n" % nTLD
	for i in range(0, nTLD, rowSize):
		p = "\", \"".join( TLDList[i:i+rowSize] )
		v = ""
		if( (i+rowSize) < nTLD ):
			v = ","
		cCode += "\t\t\"%s\"%s\n" % (p, v)
	cCode += "\t};\n"
	cCode += "\t*nbTLD  = %s;\n\n" % nTLD
	
	# read the C code and update it
	code = open("tldlist.c.macro", "r").readlines()

	oCode = ""
	for c in code:
		oCode += re.sub("%TLD_PYTHON_GENERATED_LIST%", cCode, c)
 	
	# write it
	f = open("tldlist.c", "w")
	f.write(oCode)
	f.close()

# eof printCfunction

	
if( __name__ == "__main__" ):

	TLDList = []
	
	# Download Mozilla List : http://publicsuffix.org/list/
	print "Downloading mozilla's TLD list"
	url   = "http://mxr.mozilla.org/mozilla-central/source/netwerk/dns/effective_tld_names.dat?raw=1"
	lines = urlopen(url).readlines()
	for l in lines:
		l = l.strip()

		if( (not l) or (re.search('^//', l)) ):
			continue

		# punycode encoding
		l = l.decode('utf-8').encode('idna')

		# *.sch.uk
		# -> .devon.sch.uk (ex: www.stwilfrids.devon.sch.uk)
		# -> .devon.sch.uk (ex: www.westcroft.devon.sch.uk)
		#if( re.search('^[\*\!]', l) ):
		#	print l

		# ADD THE LEADING DOT
		TLDList.append( l.lower() )

	# IANA TLD List (already punnycoded)
	print "Downloading iana's TLD list"
	url   = "http://data.iana.org/TLD/tlds-alpha-by-domain.txt"
	lines = urlopen(url).readlines()
	for l in lines:
		l = l.strip()

		if( (not l) or (re.search('^#', l)) ):
			continue

		TLDList.append( l.lower() )

	# write the C code
	printCfunction(TLDList)
	print "This is the end !"







