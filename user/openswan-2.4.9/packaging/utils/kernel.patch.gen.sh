#!/bin/bash
#
# RCSID $Id: kernel.patch.gen.sh,v 1.1.1.1 2011/08/17 11:10:49 brwang Exp $

patchdir=`pwd`
kernelsrc=/usr/src/linux
[ "$1~" = "~" ] || kernelsrc=$1
cd $kernelsrc
# clean out destination file for all patch
#echo "">$patchdir/all

# find files to patch and loop
for i in  `find . -name '*.preipsec'`
do

# strip off '.preipsec' suffix
j=${i%.preipsec}

# strip off './' prefix
k=${j#\.\/}

# single unified diff
#diff -u $i $j >>$patchdir/all

# convert '/' in filename to '.' to avoid subdirectories
sed -e 's/\//\./g' << EOI > /tmp/t
$k
EOI
l=`cat /tmp/t`
rm -f /tmp/t

# *with* path from source root
#echo do diff -u $i $j '>' $patchdir/$l
echo found $i
echo "RCSID \$Id: kernel.patch.gen.sh,v 1.1.1.1 2011/08/17 11:10:49 brwang Exp $" >$patchdir/$l
diff -u $i $j >>$patchdir/$l

done

#
# $Log: kernel.patch.gen.sh,v $
# Revision 1.1.1.1  2011/08/17 11:10:49  brwang
# 3520G start
#
# Revision 1.1.1.1  2011/03/15 05:24:34  lei.minghuan
# import ralink
#
# Revision 1.1.1.1  2009/12/14 12:44:52  brwang
# ralink wlan
#
# Revision 1.1.1.1  2007-10-08 08:01:06  steven
#
#
# Revision 1.6  2002/04/25 17:04:16  mcr
# 	resurrected kernel.patch.gen.sh
#
# Revision 1.4  1999/04/06 04:54:30  rgb
# Fix/Add RCSID Id: and Log: bits to make PHMDs happy.  This includes
# patch shell fixes.
#
#
