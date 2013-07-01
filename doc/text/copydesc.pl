use strict;
use warnings;

my @progs = split "\n", `wossname -showembassy GEMBASSY -auto | cut -d ' ' -f 1| grep ^g | sort`;

copy($_) foreach @progs;

sub copy {
    my $prog = shift;

    print STDERR "\r\e[K$prog";

    open my $rdr, "<", "old/$prog.txt";
    open my $wtr, ">", "final/$prog.txt";
    open my $tmp, "<", "new/$prog.txt";

    my $out = join "", <$tmp>;

    my $progdesc;

    while(my $line = readline $rdr) {
        if($line =~ /^Description/) {
            readline $rdr;
            while($line !~ /SOAP/) {
                $line = readline $rdr;
                last if $line =~ /SOAP/;
                $progdesc .= $line;
            }
        $progdesc =~ s/\n+$//smg;
        }
    }

    $out =~ s/\[ProgDef\]\n/$progdesc/smg;

    print $wtr $out;
}
