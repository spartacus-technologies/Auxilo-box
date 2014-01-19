#!/usr/bin/perl
#use strict;

#Save file lines to array
my $temperature = $ARGV[0];
my $humidity = $ARGV[0];

$temperature =~ s/.*Temp = (.*) \*C.*/$1/;
$humidity =~ s/.*Hum = (.*) .*/$1/;




#$input =~ m/.*Temp = (.*) *C/;

#$input =~ s/.*/$1;

# @lines[0] =~ m/"utterance":"(.*?)"/;

# if(defined($1))
# {
#     print "$1";
# }
# else
# {
#     print "ERROR: Cannot find the word.";
# }


print "$temperature\n";
print "$humidity\n";