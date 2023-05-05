# Shutdown watcher
## General
This does an auto-shutdown if a button is pressed.
it USES the wiringpi (see _required_ below) library Interrupt functions.

## Notes:
TEMP and voltage MONITOR: vcgencmd.
git clone https://github.com/raspberrypi/userland.git	
cd userland
./buildme --aarch64

## SWITCH:
SWITCH connection:  Normally open, ground thru a 1k resistor. The processor
is programmed with an internal pull up resistor.
The first argument is the switch's GPIO number. Default is 18.

## Logic/operation:
  Sets the SWITCH pin to input, pulled high with internal pull-up resistor.
  It watches for the SWITCH pin going low, sampling the pin once per second.
  IF it goes low, we invoke 'shutdown -h now' to shut down the processor.
  
## Install notes:
  This program should be run as a daemon at startup (use  /etc/rc.local?)
  Add it to the end of /etc/rc.local just before the 'exit 0':
  '/usr/local/bin/ShutdownWatcher&'
  
## Required:
   cmake (apt-get install cmake)
   cmake-curses-gui (apt-get install cmake-curses-gui)
   
   Download WiringPi and install it:

    (1) wget https://project-downloads.drogon.net/wiringpi-latest.deb
    (2) dpkg -i wiringpi-latest.deb

    To compile:
    (1) create (or cd into) a 'build' directory
    (2) run 'cmake ..'
    (3) run 'make'
    (4) as root run 'make install'

## Installation:
 An install program is included. This must be run as root, in the main directory.
 
 This copies the binary to /usr/local/bin, A Man page to /usr/share/man/man1, and the
 'shutdownwatcher.99' script to /etc/init.d as 'shutdownwatcher'.  update-rc.d is called to set up the service.
 It will be started automatically at run level 3, 4, or 5.
 
## UNINSTALL:
An uninstall program is included, which unconfigures and removes all files installed by the install script.
