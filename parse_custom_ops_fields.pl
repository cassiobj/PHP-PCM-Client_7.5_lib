#!/opt/portal/ThirdParty/perl/5.18.2/bin/perl
#
#	@(#)%Portal Version: parse_custom_ops_fields.pl:PlatformR2Int:5:2006-Sep-12 03:34:31 %
#
# Copyright (c) 1996, 2014, Oracle and/or its affiliates. All rights reserved.
#       
#       This material is the confidential property of Oracle Corporation or its
#       licensors and may be used, reproduced, stored or transmitted only in
#       accordance with a valid Oracle license or sublicense agreement.
#

#
# CONFIG

use Getopt::Std;

my $prop_info = "''
'
'	Copyright (c) 1998 - 2013 Oracle Corporation. All rights reserved.
' 
'	This material is the confidential property of Oracle Corporation.
'	or its subsidiaries or licensors and may be used, reproduced, stored
'	or transmitted only in accordance with a valid Oracle license or
'	sublicense agreement.
''\n";

my $me = 'parse_custom_ops_fields';
my $usage = 
"Usage: $me -L <language> -I <input file> -O <output file> [-P <Java Package>] [-n <No portal_op_fields.dat>]
        -h      help
        -L      language: pcmc, pcmcom or pcmjava
        -I      input file
        -O      output file (or directory if L is pcmjava)
        -P      Package for generated Java classes
        -n      No portal_op_fields.dat\n";	

#
# constants
# map PIN_FLDT_xxx to the pre-shifted type value
my %fldt_map = (
	PIN_FLDT_UNUSED		=> 0x00000000,
	PIN_FLDT_INT		=> 0x01000000,
	PIN_FLDT_UINT		=> 0x02000000,
	PIN_FLDT_ENUM		=> 0x03000000,
	PIN_FLDT_NUM		=> 0x04000000,
	PIN_FLDT_STR		=> 0x05000000,
	PIN_FLDT_BUF		=> 0x06000000,
	PIN_FLDT_POID		=> 0x07000000,
	PIN_FLDT_TSTAMP		=> 0x08000000,
	PIN_FLDT_ARRAY		=> 0x09000000,
	PIN_FLDT_SUBSTRUCT	=> 0x0A000000,
	PIN_FLDT_OBJ		=> 0x0B000000,
	PIN_FLDT_BINSTR		=> 0x0C000000,
	PIN_FLDT_ERR		=> 0x0D000000,
	PIN_FLDT_DECIMAL	=> 0x0E000000,
);

getopts('hnL:I:O:P:');

#
# Exit if help is expected.
#
if ((defined $opt_h) && $opt_h)
{
	die $usage;
}

if ((! defined $opt_L) || (! defined $opt_I)
	 || (! defined $opt_O))
{
	die $usage;
}
my $DEBUG = 0;
my $version = 0;
my $n_ops = 0;
my $len_ops = 0;
my $n_flds = 0;
my $len_flds = 0;
my $n_masked_flds = 0;
my $len_masked_flds = 0;
my %op_list = ();
my %fld_list = ();
my %masked_fld_list = ();
my $fn;
my $fname;
my $out;
my $strings_offset;
my $file_size;
my $offset;
my $clsname;
my $fldname;
my $fldnum;
my $pintype;
my $opname;
my $opnum;
my $pinclass;
my $key;
my $value;
my $len;
my $written;
my $resString;

