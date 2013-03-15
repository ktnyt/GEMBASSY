#

SUBDIRS = curl include src acd doc

EXTRA_DIST = gsoap depcomp ltmain.sh config.sub config.guess data

# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - acd | ( cd $(distdir); tar xBf - ; cd acd; rm -rf CVS ) 
	tar cBf - doc | ( cd $(distdir); tar xBf - ; cd doc; rm -rf CVS; rm -rf master) 
	tar cBf - include | ( cd $(distdir); tar xBf - ; cd include; rm -rf CVS ) 
	tar cBf - curl | ( cd $(distdir); tar xBf - ; cd curl; rm -rf CVS ) 
