#!/usr/bin/perl

use strict;
use warnings;

use Time::Local;
use Data::Dumper;
$Data::Dumper::Sortkeys=1;
use Math::BigInt;
use enum;

use enum qw(
  sec 
  min 
  hour 
  mday 
  mon 
  year 
  wday 
  yday 
  isdst
  year4d
  year2d
  wdayt3
  mont3
);
use constant WOYSTEP => 0.0201;
use constant WOYOFFSET => 0.002;
use constant PBYSTEP => 0.0201;
use constant PBQWARNING => 20;
use constant PBQCRITICAL => 25;

our %woroot= ();
our @pbroot= ();

sub date_to_epoch{
	my $t = shift;
	my @tm;
	if($t =~ /^(\d{4})-(\d\d)-(\d\d) (?:(\d\d):(\d\d):(\d\d))/){
		($tm[year4d],$tm[mon],$tm[mday],$tm[hour],$tm[min],$tm[sec]) = ($1,$2,$3,$4,$5,$6);
		#my @tm[year4d,mon,mday,hour,min,sec] = ($t =~ /^(\d{4})-(\d\d)-(\d\d) (?:(\d\d):(\d\d):(\d\d))/);
	}else{
		print "Invalid date $t\n";
	}
	--$tm[mon];
	$tm[year] = $tm[year4d];
	return Time::Local::timelocal_nocheck(@tm);
}

sub isSizeFinished{
	foreach(keys(%woroot)){
		if($woroot{$_}{"sizeIndex"} == 0 && $woroot{$_}{"processEnd"} - $woroot{$_}{"processStart"} > -1){
			return(0);
		}
	}
	return(1);
}

sub findUnassignedMax{
	my $maxId="0_0_0";
	my $curMax = -1;
	my $woPeriod = 0;
	foreach(keys(%woroot)){
		$woPeriod = $woroot{$_}{"processEnd"} - $woroot{$_}{"processStart"};
        	if($woroot{$_}{"sizeIndex"} == 0 && $woPeriod > $curMax){
	                $curMax = $woPeriod;
			$maxId = $_;
        	}
	}
	return($maxId);
}