if ($opt_L eq "pcmc") {
	
	$DEBUG = $ENV{"DEBUG"};

	#
	# variables
	$version = 0x101;
	$n_ops = 0;
	$len_ops = 0;		# length of ops strings + null.
	$n_flds = 0;
	$len_flds = 0;		# length of fields strings + null.
	$n_masked_flds = 0;
	$len_masked_flds = 0;		# length of masked fields strings + null.
	$in_comment = 0;
	%op_list = ();
	%fld_list = ();
	%masked_fld_list = ();

	if (defined $opt_I) {
		open(OUTFILE, ">$opt_O");
		binmode OUTFILE;
		# This check is to make sure wink-ins will work during Portal's internal build.
		if ($opt_I =~ m/PINBUILD_PUBLISH_DIR/) {
			$opt_I =~ s/PINBUILD_PUBLISH_DIR/$ENV{PINBUILD_PUBLISH_DIR}/;
			print "In_filename changed to $opt_I\n";
		}
		open(IFILE, $opt_I);
		while(<IFILE>) {
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
				$n_ops++;
				$op_list{$1} = "$2" ;
				$len_ops += length("$1") + 1;
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
				$fld_list{$fn} = $fname ;
				$len_flds += length($fname) + 1;
			}
			# check if this is a field being defined as masked
			elsif (/^\s*(\w+)\s+(\w+.*$)/) {
				$fname = "$1";
				$masked_fname = "$2";
				if ( $masked_fname =~ m/masked/ ) {
					if ($DEBUG) {
						print "got masked field name:$fname \n";
					}
					
					# Get the field number and check if it is string field
					$found = 0;
					while (($key, $value) = each %fld_list) {
						if ( $value eq $fname) {
							$found = 1;
							$fn = $key;
							if( ($fn & 0xFF000000) != $fldt_map{PIN_FLDT_STR} ) {
								print "ERROR: Masked Field $fname is not a string field \n";
							}
							else {
								$masked_fld_list{$fn} = $fname;
								$n_masked_flds++;
								$len_masked_flds += length($fname) + 1;
							}
							# break;
						}
					}
					if (!$found) {
						if ((defined $opt_n) && $opt_n) {
							# Creating portal_op_flds.dat
							print "ERROR: Unknown field $fname\n";
							exit(1);
						}
						else {
							# Customer ops/fields.dat
							require pcmif;
							# Get the field number from portal_op_flds.dat
							$fn = pcmif::pin_perl_field_of_name($fname);	
							if ($fn == 0) {
								print "ERROR: Unknown field $fname\n";
								exit(1);
							}
							if( ($fn & 0xFF000000) != $fldt_map{PIN_FLDT_STR} ) {
								print "ERROR: Masked Field $fname is not a string field \n";
							}
							else {
								$masked_fld_list{$fn} = $fname;
								$n_masked_flds++;
								$len_masked_flds += length($fname) + 1;
							}
						}
					}
				}
				else {
					print "ERROR: Unknown attribute ($masked_fname) for field $fname\n";
				}

			} #Mask field
		}
	}

    print "Number of opcodes defined: $n_ops, \n";
    print "Total size of the opcodes: $len_ops,\n";
    print "Number of fields defined: $n_flds, \n";
    print "Total size of the fields: $len_flds\n";
    print "Number of masked fields defined: $n_masked_flds,\n";
    print "Total size of the masked fields: $len_masked_flds\n";

	# now write the stuff to the output file
	#

	# magic + version
	$out = pack "a8NV", "\001opsflds", $version, $version ;
	if (syswrite(OUTFILE, $out, 16) != 16) { die "bad write $!\n" ; }

	$strings_offset = (16 * 4) + (16 * $n_ops) + (16 * $n_flds) + (16 * $n_masked_flds);
	$file_size = $strings_offset + $len_ops + $len_flds + $len_masked_flds; 
	print "file size: $file_size\n";

	# file size + spare
	$out = pack "NVNV", $file_size, $file_size, 0, 0 ;
	if (syswrite(OUTFILE, $out, 16) != 16) { die "bad write $!\n" ; }

	# n_ops, n_flds
	$out = pack "NVNV", $n_ops, $n_ops, $n_flds, $n_flds;
	if (syswrite(OUTFILE, $out, 16) != 16) { die "bad write $!\n" ; }

	# n_masked_flds, pad1
	$out = pack "NVNV", $n_masked_flds, $n_masked_flds, 1, 2  ;
	if (syswrite(OUTFILE, $out, 16) != 16) { die "bad write $!\n" ; }

	$offset = (16 * 4);

	# next there are three arrays of ofx_entry's, 1st for ops, 2nd for fields,
	# 3rd for masked fields
	# for each ops, write the value + offset to string; then string.
	while (($value, $key) = each %op_list) {
		$out = pack "NVNV", $key, $key, $strings_offset, $strings_offset ;
		if (seek(OUTFILE, $offset, 0) != 1) { die "bad seek op ent $!\n"; }
		if (syswrite(OUTFILE, $out, 16) != 16) {
			die "bad write of op_list $key $!\n" ;
		}
		$len = length($value) + 1;
		$out = pack "a$len" . "x", $value ;
		if (seek(OUTFILE, $strings_offset, 0) != 1) { die "bad seek ops $!\n"; }
		$written = syswrite(OUTFILE, $out, $len);
		if ($written != $len) {
			die "bad write of op_list $value, wrote $written, len $len\n" ;
		}
		$offset += 16;
		$strings_offset += $len;
	}
	
	# for each field, write the value + offset to string; then string.
	while (($key, $value) = each %fld_list) {
		$out = pack "NVNV", $key, $key, $strings_offset, $strings_offset ;
		if (seek(OUTFILE, $offset, 0) != 1) { die "bad seek fld ent $!\n"; }
		if (syswrite(OUTFILE, $out, 16) != 16) {
			die "bad write of fld_list $key $!\n" ;
		}
		$len = length($value) + 1;
		$out = pack "a$len" . "x", $value ;
		if (seek(OUTFILE, $strings_offset, 0) != 1) { die "bad seek flds $!\n"; }
		$written = syswrite(OUTFILE, $out, $len);
		if ($written != $len) {
			die "bad write fld_list $value, wrote $written, len $len: $!\n" ;
		}
		$offset += 16;
		$strings_offset += $len;
	}

	# for each masked field, write the value + offset to string; then string.
	while (($key, $value) = each %masked_fld_list) {
		$out = pack "NVNV", $key, $key, $strings_offset, $strings_offset ;
		if (seek(OUTFILE, $offset, 0) != 1) { die "bad seek masked fld ent $!\n"; }
		if (syswrite(OUTFILE, $out, 16) != 16) {
			die "bad write of fld_list $key $!\n" ;
		}
		$len = length($value) + 1;
		$out = pack "a$len" . "x", $value ;
		if (seek(OUTFILE, $strings_offset, 0) != 1) { die "bad seek masked flds $!\n"; }
		$written = syswrite(OUTFILE, $out, $len);
		if ($written != $len) {
			die "bad write masked_fld_list $value, wrote $written, len $len: $!\n" ;
		}
		$offset += 16;
		$strings_offset += $len;
	}
    print "Done parsing custom ops fields with no error found."; 
	exit(0);
}
elsif ($opt_L eq "pcmcom" ) {


	#
	# Walk through the list of files passed in
	#
	if (defined $opt_I) {
		open(OUTFILE, ">$opt_O");
		open(IFILE, $opt_I);
		print OUTFILE "$prop_info";
		while(<IFILE>) {
			#
			# Accept lines matching #define XXX PIN_MAKE_FLD(PIN_FLDT_*, nnn)
			#
			if (/\s*#\s*define\s+(\w+)\s+PIN_MAKE_FLD\s*\(\s*(PIN_FLDT_\w+)\s*,\s*(\d+)\s*\).*$/) {
				print OUTFILE "Public Const $1 = $2 + $3\n";
			}

			#
			# Walk through the list of files passed in
			#
       			#
       			# Accept lines matching #define XXX nnn
       			#
       			if (/\s*#\s*define\s+(\w+)\s+(\d+).*$/) {
				$opname = $1;
				$opnum  = $2;
				print OUTFILE "Public Const $opname = $opnum\n";
			}
		}
	}
    }
