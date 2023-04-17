#!/usr/bin/perl

use strict;
use warnings;

sub incrementcount {
    my $count_file_path = 'count.txt';
    open my $fh, '+<', $count_file_path or die "Couldn't open file: $!";
    flock($fh, 2);
    my $i = <$fh>;
    $i++;
    seek($fh, 0, 0);
    print $fh "$i\n";
    close $fh;
    return $i;
}

print "Status: 200\n";
print "Content-type: text/html\n\n";
print "\r\n\r\n";
print "<html>\n";
print "<body>\n";
print "<h1>The current count is: ", incrementcount(), "</h1>\n";
print '<p><a href="/">Click here to return to the homepage</a></p>';
print "</body>\n";
print "</html>\n";