sub gatherMTAData{
	open(my $logFile,"<","/home/sebas/mtadata.log") or die("Could not open file");
	my ($pid,$wostart,$woseq,$aid)=("0")x4;
	my ($size,$soft,$hard,$sent,$processStart,$processEnd) = (0)x6;
	$woroot{"${wostart}_${pid}_$woseq"} = {
		"wostart" => $wostart,
		"pid" => $pid,
		"woseq" => $woseq,
		"processStart" => $processStart,
		"processEnd" => -1,
		"id" => "0_0_0",
		"x1" => 1.1,#Time start, left
		"x2" => -1.1,#Time end, right
		"y1" => WOYSTEP,#low unit, reference start,Bottom
		"y2" => WOYSTEP*30,#high unit, something (sent,bounce,etc),Top
		"z" => 0.0,#high unit, something (sent,bounce,etc),Top
		"r" => 1.0,
		"g" => 1.0,
		"b" => 1.0,
		"a" => 1.0,
		"sizeIndex"=>0,
	};
	my $PbCurY = -1.0;
	push(@pbroot,{
		"processStart" => $processStart - 1,
		"y" => $PbCurY,
		"x" => -1.0,
		"z" => 0.0,
	});
	my $woi = 1;#Iterators
	my $pbi = 1;
	my $wsre = qr/^([-0-9: ]{19})\.\d* \d* pbs: \(Q*(\d*)_*(\d*)_(\d*)_([-\d]*)\) Starting on.*/; #Should be a config file so we support more logs.
	my $were = qr/^([-0-9: ]{19})\.\d* \d* pbs: \(Q*(\d*)_*(\d*)_(\d*)_([-\d]*)\) done with.*; \((\d+)\) (\d+) total, (\d+) S, (\d+) T .R.(\d*),.*/;
	while(my $line = <$logFile>){
		if($line =~ /$wsre/){
			$processStart=date_to_epoch($1);
			if($processStart  < 0){
				print "//[EE] Could not translate $1 to epoch... bailing...\n";
				last;
			}
			if($3 eq ""){#This is a initial WO, not a retry WO
				$wostart=$2;
			}else{
				$wostart=$3;
			}
			$pid=$4;
			$woseq=$5;
			$woseq=~s/-/_/;#We'll have this as a js variable so let's have a valid var name.
			$woroot{"${wostart}_${pid}_$woseq"}={
				"wostart" => $wostart,
				"pid" => $pid,
				"woseq" => $woseq,
				"processStart" => $processStart + 0,
				"processEnd" => -1,
				"id" => "$wostart-$pid-$woseq",
				"sizeIndex"=>0,
				"y1" => -10,
				"z" => 0.0,
			};
			$PbCurY+=PBYSTEP;
			push(@pbroot,{
				"processStart" => $processStart + 0,
				"y" => $PbCurY,
				"z" => 0.0,
			});
		}elsif($line =~ /$were/){
			$processEnd=date_to_epoch($1);
			if($3 eq ""){#This is an initial WO, not a retry WO
				$wostart=$2;
			}else{
				$wostart=$3;
			}
			($pid,$woseq,$aid,$size,$hard,$soft,$sent)=($4,$5,$6,$7,$8,$9,$10);
			$woseq=~s/-/_/;#We'll have this as a js variable so let's have a valid var name.
			if(!exists($woroot{"${wostart}_${pid}_$woseq"})){
				$woroot{"${wostart}_${pid}_$woseq"} = {
					"wostart" => $wostart,
					"pid" => $pid,
					"woseq" => $woseq,
					"processStart" => -1,#special value we will adjust later to the minimum valid epoch found.
					"processEnd" => -1,
					"id" => "$wostart-$pid-$woseq",
					"sizeIndex"=>0,
					"y1" => -10,
					"z" => 0.0,
				};
				foreach(@pbroot){
					$_{"y"}+=PBYSTEP;
				}
				$PbCurY+=PBYSTEP;
			}
			$woroot{"${wostart}_${pid}_$woseq"}{"aid"}=$aid;
			$woroot{"${wostart}_${pid}_$woseq"}{"size"}=$size;
			$woroot{"${wostart}_${pid}_$woseq"}{"soft"}=$soft;
			$woroot{"${wostart}_${pid}_$woseq"}{"hard"}=$hard;
			$woroot{"${wostart}_${pid}_$woseq"}{"processEnd"}=$processEnd;
			$woroot{"${wostart}_${pid}_$woseq"}{"sent"}=$sent;
			$PbCurY-=PBYSTEP;
			push(@pbroot,{
				"processStart" => $processStart + 0,
				"y" => $PbCurY,
				"z" => 0.0,
			});
		}else{
			print "//[EE]: Pattern not found for line $. -> $line";
		}
	}
	close($logFile);
	if(!%woroot){
		print "//[EE]: No entries processable found...\n";
		return 0;
	}
	my $minproc=date_to_epoch("3030-01-01 00:00:00");
	my $maxproc=-1;
	foreach(keys(%woroot)){
		next if($_ eq "0_0_0");
		if($minproc > $woroot{$_}{"processStart"} && $woroot{$_}{"processStart"} > 0){
			$minproc = $woroot{$_}{"processStart"};
		}
		if($minproc > $woroot{$_}{"processEnd"} && $woroot{$_}{"processEnd"} > 0){#We may have only the end of a wo process
			$minproc = $woroot{$_}{"processEnd"};
		}
		if($maxproc < $woroot{$_}{"processEnd"} && $woroot{$_}{"processEnd"} > 0){
			$maxproc = $woroot{$_}{"processEnd"};
		}
		if($maxproc < $woroot{$_}{"processStart"} && $woroot{$_}{"processStart"} > 0){#We may only have the start of a wo process
			$maxproc = $woroot{$_}{"processStart"};
		}
	}
	print "//Minimizing epochs: ( $minproc, $maxproc) to: (0,".($maxproc-$minproc).")\n";
	$maxproc=$maxproc-$minproc;
	#Normalizing values.
	foreach(keys(%woroot)){
		#Some values might be unknown or not accounted for, they are set to -1
		if($woroot{$_}{"processStart"} == -1){
			$woroot{$_}{"processStart"} = 1;#why not 0?
		}else{
			$woroot{$_}{"processStart"}-=$minproc;
		}
		if(!exists($woroot{$_}{"processEnd"}) || $woroot{$_}{"processEnd"} == -1){
			$woroot{$_}{"processEnd"} = $maxproc;#basically outside of the map, the process is still running in our window
		}else{
			$woroot{$_}{"processEnd"}-=$minproc;
		}
	}
	my $count = 0;
	#print Dumper(\@pbroot);
	foreach(@pbroot){
		if($_->{"processStart"} == -1){
			$_->{"processStart"} = 1;#why not 0?
		}else{
			$_->{"processStart"}-=$minproc;
		}
		my $queueSize=((1.0+$_->{"y"})/WOYSTEP);
		$_->{"queueSize"}=$queueSize;
		if($queueSize >= PBQWARNING){#These should be the thresholds as in from nagios.
			if($queueSize <= PBQCRITICAL){
				$_->{"r"}=1.0;$_->{"g"}=1.0;$_->{"b"}=0.0;
			}else{
				$_->{"r"}=1.0;$_->{"g"}=0.0;$_->{"b"}=0.0;
			}
		}else{
			$_->{"r"}=0.0;$_->{"g"}=1.0;$_->{"b"}=0.0;
		}
	}
	#Let's translate x (and y initially...)
	foreach(keys(%woroot)){
		#preset to CCW...
		#print "//Transforming ( ".$woi->$processStart.",".") to: ";
		$woroot{$_}{"x1"}=(1.9*$woroot{$_}{"processEnd"}/$maxproc)-0.95;
		$woroot{$_}{"x2"}=(1.9*$woroot{$_}{"processStart"}/$maxproc)-0.95;
		#print "(".$woi->x1.",".$woi->x3.")\n";
		
	}
	foreach(@pbroot){
		$_->{"x"} = (1.9*$_->{"processStart"}/$maxproc)-0.95;
	}
	$count = 1;
	#Order them by size
	while(!isSizeFinished()){
		$woroot{findUnassignedMax()}{"sizeIndex"}=$count++;
	}
	$woroot{"0_0_0"}{"processEnd"}=$maxproc;#We need to have this preserved to -1 until after the sizeOrder...
#	srand();
#	foreach(keys(%woroot)){
#		if($woroot{$_}{"r"} == 0.0 && $woroot{$_}{"g"} == 0.0 && $woroot{$_}{"b"} == 0.0){
#			$r=(rand()%100)/100;
#			$g=(rand()%100)/100;
#			$b=(rand()%100)/100;
#			infectAID(r,g,b,alpha,$woi->aid); # Let's move this to javascript
#		}
#	}
	return ($minproc,$maxproc);
}
sub resolveOverlaps{
	my $maxproc=shift;
	print "//We got ".scalar(keys(%woroot))." items,sorting keys\n";
	my @sortedkeys = sort { $woroot{$a}{"sizeIndex"} <=> $woroot{$b}{"sizeIndex"} } keys %woroot;
	#}}}}#WTF... vim syntax fail.
	#Once ordered by size we can save the 
	my $curRep = 1;
	foreach my $woA(@sortedkeys){
		next if($woA eq "0_0_0");
		$woroot{$woA}{"y1"} = 0.0;
		my @woFixed=@sortedkeys[1..$curRep];
		next if($#woFixed == -1);
		my $iter = 0;
		foreach my $key(@woFixed){
			#Let's remove values that are not relevant to us: stuff we don't overlap.
			if(	$woroot{$woA}{"z"} != $woroot{$key}{"z"} ||
				#abs($woroot{$woA}{"processStart"} - $woroot{$key}{"processStart"}) > $maxproc || #This surprisingly slows it down.
				($woroot{$woA}{"processStart"} > $woroot{$key}{"processEnd"} &&
				$woroot{$key}{"processStart"} > $woroot{$woA}{"processEnd"}) ||
				$woA eq $key ||
				$key eq "0_0_0"
			){
				splice(@woFixed,$iter,1);
				#print "Splicing item number [$iter] $key ".$woroot{$woA}{"processStart"}." > ".$woroot{$key}{"processEnd"}." || ".$woroot{$key}{"processStart"}." > ".$woroot{$woA}{"processEnd"}." ".Dumper(\@woFixed) if ($curRep <= 6);
			}else{
				$iter++;
			}
		}
		$iter=0;
		#print "**** ".$woA." items = ".$#woFixed." => ".Dumper(\@woFixed) if ($curRep <= 6);
		while($iter < $#woFixed){#Raise until free
			#print $woA." vs ".$woFixed[$iter]."\n" if ($curRep <= 6);
			if($woroot{$woA}{"processStart"} <= $woroot{$woFixed[$iter]}{"processEnd"} && $woroot{$woFixed[$iter]}{"processStart"} <= $woroot{$woA}{"processEnd"}){
				if($woroot{$woA}{"y1"} == $woroot{$woFixed[$iter]}{"y1"}){
					$woroot{$woA}{"y1"}+=WOYSTEP;#raise, we could raise by, say, sent,bounce, and then normalize.i.e. +=$woroot{$woFixed[$iter]}{"sent"}
					splice(@woFixed,$iter,1);#We won't overlap again.
					$iter=-1;
				}else{
					if($woroot{$woA}{"y1"} > $woroot{$woFixed[$iter]}{"y1"}){#This is below us. no overlap.
						splice(@woFixed,$iter,1);
						$iter--;
					}
				}
			}
			$iter++;
		}
		$woroot{$woA}{"y2"}=$woroot{$woA}{"y1"} + WOYSTEP;
		$curRep++;
	}
	return;
	#TODO upload to mongoDB, calculating the same scenario over and over makes no sense.
}
sub printWebGL{
	my $minproc=shift;
	my @sortedkeys = sort { $woroot{$a}{"sizeIndex"} <=> $woroot{$b}{"sizeIndex"} } keys %woroot;
	#}}}}
	print "var minProc = $minproc;\n";
	print "function loadMTAData(){";
	foreach my $woA(@sortedkeys){
		next if($woA eq "0_0_0");
		print "\tvar mtaWo_x_".$woA." = new MTAStat(".
			$woroot{$woA}{"wostart"}.",".
			$woroot{$woA}{"pid"}.",".
			"\"".$woroot{$woA}{"woseq"}."\",".
			$woroot{$woA}{"x1"}.",".
			$woroot{$woA}{"x2"}.",".
			($woroot{$woA}{"y1"} + WOYOFFSET).",".
			($woroot{$woA}{"y2"} - WOYOFFSET).",".
			"\"".$woroot{$woA}{"id"}."\",".
			$woroot{$woA}{"z"}.",".
			(exists($woroot{$woA}{"aid"})?$woroot{$woA}{"aid"}:"0").",". #We might have incomplete data...
			$woroot{$woA}{"processStart"}.",".
			$woroot{$woA}{"processEnd"}.",".
			$woroot{$woA}{"sizeIndex"}.",".
			(exists($woroot{$woA}{"size"})?$woroot{$woA}{"size"}.",".$woroot{$woA}{"soft"}.",".$woroot{$woA}{"sent"}.",".$woroot{$woA}{"hard"}:"0,0,0,0"). #We might have incomplete data...
		");";
		print "scene.add( mtaWo_x_$woA.printWebGL());\n";
	#	print "wo".$woA."colors = [".$woroot{$woA}{"r"}.",".$woroot{$woA}{"g"}.",".$woroot{$woA}{"b"}."];\n";#Bottom left
	}
	print "}\n";
	print "function loadPBData(){";
	print "\tvar mtaPbShape = new THREE.Shape();";
	print "mtaPbShape.moveTo(1,-1);";
	foreach(@pbroot){
		print "mtaPbShape.lineTo(".$_->{"x"}.",".$_->{"y"}.");\n";
	}
	print "\tmtaPbPoints = mtaPbShape.createPointsGeometry();";
	print "\tgroup.add(new THREE.Line( mtaPbPoints, new THREE.LineBasicMaterial( { color: 0x00FF00, linewidth: 0.2 } ) ));\n";
	print "}";
}

my ($epochstart,$maxproc) = (gatherMTAData());
resolveOverlaps($maxproc);
print printWebGL($epochstart);
