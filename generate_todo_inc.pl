#! /usr/bin/env perl

use warnings;

print "<ul>\n";
open( TODO, "<TODO" );
while( <TODO> )
  {
    if( /^\* (.*)$/ )
      {
	print "</li></ul></li>\n\n" if( $inlist );
	print "<li><span style=\"font-weight:bold\">$1</span><br/>\n";
	$inlist = 0;
	$initem = 0;
      }
    elsif( /^- (.*)$/ )
      {
	if( $initem ) { print "</li>"; };
	print "<ul>\n" if( ! $inlist );
	$inlist = 1;
	print "<li>$1\n";
	$initem = 1;
      }
    else { print unless /^$/; }
  };
print "</li></ul></li></ul>\n";
