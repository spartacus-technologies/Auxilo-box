#!/usr/bin/perl
#use strict;

#Save file lines to array
my $file = "google_output.json";
open (FH, "< $file") or die "Can't open $file for read: $!";
my @lines = <FH>;

@lines[0] =~ m/"utterance":"(.*?)"/;

if(defined($1))
{
    print "$1";
}
else
{
    print "ERROR: Cannot find the word.";
}


#(?<="utterance":").*(?=",)

# #If first lines CRC=YES get new value
# if($lines[0] =~ m/crc=.*YES/)
# {
#     $lines[1] =~ s/.*t=(.*)/$1/;
#     print "$lines[1]";
# }
# elsif($lines[0] =~ m/crc=.*NO/)
# {
#     print "WRONG_VALUE";
# }
# else
# {
#     print "ERROR";
# }


# {"status":0,"id":"d4a0a076ac1c29512e59f3cb34f4bea7-1","hypotheses":[{"utterance":"aux ilo on hieno projekti","confidence":0.6136561}]}