elsif ($opt_L eq "pcmjava" ) {


my %PINTYPE = (
	"PIN_FLDT_UNUSED",         "0",
	"PIN_FLDT_INT",            "1",
	"PIN_FLDT_UINT",           "2",
	"PIN_FLDT_ENUM",           "3",
	"PIN_FLDT_NUM",            "4",
	"PIN_FLDT_STR",            "5",
	"PIN_FLDT_BUF",            "6",
	"PIN_FLDT_POID",           "7",
	"PIN_FLDT_TSTAMP",         "8",
	"PIN_FLDT_ARRAY",          "9",
	"PIN_FLDT_SUBSTRUCT",      "10",
	"PIN_FLDT_OBJ",            "11",
	"PIN_FLDT_BINSTR",         "12",
	"PIN_FLDT_ERR",            "13",
	"PIN_FLDT_DECIMAL",        "14",
);

my %PINCLASS = (
	"PIN_FLDT_UNUSED",         "XXX",
	"PIN_FLDT_INT",            "IntField",
	"PIN_FLDT_UINT",           "UIntField",
	"PIN_FLDT_ENUM",           "EnumField",
	"PIN_FLDT_NUM",            "NumField",
	"PIN_FLDT_STR",            "StrField",
	"PIN_FLDT_BUF",            "BufField",
	"PIN_FLDT_POID",           "PoidField",
	"PIN_FLDT_TSTAMP",         "TStampField",
	"PIN_FLDT_ARRAY",          "ArrayField",
	"PIN_FLDT_SUBSTRUCT",      "SubStructField",
	"PIN_FLDT_OBJ",            "ObjField",
	"PIN_FLDT_BINSTR",         "BinStrField",
	"PIN_FLDT_ERR",            "ErrField",
	"PIN_FLDT_DECIMAL",        "DecimalField",
);

my $elemid = 0;
my $maxnum = 0;
my $opfile_open=0;
my $prop_infoj = $prop_info;
$prop_infoj =~ s/^''/\/\*/g;
$prop_infoj =~ s/''/\ *\//g;
$prop_infoj =~ s/'/ */g;

open( IFILE, "<$opt_I" ) || die "Cant open $opt_I";

chdir( $opt_O );
open( ROUT, ">InfranetPropertiesAdditions.properties" );

if( defined $opt_P ) {
    print ROUT "infranet.custom.field.package=$opt_P\n";
}

while (<IFILE>) {
    #
    # Accept lines matching #define XXX PIN_MAKE_FLD(PIN_FLDT_*, nnn)
    #
    if (/\s*#\s*define\s+(\w+)\s+PIN_MAKE_FLD\s*\(\s*(PIN_FLDT_\w+)\s*,\s*(\d+)\s*\).*$/) {
	$fldname = $1;
	$clsname = ucfirst (lc ($fldname));
	if (defined ($PINCLASS{$2})) {
	    $pinclass = $PINCLASS{$2};
	    $pintype = $PINTYPE{$2};
	} else {
	    die "$me: Unknown PIN field type $2 for field $1\n";
	}
	$fn = $fldt_map{$2} + $3;
	$fld_list{$fn} = $fldname;

	$resString=ucfirst (lc (substr ($clsname,3)));

	$fldnum  = $3;

	print ROUT "infranet.custom.field.$fldnum=$fldname\n";


	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;
	$clsname =~ s/_(.+)/ucfirst($1)/e;

	open( JOUT, ">$clsname" . ".java" );

	print JOUT "$prop_infoj";

	if( defined $opt_P ) {
	    print JOUT "package $opt_P;\n\n";
	}
	print JOUT "import com.portal.pcm.$pinclass;\n\n";
	print JOUT "/**\n";
	print JOUT " * Specific Field subclasses. This subclasses of <code>Field</code>\n";
	print JOUT " * is used with the FList class to specifiy which field is being\n";
	print JOUT " * accessed, and its type.  The type information is used to provide\n";
	print JOUT " * compile time type checking.  These classes are auto generated\n";
	print JOUT " * \@version 1.0 " . localtime(time) . "\n";
	print JOUT " * \@author Autogenerated\n";
	print JOUT " */\n\n";

	print JOUT "public class $clsname extends $pinclass {\n";
	print JOUT "/**\n";
	print JOUT " * Constructs an instance of <code>$clsname</code>\n";
	print JOUT " */\n";
	print JOUT "\tpublic $clsname() { super($fldnum, $pintype); }\n";
	print JOUT "/**\n";
	print JOUT " * Returns an instance of <code>$clsname</code>\n";
	print JOUT " * \@return An instance of <code>$clsname</code>\n";
	print JOUT " */\n";
	print JOUT "\tpublic static synchronized $clsname getInst() { \n";
	print JOUT "\t\tif( me == null ) me = new $clsname();\n";
	print JOUT "\t\treturn me;\n\t}\n";
	print JOUT "\tprivate static $clsname me;\n}\n\n";

	if( $maxnum < $fldnum ) {
	    $maxnum = $fldnum;
	}
	
	close( JOUT );
    }
    elsif (/\s*#\s*define\s+(\w+)_OP_(\w+)\s+(\d+).*$/) {
	     if( $opfile_open == 0 ) {
		 open( OPOUT, ">CustomOp.java" );
		 print OPOUT "$prop_infoj";

		 if( defined $opt_P ) {
		     print OPOUT "package $opt_P;\n\n";
		 }
		 print OPOUT "/**\n";
		 print OPOUT " * Custom opcode constants\n";
		 print OPOUT " * These are auto generated from <code>$opt_I</code>\n";
		 print OPOUT " * \@version 1.0 " . localtime(time) . "\n";
		 print OPOUT " * \@author Autogenerated\n";
		 print OPOUT " */\n\n";

		 print OPOUT "public class CustomOp {\n";

	 	$opfile_open = 1;
	     }
	if($1 eq "PCM")
	{
		print OPOUT "\tpublic static final int $2 = $3;\n";
	}
	else
	{
		print OPOUT "\tpublic static final int $1_OP_$2 = $3;\n";
	}
    }
    	#Maskable fields lines matching <FIELD NAME> <attributes>
	elsif (/^\s*(\w+)\s+(\w+.*$)/) {
		$fname = "$1";
		$masked_fname = "$2";
		if ( $masked_fname =~ m/masked/ ) {
			if ($DEBUG) {
				print "got masked field name:$fname \n";
			}
			$found = 0;
			# Get the field number and check if it is string field
			while (($key, $value) = each %fld_list) {
				if ( $value eq $fname) {
					$found = 1;
					$fn = $key;
					if( ($fn & 0xFF000000) != $fldt_map{PIN_FLDT_STR} ) {
						print "ERROR: Masked Field $fname is not a string field \n";
					}
					else {
						$fnum = $fn & 0x00FFFFFF;
						print ROUT "infranet.custom.masked.field.$fnum=$fname\n";
					}
					# break
				}
			}
			if (!$found) {
				if ((defined $opt_n) && $opt_n) {
					# Creating portal_op_flds.dat
					print "ERROR: Unknown field $fname\n";
					exit(1);
				}
				else {
					# Customer ops/fields.dat
					require pcmif;
					# Get the field number from portal_op_flds.dat
					$fn = pcmif::pin_perl_field_of_name($fname);	
					if ($fn == 0) {
						print "ERROR: Unknown field $fname\n";
						exit(1);
					}
					if( ($fn & 0xFF000000) != $fldt_map{PIN_FLDT_STR} ) {
						print "ERROR: Masked Field $fname is not a string field \n";
					}
					else {
						$fnum = $fn & 0x00FFFFFF;
						print ROUT "infranet.custom.masked.field.$fnum=$fname\n";
					}
				}
			}
		}
		else {
			print "ERROR: Unknown attribute ($masked_fname) for field $fname\n";
		}
 	}
  }

if( $opfile_open == 1 ) {
    print OPOUT "\n\tpublic static String opToString( int op ) {\n";
    print OPOUT "\t\ttry {\n";
    print OPOUT "\t\t\tjava.lang.reflect.Field[] flds = CustomOp.class.getFields();\n";
    print OPOUT "\t\t\tfor( int i = 0; i < flds.length; i++ ) {\n";
    print OPOUT "\t\t\t\ttry {\n";
    print OPOUT "\t\t\t\t\tint val = flds[i].getInt(null);\n";
    print OPOUT "\t\t\t\t\tif( val == op ) {\n";
    print OPOUT "\t\t\t\t\t\treturn flds[i].getName();\n";
    print OPOUT "\t\t\t\t\t}\n";
    print OPOUT "\t\t\t\t} catch( IllegalAccessException e ) { continue; }\n";
    print OPOUT "\t\t\t\t  catch( IllegalArgumentException e ) { continue; }\n";
    print OPOUT "\t\t\t}\n";
    print OPOUT "\t\t} catch( SecurityException e ) {}\n";
    print OPOUT "\n\t\treturn null;\n";
    print OPOUT "\t}\n";
    print OPOUT "\n\tpublic static int stringToOp( String op ) {\n";
    print OPOUT "\t\ttry {\n";
    print OPOUT "\t\t\tjava.lang.reflect.Field[] flds = CustomOp.class.getFields();\n";
    print OPOUT "\t\t\tfor( int i = 0; i < flds.length; i++ ) {\n";
    print OPOUT "\t\t\t\ttry {\n";
    print OPOUT "\t\t\t\t\tString name = flds[i].getName();\n";
    print OPOUT "\t\t\t\t\tif( name.equals(op) ) {\n";
    print OPOUT "\t\t\t\t\t\treturn flds[i].getInt(null);\n";
    print OPOUT "\t\t\t\t\t}\n";
    print OPOUT "\t\t\t\t} catch( IllegalAccessException e ) { continue; }\n";
    print OPOUT "\t\t\t\t  catch( IllegalArgumentException e ) { continue; }\n";
    print OPOUT "\t\t\t}\n";
    print OPOUT "\t\t} catch( SecurityException e ) {}\n";
    print OPOUT "\n\t\treturn -1;\n";
    print OPOUT "\t}\n";

    print OPOUT "}\n";

    close( OPOUT );

}

close(ROUT);

}    

print "Done parsing custom ops fields with no error found."; 
exit(0);

