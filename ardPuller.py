#!/usr/bin/python2

import serial
import re
import os
import sys
import datetime
import random
import pprint
import httplib, urllib
import pymongo

def pushOver(mtype,minterval,mvalue):
	conn = httplib.HTTPSConnection("api.pushover.net:443")
	conn.request("POST", "/1/messages.json",
	urllib.urlencode({
		"token": "yourTok3n",
		"user": "YourUserHash",
		"message": "["+str(mvalue)+"]"+ mtype + " in " + minterval,
	}), { "Content-type": "application/x-www-form-urlencoded" })
	conn.getresponse()

def statDays(mtype,mArray):
	minValue= min(mArray)
	maxValue= max(mArray)
	for i in 30,14,7,5,4,3,2,1:
		timerange=curtime - datetime.timedelta(i)
		maxQuery={
			u'ts':{
				"$gt":timerange
			},
			unicode(mtype):{
				"$gt":maxValue
			}
		}
		minQuery={
			u'ts':{
				"$gt":timerange
			},
			unicode(mtype):{
				"$lt":minValue
			}
		}
		if collection.find(maxQuery).count() == 0:
			print("Pushing Over new MAX "+mtype+" ["+str(maxValue)+"] in last "+str(i)+" day(s)")
			print maxQuery
			pushOver(mtype+" new MAX ",str(i) + (" day" if i == 1 else " days"),maxValue);
			break
		if collection.find(minQuery).count() == 0:
			print("Pushing Over new MIN "+mtype+" ["+str(minValue)+"] in last "+str(i)+" day(s)")
			print minQuery
			pushOver(mtype+" new MIN ",str(i) + (" day" if i == 1 else " days"),minValue);
			break

# Main Function
if __name__ == '__main__':
	#my $isXUp = `pidof /usr/bin/Xorg`;
	#if ($isXUp ne "")
	#	my $nvidiatemp = `nvidia-settings -q gpucoretemp -t`;
	#	print "GPU: $1,\n";
	AMIXER = '/usr/bin/amixer'
	DEBUG = 0
	client = pymongo.MongoClient()
	db = client.ardpuller
	collection = db.measurements
	#my $ardVendor = "2341";
	#my $ardProdID = "0001";
	ardPort = "/dev/arduinouno" #should be in a conf file
	#ardPort = "/dev/ttyACM0" #should be in a conf file
	#my $ardLocation = `udevadm info -a -p \$(udevadm info -q path -n $ardDev)`; or use this to make sure this is an arduino
	#Better use lsusb and avoid this nasty idea, got a webcam that is USB ACM and screws it all
	#Or use inotify?
	print("ardPort: ",ardPort)
	ser = serial.Serial(ardPort, 9600)
	temp=light=slide=volvalue=vollimitup=vollimitdown=volstep=0
	audioIface = 'PCM Playback Volume'
	amixervals = os.popen(AMIXER+" -c 0 cget name='"+audioIface+"'").read()
	#numid=32,iface=MIXER,name='Master Playback Volume'
	# type=INTEGER,access=rw---R--,values=1,min=0,max=64,step=0
	# values=62
	#| dBscale-min=-64.00dB,step=1.00dB,mute=0
	volRegEx = re.compile("numid=.*[\r\n]*.*min=([^,]*),max=([^,]*),step.*[\r\n]*.*values=([0-9]*)[\r\n]*",re.MULTILINE)
	volData = volRegEx.search(amixervals)
	if volData:
		vollimitdown = volData.group(1)
		vollimitup   = volData.group(2)
		volvalue     = int(volData.group(3))
	else:
		print("AMIXER -c 0 cget name='",audioIface,"' didnt return the right results")
		sys.exit()
	volstep = (int(vollimitup) - int(vollimitdown)) / 10
	print("[INIT]: ",AMIXER," limits from ",vollimitdown," to ",vollimitup,". Current volume is ",volvalue,". Volume Step is ",volstep)
	i=0
	tempArray = []
	lightArray = []
	softArray = []
	while 1:
		sensors = ser.readline()
		#Temp: 26.66. Light: 125. softPot: 99
		sensorsRegEx = re.compile("Temp: ([0-9.]*). Light: (\d{0,3}). softPot: (\d{0,3})")
		sensorsLine = sensorsRegEx.search(sensors)
		if i == 10:
			sys.stderr.write("mongodb...\n")
			#client.write_key("light", lightArray)
			#client.write_key("soft", softArray)
			#client.write_key("temp", tempArray)
			curtime = datetime.datetime.now()
			document = {
				"temp" :(sum(tempArray) / float(len(tempArray))),
				"light":(sum(lightArray) / float(len(lightArray))),
				"soft" :(sum(softArray) / float(len(softArray))),
				"ts"   :curtime #Check if this works or add isoformat()
			}
			print str(collection.count()) + " Total Items "
			statDays("temp",tempArray)
			statDays("light",lightArray)
			statDays("soft",softArray)
			collection.insert(document);
			tempArray = []
			lightArray = []
			softArray = []
			i = 0
		i+=1
		if sensorsLine:
			temp  = float(sensorsLine.group(1))
			light = float(sensorsLine.group(2))
			slide = float(sensorsLine.group(3))
			tempArray.append(temp)
			lightArray.append(light)
			softArray.append(slide)
			#sys.stdout.write("[PULL] "+temp+".."+light+".."+slide+" from: "+sensors)
			#print "[PULL] ",float(temp),"..",float(light),"..",float(slide)
			volvalue = int(os.popen(AMIXER+" -c 0 cget name='"+audioIface+"'|grep :|cut -f2 -d=|cut -d, -f1").read())
			if slide > 240:
				if volvalue < vollimitup:
					volvalue+=volstep
					print("[INFO]: Setting Volume + 10 = ",volvalue)
					os.popen(AMIXER+" -q cset name='"+audioIface+"' "+str(volvalue)).read()
			elif slide < 20:
				if volvalue > vollimitdown:
					volvalue-=volstep
					print("[INFO]: Setting Volume - 10 = ",volvalue)
					os.popen(AMIXER+" -q cset name='"+audioIface+"' "+str(volvalue)).read()
