#!/usr/bin/perl

use Device::SerialPort::Arduino;
#my $lastentries = `grep 'usb 3-1' /var/log/messages.log|tail -n 1`;#This changed in the latest kernel wtf
my $lastentries = `grep 'cdc_acm' /var/log/messages.log|tail -n 1`;#This changed in the latest kernel wtf
#Better use lsusb and avoid this nasty idea, got a webcam that is USB ACM and screws it all
if ($lastentries =~ /USB disconnect/){
	print "Arduino not connected\n";
	exit;
}
my $lastportfound = `grep ttyACM /var/log/messages.log|tail -n 1`;
my $port = "ACM1PT";
if ($lastportfound =~ /.*localhost kernel:.*cdc_acm 3-1:1.0: (.*): USB ACM device/){
	$port = $1;
}else{
	print "Arduino Serial Port not found...\n";
	exit;
}
my $Arduino = Device::SerialPort::Arduino->new(
	port     => "/dev/$port",
	baudrate => 9600,
	databits => 8,
	parity   => 'none',
) or die "No Arduino connected maybe?";
# Reading from Arduino via Serial
print $Arduino->receive(), "\n";
print $Arduino->receive(), "\n";
print $Arduino->receive(), "\n";
sleep 1;
# Send something via Serial
#$Arduino->communicate('oh hi!!11') or die 'Warning, empty string: ', "$!\n";
