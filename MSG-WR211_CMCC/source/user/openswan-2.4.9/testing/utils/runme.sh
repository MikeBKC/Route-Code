#!/bin/sh

#
# $Id: runme.sh,v 1.1.1.1 2011/08/17 11:10:43 brwang Exp $
#
# use this script to run a single test from within that test directory.
# note that this assumes a "klipstest" type test.
#

. ../../../umlsetup.sh
. ../setup.sh
. $UTILS/functions.sh
. testparams.sh

if [ -z "${TEST_TYPE}" ]
then
    echo runme.sh now requires that testparams.sh defines TEST_TYPE=
fi

( cd .. && $TEST_TYPE $TESTNAME good )

perl ${OPENSWANSRCDIR}/testing/utils/regress-summarize-results.pl ${REGRESSRESULTS} notest


# $Log: runme.sh,v $
# Revision 1.1.1.1  2011/08/17 11:10:43  brwang
# 3520G start
#
# Revision 1.1.1.1  2011/03/15 05:24:28  lei.minghuan
# import ralink
#
# Revision 1.1.1.1  2009/12/14 12:44:47  brwang
# ralink wlan
#
# Revision 1.1.1.1  2007-10-08 08:01:05  steven
#
#
# Revision 1.3  2004/05/28 02:07:02  mcr
# 	update test case summary at end of manual run.
#
# Revision 1.2  2002/05/23 14:26:39  mcr
# 	verify that $TEST_TYPE is actually set.
#
# Revision 1.1  2002/05/05 23:12:05  mcr
# 	runme.sh script now common for all test types.
#
# 

