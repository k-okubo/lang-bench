#!/usr/bin/perl

use strict;
use Cwd;
use File::Basename;

my $base_dir = Cwd::getcwd() . '/' . dirname($0);
my $NUM_SAMPLES = $ARGV[0] || 3;

sub main {
	print "num of samples = $NUM_SAMPLES\n";
	print "\n";

	my @tests = find_tests();
	build_all(\@tests);
	run_all(\@tests);
	# print "\n";
	# clean_all(\@tests);
}

sub find_tests {
	opendir(DIR, path());
	my @tests = grep { $_ !~ /^\./ && -d $_ } readdir(DIR);
	closedir(DIR);

	my @result;

	foreach my $test (@tests) {
		opendir(DIR, path($test));
		my @targets = map { { 'name' => $_, 'path' => path($test, $_) } }
				grep { $_ !~ /^\./ && -d path($test, $_) }
				readdir(DIR);
		closedir(DIR);

		push @result, {
			'name' => $test,
			'targets' => \@targets,
		};
	}

	return @result;
}

sub build_all {
	my $tests = shift;

	foreach my $test (@$tests) {
		foreach my $target (@{$test->{'targets'}}) {
			build($target->{'path'});
		}
	}
}

sub run_all {
	my $tests = shift;

	foreach my $test (@$tests) {
		print "\n";
		print "$test->{'name'} -----------------------\n";

		my %results;
		$results{ $_->{'name'} } = [] foreach (@{$test->{'targets'}});

		for (my $i = 0; $i < $NUM_SAMPLES; $i++) {
			foreach my $target (@{$test->{'targets'}}) {
				my $result = run($target->{'path'});
				push @{$results{$target->{'name'}}}, $result if $result;
			}
		}

		foreach my $target_name (sort keys %results) {
			my ($avg, $stddev) = aggregate( map { $_->{'user'} + $_->{'sys'} } @{$results{$target_name}} );
			printf " %8s %6.2f (%6.2f)\n", $target_name, $avg, $stddev;
		}
	}
}

sub clean_all {
	my $tests = shift;

	foreach my $test (@$tests) {
		foreach my $target (@{$test->{'targets'}}) {
			clean($target->{'path'});
		}
	}
}

sub build {
	my $path = shift;

	print "build $path\n";
	chdir $path;

	my $retcode = system 'make';
	error_exit() if $retcode != 0;
}

sub run {
	my $path = shift;

	my $result = undef;
	chdir $path;

	open(CMD, 'make run 2>&1 1> /dev/null |');
	while (<CMD>) {
		if (/user\:([\d\.]+).sys\:([\d\.]+)/) {
			$result = { 'user' => $1, 'sys' => $2 };
			last;
		}
	}
	close(CMD);

	return $result;
}

sub clean {
	my $path = shift;
	print "clean $path\n";
	chdir $path;

	system 'make clean';
}

sub aggregate {
	my $sum = 0;
	my $sum_var = 0;

	$sum += $_ foreach (@_);
	my $average = $sum / @_;
	$sum_var += $_ foreach (map { ($_ - $average) ** 2 } @_);
	my $variance = $sum_var / @_;

	return $average, sqrt($variance);
}

sub path {
	return join('/', $base_dir, @_);
}

sub error_exit {
	exit(1);
}

main();
