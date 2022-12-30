Shutdownwatcher-

    The utility itself is a simple C++ program. It's sole task is to intialize
a pre-selected GPIO pin for input. The internal pull-up resistor is enabled,
so the pin is normally high. When the pin is brought to ground, the program
reacts by issuing the system command 'shutdown -h now' to shut down the
machine.

    The program is normally run through the systemd service manager. 'ShutdownWatcher.service' is the control file.
    
    A Man page is also provided.
    
    The 'install' program should be run as root, and does the following:
    (1) Copy the man page to the /usr/share/man/man1 directory, and cause
        the man page database to be updated
    (2) Copy the service control file to /etc/systemd/system.
    (3) Copy the program to /usr/local/bin.

