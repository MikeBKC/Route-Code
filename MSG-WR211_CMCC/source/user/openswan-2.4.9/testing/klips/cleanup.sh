#
# use this script to undo effects of sourcing a "testparams.sh" into
# your shell, when testing.
#
# $Id: cleanup.sh,v 1.1.1.1 2011/08/17 11:10:44 brwang Exp $
#
unset SCRIPT
unset REFCONSOLEOUTPUT 
unset PRIVINPUT
unset PUBINPUT
unset REFPRIVOUTPUT
unset REFPUBOUTPUT
unset TCPDUMPARGS

#
# $Log: cleanup.sh,v $
# Revision 1.1.1.1  2011/08/17 11:10:44  brwang
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
# Revision 1.4  2002/04/24 07:36:13  mcr
# Moved from ./klips/test/cleanup.sh,v
#
# Revision 1.3  2002/02/20 07:26:24  rgb
# Corrected de-pluralized variable names.
#
# Revision 1.2  2001/11/23 01:08:12  mcr
# 	pullup of test bench from klips2 branch.
#
# Revision 1.1.2.1  2001/10/23 04:43:18  mcr
# 	shell/testing cleaning script.
#
# 
