#!/bin/sh
#
## del *xml in the cvs Repository
#
rm -r *.xml
cvs rm *.xml
cvs ci
