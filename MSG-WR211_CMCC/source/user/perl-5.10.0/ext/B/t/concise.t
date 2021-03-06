#!./perl

BEGIN {
    if ($ENV{PERL_CORE}){
	chdir('t') if -d 't';
	@INC = ('.', '../lib');
    } else {
	unshift @INC, 't';
	push @INC, "../../t";
    }
    require Config;
    if (($Config::Config{'extensions'} !~ /\bB\b/) ){
        print "1..0 # Skip -- Perl configured without B module\n";
        exit 0;
    }
    require 'test.pl';		# we use runperl from 'test.pl', so can't use Test::More
    sub diag { print "# @_\n" } # but this is still handy
}

plan tests => 156;

require_ok("B::Concise");

$out = runperl(switches => ["-MO=Concise"], prog => '$a', stderr => 1);

# If either of the next two tests fail, it probably means you need to
# fix the section labeled 'fragile kludge' in Concise.pm

($op_base) = ($out =~ /^(\d+)\s*<0>\s*enter/m);

is($op_base, 1, "Smallest OP sequence number");

($op_base_p1, $cop_base)
  = ($out =~ /^(\d+)\s*<;>\s*nextstate\(main (-?\d+) /m);

is($op_base_p1, 2, "Second-smallest OP sequence number");

is($cop_base, 1, "Smallest COP sequence number");

# test that with -exec B::Concise navigates past logops (bug #18175)

$out = runperl(
    switches => ["-MO=Concise,-exec"],
    prog => q{$a=$b && print q/foo/},
    stderr => 1,
);
#diag($out);
like($out, qr/print/, "'-exec' option output has print opcode");

######## API tests v.60 

use Config;	# used for perlio check
B::Concise->import(qw( set_style set_style_standard add_callback 
		       add_style walk_output reset_sequence ));

## walk_output argument checking

# test that walk_output rejects non-HANDLE args
foreach my $foo ("string", [], {}) {
    eval {  walk_output($foo) };
    isnt ($@, '', "walk_output() rejects arg '$foo'");
    $@=''; # clear the fail for next test
}
# test accessor mode when arg undefd or 0
foreach my $foo (undef, 0) {
    my $handle = walk_output($foo);
    is ($handle, \*STDOUT, "walk_output set to STDOUT (default)");
}

{   # any object that can print should be ok for walk_output
    package Hugo;
    sub new { my $foo = bless {} };
    sub print { CORE::print @_ }
}
my $foo = new Hugo;	# suggested this API fix
eval {  walk_output($foo) };
is ($@, '', "walk_output() accepts obj that can print");

# test that walk_output accepts a HANDLE arg
SKIP: {
    skip("no perlio in this build", 4)
        unless $Config::Config{useperlio};

    foreach my $foo (\*STDOUT, \*STDERR) {
	eval {  walk_output($foo) };
	is ($@, '', "walk_output() accepts STD* " . ref $foo);
    }

    # now test a ref to scalar
    eval {  walk_output(\my $junk) };
    is ($@, '', "walk_output() accepts ref-to-sprintf target");

    $junk = "non-empty";
    eval {  walk_output(\$junk) };
    is ($@, '', "walk_output() accepts ref-to-non-empty-scalar");
}

## add_style
my @stylespec;
$@='';
eval { add_style ('junk_B' => @stylespec) };
like ($@, 'expecting 3 style-format args',
    "add_style rejects insufficient args");

@stylespec = (0,0,0); # right length, invalid values
$@='';
eval { add_style ('junk' => @stylespec) };
is ($@, '', "add_style accepts: stylename => 3-arg-array");

$@='';
eval { add_style (junk => @stylespec) };
like ($@, qr/style 'junk' already exists, choose a new name/,
    "add_style correctly disallows re-adding same style-name" );

# test new arg-checks on set_style
$@='';
eval { set_style (@stylespec) };
is ($@, '', "set_style accepts 3 style-format args");

@stylespec = (); # bad style

eval { set_style (@stylespec) };
like ($@, qr/expecting 3 style-format args/,
      "set_style rejects bad style-format args");

#### for content with doc'd options

our($a, $b);
my $func = sub{ $a = $b+42 };	# canonical example asub

sub render {
    walk_output(\my $out);
    eval { B::Concise::compile(@_)->() };
    # diag "rendering $@\n";
    return ($out, $@) if wantarray;
    return $out;
}

SKIP: {
    # tests output to GLOB, using perlio feature directly
    skip "no perlio on this build", 127
	unless $Config::Config{useperlio};
    
    set_style_standard('concise');  # MUST CALL before output needed
    
    @options = qw(
		  -basic -exec -tree -compact -loose -vt -ascii
		  -base10 -bigendian -littleendian
		  );
    foreach $opt (@options) {
	($out) = render($opt, $func);
	isnt($out, '', "got output with option $opt");
    }
    
    ## test output control via walk_output
    
    my $treegen = B::Concise::compile('-basic', $func); # reused
    
    { # test output into a package global string (sprintf-ish)
	our $thing;
	walk_output(\$thing);
	$treegen->();
	ok($thing, "walk_output to our SCALAR, output seen");
    }
    
    # test walkoutput acceptance of a scalar-bound IO handle
    open (my $fh, '>', \my $buf);
    walk_output($fh);
    $treegen->();
    ok($buf, "walk_output to GLOB, output seen");
    
    ## test B::Concise::compile error checking
    
    # call compile on non-CODE ref items
    if (0) {
	# pending STASH splaying
	
	foreach my $ref ([], {}) {
	    my $typ = ref $ref;
	    walk_output(\my $out);
	    eval { B::Concise::compile('-basic', $ref)->() };
	    like ($@, qr/^err: not a coderef: $typ/,
		  "compile detects $typ-ref where expecting subref");
	    is($out,'', "no output when errd"); # announcement prints
	}
    }
    
    # test against a bogus autovivified subref.
    # in debugger, it should look like:
    #  1  CODE(0x84840cc)
    #      -> &CODE(0x84840cc) in ???

    my ($res,$err);
    TODO: {
	#local $TODO = "\tdoes this handling make sense ?";

	sub declared_only;
	($res,$err) = render('-basic', \&declared_only);
	like ($res, qr/coderef CODE\(0x[0-9a-fA-F]+\) has no START/,
	      "'sub decl_only' seen as having no START");

	sub defd_empty {};
	($res,$err) = render('-basic', \&defd_empty);
	is(scalar split(/\n/, $res), 3,
	   "'sub defd_empty {}' seen as 3 liner");

	is(1, $res =~ /leavesub/ && $res =~ /(next|db)state/,
	   "'sub defd_empty {}' seen as 2 ops: leavesub,nextstate");

	($res,$err) = render('-basic', \&not_even_declared);
	like ($res, qr/coderef CODE\(0x[0-9a-fA-F]+\) has no START/,
	      "'\&not_even_declared' seen as having no START");

	{
	    package Bar;
	    our $AUTOLOAD = 'garbage';
	    sub AUTOLOAD { print "# in AUTOLOAD body: $AUTOLOAD\n" }
	}
	($res,$err) = render('-basic', Bar::auto_func);
	like ($res, qr/unknown function \(Bar::auto_func\)/,
	      "Bar::auto_func seen as unknown function");

	($res,$err) = render('-basic', \&Bar::auto_func);
	like ($res, qr/coderef CODE\(0x[0-9a-fA-F]+\) has no START/,
	      "'\&Bar::auto_func' seen as having no START");

	($res,$err) = render('-basic', \&Bar::AUTOLOAD);
	like ($res, qr/in AUTOLOAD body: /, "found body of Bar::AUTOLOAD");

    }
    ($res,$err) = render('-basic', Foo::bar);
    like ($res, qr/unknown function \(Foo::bar\)/,
	  "BC::compile detects fn-name as unknown function");

    # v.62 tests

    pass ("TEST POST-COMPILE OPTION-HANDLING IN WALKER SUBROUTINE");
    
    my $sample;

    my $walker = B::Concise::compile('-basic', $func);
    walk_output(\$sample);
    $walker->('-exec');
    like($sample, qr/goto/m, "post-compile -exec");

    walk_output(\$sample);
    $walker->('-basic');
    unlike($sample, qr/goto/m, "post-compile -basic");


    # bang at it combinatorically
    my %combos;
    my @modes = qw( -basic -exec );
    my @styles = qw( -concise -debug -linenoise -terse );

    # prep samples
    for $style (@styles) {
	for $mode (@modes) {
	    walk_output(\$sample);
	    reset_sequence();
	    $walker->($style, $mode);
	    $combos{"$style$mode"} = $sample;
	}
    }
    # crosscheck that samples are all text-different
    @list = sort keys %combos;
    for $i (0..$#list) {
	for $j ($i+1..$#list) {
	    isnt ($combos{$list[$i]}, $combos{$list[$j]},
		  "combos for $list[$i] and $list[$j] are different, as expected");
	}
    }
    
    # add samples with styles in different order
    for $mode (@modes) {
	for $style (@styles) {
	    reset_sequence();
	    walk_output(\$sample);
	    $walker->($mode, $style);
	    $combos{"$mode$style"} = $sample;
	}
    }
    # test commutativity of flags, ie that AB == BA
    for $mode (@modes) {
	for $style (@styles) {
	    is ( $combos{"$style$mode"},
		 $combos{"$mode$style"},
		 "results for $style$mode vs $mode$style are the same" );
	}
    }

    my %save = %combos;
    %combos = ();	# outputs for $mode=any($order) and any($style)

    # add more samples with switching modes & sticky styles
    for $style (@styles) {
	walk_output(\$sample);
	reset_sequence();
	$walker->($style);
	for $mode (@modes) {
	    walk_output(\$sample);
	    reset_sequence();
	    $walker->($mode);
	    $combos{"$style/$mode"} = $sample;
	}
    }
    # crosscheck that samples are all text-different
    @nm = sort keys %combos;
    for $i (0..$#nm) {
	for $j ($i+1..$#nm) {
	    isnt ($combos{$nm[$i]}, $combos{$nm[$j]},
		  "results for $nm[$i] and $nm[$j] are different, as expected");
	}
    }
    
    # add samples with switching styles & sticky modes
    for $mode (@modes) {
	walk_output(\$sample);
	reset_sequence();
	$walker->($mode);
	for $style (@styles) {
	    walk_output(\$sample);
	    reset_sequence();
	    $walker->($style);
	    $combos{"$mode/$style"} = $sample;
	}
    }
    # test commutativity of flags, ie that AB == BA
    for $mode (@modes) {
	for $style (@styles) {
	    is ( $combos{"$style/$mode"},
		 $combos{"$mode/$style"},
		 "results for $style/$mode vs $mode/$style are the same" );
	}
    }


    #now do double crosschecks: commutativity across stick / nostick
    %combos = (%combos, %save);

    # test commutativity of flags, ie that AB == BA
    for $mode (@modes) {
	for $style (@styles) {

	    is ( $combos{"$style$mode"},
		 $combos{"$style/$mode"},
		 "$style$mode VS $style/$mode are the same" );

	    is ( $combos{"$mode$style"},
		 $combos{"$mode/$style"},
		 "$mode$style VS $mode/$style are the same" );

	    is ( $combos{"$style$mode"},
		 $combos{"$mode/$style"},
		 "$style$mode VS $mode/$style are the same" );

	    is ( $combos{"$mode$style"},
		 $combos{"$style/$mode"},
		 "$mode$style VS $style/$mode are the same" );
	}
    }
}


# test proper NULLING of pointer, derefd by CvSTART, when a coderef is
# undefd.  W/o this, the pointer can dangle into freed and reused
# optree mem, which no longer points to opcodes.

# Using B::Concise to render Config::AUTOLOAD's optree at BEGIN-time
# triggers this obscure bug, cuz AUTOLOAD has a bootstrap version,
# which is used at load-time then undeffed.  It is normally
# re-vivified later, but not in time for this (BEGIN/CHECK)-time
# rendering.

$out = runperl ( switches => ["-MO=Concise,Config::AUTOLOAD"],
		 prog => 'use Config; BEGIN { $Config{awk} }',
		 stderr => 1 );

like($out, qr/Config::AUTOLOAD exists in stash, but has no START/,
    "coderef properly undefined");

$out = runperl ( switches => ["-MO=Concise,Config::AUTOLOAD"],
		 prog => 'use Config; CHECK { $Config{awk} }',
		 stderr => 1 );

like($out, qr/Config::AUTOLOAD exists in stash, but has no START/,
    "coderef properly undefined");

# test -stash and -src rendering
# todo: stderr=1 puts '-e syntax OK' into $out,
# conceivably fouling one of the lines that are tested
$out = runperl ( switches => ["-MO=Concise,-stash=B::Concise,-src"],
		 prog => '-e 1', stderr => 1 );

like($out, qr/FUNC: \*B::Concise::concise_cv_obj/,
     "stash rendering of B::Concise includes Concise::concise_cv_obj");

like($out, qr/FUNC: \*B::Concise::walk_output/,
     "stash rendering includes Concise::walk_output");

like($out, qr/FUNC: \*B::Concise::PAD_FAKELEX_MULTI/,
     "stash rendering includes constant sub: PAD_FAKELEX_MULTI");

like($out, qr/PAD_FAKELEX_MULTI is a constant sub, optimized to a IV/,
     "stash rendering identifies it as constant");

like($out, qr/\# 4\d\d: \s+ \$l->concise\(\$level\);/,
     "src-line rendering works");

$out = runperl ( switches => ["-MO=Concise,-stash=Data::Dumper,-src,-exec"],
		 prog => '-e 1', stderr => 1 );

like($out, qr/FUNC: \*Data::Dumper::format_refaddr/,
     "stash rendering loads package as needed");

my $prog = q{package FOO; sub bar { print "bar" } package main; FOO::bar(); };

# this would fail if %INC used for -stash test
$out = runperl ( switches => ["-MO=Concise,-src,-stash=FOO,-main"],
		 prog => $prog, stderr => 1 );

like($out, qr/FUNC: \*FOO::bar/,
     "stash rendering works on inlined package");

__END__
