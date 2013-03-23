#!/usr/bin/python -u

import pyinotify
import re
import time
import os
import sys

class ardPullerHandler(pyinotify.ProcessEvent):
    #ardVendor = "2341"
    #ardProdID = "0001"
    #ardLocation = `udevadm info -a -p \$(udevadm info -q path -n $ardDev)`
    #lsusb maybe
    def process_IN_CREATE(self, event):
	#print "Just got: ",event.pathname
	m = re.search("/dev/ttyACM",event.pathname)
	if m:
		acms = re.match("/dev/(ttyACM[0-9]+)$",event.pathname)
        	sys.stderr.write("Arduino plugged:"+event.pathname+" -> "+acms.group(1)+"\n")
		os.system("sleep 1;systemctl start ardpuller.service")

    def process_IN_DELETE(self, event):
	m = re.search("/dev/ttyACM",event.pathname)
	if m:
		acms = re.match("/dev/(ttyACM[0-9]+)$",event.pathname)
        	sys.stderr.write("Arduino unplugged:"+event.pathname+" -> "+acms.group(1)+"\n")
		os.system("sleep 1;systemctl stop ardpuller.service")

def main():
    # watch manager
    ardChecker = pyinotify.WatchManager()
    ardChecker.add_watch('/dev/', pyinotify.IN_DELETE|pyinotify.IN_CREATE, rec=True)

    # event handler
    handler = ardPullerHandler()

    # notifier
    notifier = pyinotify.Notifier(ardChecker, handler)
    notifier.loop()

if __name__ == '__main__':
    main()
