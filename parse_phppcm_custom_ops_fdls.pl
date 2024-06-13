#!/opt/portal/ThirdParty/perl/5.18.2/bin/perl
#
#  PHP-PCM-Client Custom Fields and Opcodes converter.
#
#	
#
# 		Copyright Cassio Jemael. All rights reserved.
#       


use strict;
use warnings;


my %fldt_map = (
	PIN_FLDT_UNUSED		=> 0,
	PIN_FLDT_INT		=> 1,
	PIN_FLDT_UINT		=> 2,
	PIN_FLDT_ENUM		=> 3,
	PIN_FLDT_NUM		=> 4,
	PIN_FLDT_STR		=> 5,
	PIN_FLDT_BUF		=> 6,
	PIN_FLDT_POID		=> 7,
	PIN_FLDT_TSTAMP		=> 8,
	PIN_FLDT_ARRAY		=> 9,
	PIN_FLDT_SUBSTRUCT	=> 10,
	PIN_FLDT_OBJ		=> 11,
	PIN_FLDT_BINSTR		=> 12,
	PIN_FLDT_ERR		=> 13,
	PIN_FLDT_DECIMAL	=> 14,
	PIN_FLDT_TIME	    => 15,
	PIN_FLDT_TEXTBUF	=> 16	
);


my $DEBUG=0;

my $me = 'parse_phppcm_custom_ops_fdls.pl';
my $usage = 
"\nUsage: $me -I <input file> -O <output file>
        -h      help        
        -I      input file
        -O      output file (or directory if L is pcmjava)
        \n";	

my $type_O_total = 0;
my $type_F_total = 0;
my $type_O_errors = 0;
my $type_F_errors = 0;

sub process_file {
    my ($file, $data_ref, $errors_ref) = @_;
    my $in_comment = 0;
    my $n_ops = 0;    
    my $len_ops=0;
    my $n_flds = 0;    
    my $len_flds=0;
    my $fname;
    my $fn;
    


    open(my $fh, '<', $file) or return "Error Open $file: $!";
    
    while (<$fh>)
    {
    		#
			# Assumption: 
			# 1. No nested comments
			# 2. No multiple comments on a single line
			# 3. No relevant statements after a comment line
			#
			if (/\/\*/ && !/\*\//) {
				$in_comment = 1;
			}
		
			# skip line with comments
			if (/^\s*\/\*.*\*\/\s*$/ || $in_comment) {
				if (/\*\//) {
					$in_comment = 0;
				}
				if ($DEBUG) {
					print "skipping comment: $_";
				}
			}
			# Skip special case
			elsif (/PIN_FLD_RESERVED\s/) {
			}
			# check if this is an op being defined
			elsif (/\s*#\s*define\s+(\w+)\s+(\d+).*$/) {
				if ($DEBUG) {
					print "got op: \"$1\", value $2\n";
				}
				$type_O_total++;
				my %record;								
				$record{nome} = $1;
				$record{numero} = $2;
				$record{tipo_informacao} = "O";				
				push @$data_ref, \%record;
			}	
			# check if this is a field being defined
			elsif (/\s*#\s*define\s+(\w+)\s+PIN_MAKE_FLD\s*\(\s*(PIN_FLDT_\w+)\s*,\s*(\d+)\s*\).*$/) {
				# $1 is name after prefix, $2 PIN_FLDT_xxx, $3 number
				if ($DEBUG) {
					print "got field: \"$1\"," .
					    " type $2, num $3\n";
				}
				$n_flds++;
			
				$fn = $fldt_map{$2} + $3 ;
				$fname = "$1" ;
				if ($DEBUG) {
					printf("fn is 0x%x, fname is \"%s\"\n", $fn, $fname);
				}				
				$type_F_total++;
#PIN_FLDT_DECIMAL			
				my %record;
				$record{nome} = $fname;
				$record{numero} = $3;
				$record{tipo_campo} = substr $2, 9 ;
				$record{tipo_informacao} = "F";
				push @$data_ref, \%record;
				
			}
			# check if this is a field being defined as masked
			elsif (/^\s*(\w+)\s+(\w+.*$)/) {
				$fname = "$1";				
				print "masked field will be ignored:$fname \n";
			} #Mask field    	
    }
	close $fh;
    return ($type_F_total + $type_O_total) ;
    
    
    #    while (my $line = <$fh>) {
#        chomp $line;
#        my @fields = split /\|/, $line;
#        if (@fields == 2 || @fields == 3) {
#            my %record;
#            $record{nome} = $fields[0];
#            $record{numero} = sprintf("%06d", $fields[-1]);
#            if (@fields == 3) {
#                $record{tipo_campo} = $fields[1];
#                $record{tipo_informacao} = 'F';
#            } else {
#                $record{tipo_informacao} = 'O';
#            }
#            push @$data_ref, \%record;
#        } else {
#            push @$errors_ref, "Erro: Formato inválido em $file - linha: $.";
#        }
#    }
#    close $fh;
}

sub write_csv {
    my ($file, $data_ref) = @_;

    open(my $fh, '>', $file) or return "Erro ao abrir $file para escrita: $!";
    
     # Ordenar @data pelo campo nome
    my @sorted_data = sort { $a->{nome} cmp $b->{nome} } @$data_ref;

    foreach my $row (@sorted_data) {
        printf $fh "%-60s|%-9s|%06d|%s|\n", $row->{nome}, $row->{tipo_campo} // '', $row->{numero}, $row->{tipo_informacao};
    }
    close $fh;
    return;
}

sub print_errors {
    my ($errors_ref) = @_;

    foreach my $error (@$errors_ref) {
        print STDERR "$error\n";
    }
}

sub print_summary {
    my ($type_O_total, $type_F_total, $type_O_errors, $type_F_errors, $total_ok) = @_;

    print "Summary:\n";
    print "Total Registers for Type Information O: $type_O_total\n";
    print "Total Registers for Type Information F: $type_F_total\n";
    print "Total Errors for Type Information O: $type_O_errors\n";
    print "Total Errors for Type Information F: $type_F_errors\n";
    print "Total OK: ${total_ok}\n";
}

sub main {
    my ($input_file, $csv_file) = @_;

    my (@data, @errors);
    my $total_ok=0;

    $total_ok = process_file($input_file, \@data, \@errors);
    

    write_csv($csv_file, \@data );

    print_errors(\@errors);

    
    print_summary($type_O_total, $type_F_total, $type_O_errors, $type_F_errors, $total_ok);
}

if (@ARGV < 2) {
    print STDERR $usage;
    exit 1;
}

my ($input_file, $csv_file);
while (my $arg = shift @ARGV) {
    if ($arg eq '-I') {
        $input_file = shift @ARGV;
    } elsif ($arg eq '-O') {
        $csv_file = shift @ARGV;
    } elsif ($arg eq '-h') {
        die $usage;
    }
}

main($input_file, $csv_file);
