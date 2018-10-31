#! /bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.
#
# RCS: @(#) $Id: configure.ac,v 1.1 2013/01/25 19:12:03 chen.bin Exp $

dnl	Ensure that the config (auto)headers support is used, then just 
dnl	include the configure sources from ../unix:

m4_include(../unix/aclocal.m4)
m4_define(SC_USE_CONFIG_HEADERS)
m4_include(../unix/configure.in)
