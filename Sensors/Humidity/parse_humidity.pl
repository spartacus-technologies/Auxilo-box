#!/usr/bin/perl
#use strict;

#usage
# ./parse_humidity temp
# ./parse_humidity hum


#MUUTA EETU TÄMÄ KOHTA
my $INPUT_SCRIPT_PATH = './vale_syote.sh';
#MUUTA EETU TÄMÄ KOHTA


my $first_argument = $ARGV[0]; #temp or hum
my $sensor_output = qx($INPUT_SCRIPT_PATH); #run external command

#If inputdata contains 'Data (0)' -> sensor error
if ($sensor_output =~ m/Data \(0\)/s)
{
    print "-1";
}
else
{
    # parse sensor output
    if($first_argument eq temp)
    {
        #the ending 's' makes the regexp dot(.) include linebreaks
        $sensor_output =~ s/.*Temp =  (.*) \*C.*/$1/s;
        print "$sensor_output";
    }
    elsif($first_argument eq hum)
    {
        $sensor_output =~ s/.*Hum = (.*) .*/$1/s;
        print "$sensor_output";
    }
}
