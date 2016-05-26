use warnings;
use strict;
use Scalar::Util qw(looks_like_number);
sub cycle {
	my ($aref, $index) = @_;
	my $next = $$aref[$index];
	if ($next == -2) {return $index;}
	if ($next == -1) {
		$$aref[$index] = -2;
		return -1;
	}
	$$aref[$index] = -2;
	return cycle($aref, $next);
}

open(my $fh, '<', 'input.txt') or die "$!";

my $str = <$fh>;
my @array = split(' ', $str);
my $size = 0 + @array;
for (@array) {
	if (!looks_like_number($_) or int($_) != $_ or $_ >= $size or $_ < 0) {
		die "Wrong data";
	}
}
my $retval = -1;
#each array element is used no more than 2 times ('for' cycle and the sub 'cycle') which gives us ~2n operations (exlude the data correctness check)
for (my $i = 0; $i < $size; $i++) {
	if ($array[$i] == -2) { 
		next; 
	}
	my $index = $array[$i];
	$array[$i] = -1;
	$retval = cycle(\@array, $index);
	if ($retval >= 0) { 
		last; 
	}
}

print "$retval\n";
