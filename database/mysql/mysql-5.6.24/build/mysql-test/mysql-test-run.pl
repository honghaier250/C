#!/usr/bin/perl
# Call mtr in out-of-source build
$ENV{MTR_BINDIR} = '/root/workspace/mysql-5.6.24/build';
chdir('/root/workspace/mysql-5.6.24/mysql-test');
exit(system($^X, '/root/workspace/mysql-5.6.24/mysql-test/mysql-test-run.pl', @ARGV) >> 8);
