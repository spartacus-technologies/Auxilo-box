#!/usr/bin/perl
no warnings;

#Save file lines to array
my $sensor_id = shift; #1. input parameter
my $file = "/sys/bus/w1/devices/$sensor_id/w1_slave";

open (FH, "< $file") or die "Can't open $file for read: $!";
my @lines = <FH>;

#If first lines CRC=YES get new value
if($lines[0] =~ m/crc=.*YES/)
{
    $lines[1] =~ s/.*t=(.*)/$1/;
    print "$lines[1]";
}
elsif($lines[0] =~ m/crc=.*NO/)
{
    print "WRONG_VALUE";
}
else
{
    print "ERROR";
}