#!/usr/bin/perl

use strict;
use warnings;

my $className="Linux_Processor.instance";
my $INST_FILE;

my $IN=$ARGV[0];

if(defined $IN) {
  if($IN =~ /-rm/) {
    unlink("$className");
    exit 0;
  }
}

my @out;
if ( `uname -i | grep s390` ) {
  @out=`cat /proc/cpuinfo | grep ^processor | sed -e s/processor//`;
  foreach my $out (@out) {
    if ( $out =~ /^(.+?):(.*)/ ) {
      $out = $1;
    }
  }
}
else {
  @out=`cat /proc/cpuinfo | grep ^processor | sed -e s/processor// | sed -e s/://`;
}

if( !open($INST_FILE,'>', "$className")) {
  print "can not open $className\n"; 
  exit 0;
}

foreach my $out (@out) {
  chomp($out);
  $out =~ s/\s|\t//g;
  print $INST_FILE "$out\n";
  print $INST_FILE "\n";
}
