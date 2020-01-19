#!/usr/bin/perl

use strict;
use warnings;

my $className="Linux_UnixProcess.instance";
my $INST_FILE;

my $IN=$ARGV[0];

if(defined $IN) {
  if($IN =~ /-rm/) {
    unlink("$className");
    exit 0;
  }
}


my @out=`ps --no-headers -eo pid,ppid,tty,uid,gid,comm,session`;

if( !open($INST_FILE,'>', "$className")) {
  print "can not open $className\n"; 
  exit 0;
}

foreach my $out (@out) {
  my @val = split(' ',$out);
  foreach my $val (@val) {
    print $INST_FILE "$val\n";
  }
  print $INST_FILE "\n";
}
