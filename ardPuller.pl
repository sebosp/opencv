#!/usr/bin/perl

use Device::SerialPort::Arduino;
use IO::Handle;

my $AMIXER = '/usr/bin/amixer';
my $DEBUG = 0;
#my $ardVendor = "2341";
#my $ardProdID = "0001";
my $ardDev = `ls /dev/ttyACM*|tail -1`;#Ugly shit, remove, use Device::USB, currently not building for me.
#my $ardLocation = `udevadm info -a -p \$(udevadm info -q path -n $ardDev)`; or use this to make sure this is an arduino
#Better use lsusb and avoid this nasty idea, got a webcam that is USB ACM and screws it all
(my $ardPort = $ardDev) =~ s/^\/dev\/(.*)[\r\n]/$1/;
my $lastEntries = `grep $ardPort /var/log/inotify/current|tail -1`;
if ($lastEntries =~ /DELETE $ardPort/){
	print "[INIT]: Arduino not connected\n";
	exit;
}
my $Arduino = Device::SerialPort::Arduino->new(
	port     => "/dev/$ardPort",
	baudrate => 9600,
	databits => 8,
	parity   => 'none',
) or die "[INIT]: No Arduino connected maybe?";
# Reading from Arduino via Serial
my $sensors = "";
my $temp,$light,$slide,$volvalue = 0 x 4;
my $vollimitup,$vollimitdown,$volstep = 0 x 3;
my $audioIface = 'PCM Playback Volume';
my $amixervals = `$AMIXER -c 0 cget name='$audioIface'`;
#numid=32,iface=MIXER,name='Master Playback Volume'
# type=INTEGER,access=rw---R--,values=1,min=0,max=64,step=0
# values=62
#| dBscale-min=-64.00dB,step=1.00dB,mute=0
($vollimitdown,$vollimitup,$volvalue) = ($1,$2,$3) if ($amixervals =~ /^numid=.*[\r\n]*.*min=([^,]*),max=([^,]*),step.*[\r\n]*.*values=([0-9]*)[\r\n]*/m);
$volstep = ($vollimitup - $vollimitdown) / 10;
print "[INIT]: $AMIXER limits from $vollimitdown to $vollimitup. Current volume is $volvalue. Volume Step is $volstep\n";
while(1){
	$sensors = $Arduino->receive();
	STDOUT->printflush("[PULL]: $sensors\n");
	#Temp: 26.66. Light: 125. softPot: 99
	if ($sensors =~ /Temp: ([0-9.]*). Light: (\d{0,3}). softPot: (\d{0,3})/){
		($temp,$light,$slide) = ($1,$2,$3);
		if ($slide > 240){
			$volvalue = `$AMIXER -c 0 cget name='$audioIface' | grep : | cut -f2 -d=`;
			STDOUT->printflush("[STAT]: (1) Got amixer value: $volvalue\n") if ($DEBUG);
			if ($volvalue < $vollimitup){
				$volvalue+=$volstep;
				STDOUT->printflush("[INFO]: Setting Volume + 10 = $volvalue\n");
			}
			STDOUT->printflush("[STAT]: (2) Set amixer value: $volvalue\n") if ($DEBUG);
			`$AMIXER -q cset name='$audioIface' $volvalue`;
			STDOUT->printflush("[STAT]: (3) Got amixer value: $volvalue\n") if ($DEBUG);
			$volvalue = `$AMIXER -c 0 cget name='$audioIface' | grep : | cut -f2 -d=`;
		}elsif($slide < 20){
			$volvalue = `$AMIXER -c 0 cget name='$audioIface' | grep : | cut -f2 -d=`;
			if ($volvalue > $vollimitdown){
				$volvalue-=$volstep;
				STDOUT->printflush("Setting Volume -10 = $volvalue\n");
			}
			`$AMIXER -q cset name='$audioIface' $volvalue`;
		}
	}
}
# Send something via Serial
#$Arduino->communicate('oh hi!!11') or die 'Warning, empty string: ', "$!\n";
