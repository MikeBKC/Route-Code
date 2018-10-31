# $Id: Mod_EUCJP.pm,v 1.1 2012/09/27 21:25:11 yu.dehai Exp $
# This file is in euc-jp
package Mod_EUCJP;
use encoding "euc-jp";
sub new {
  my $class = shift;
  my $str = shift || qw/���ʸ����/;
  my $self = bless { 
      str => '',
  }, $class;
  $self->set($str);
  $self;
}
sub set {
  my ($self,$str) = @_;
  $self->{str} = $str;
  $self;
}
sub str { shift->{str}; }
sub put { print shift->{str}; }
1;
__END__