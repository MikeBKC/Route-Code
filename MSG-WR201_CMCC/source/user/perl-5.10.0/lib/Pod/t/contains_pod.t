#!/usr/bin/env perl

# Copyright (C) 2005  Joshua Hoblitt
#
# $Id: contains_pod.t,v 1.1 2012/09/27 21:30:09 yu.dehai Exp $

use strict;

BEGIN {
    if( $ENV{PERL_CORE} ) {
        chdir 't';
        @INC = '../lib';
    } else {
        use lib qw( ./lib );
    }
}


use Test::More tests => 1;

use Pod::Find qw( contains_pod );

{
    ok(contains_pod('lib/contains_pod.xr'), "contains pod");
}
