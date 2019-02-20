# Magma MUD

Magma MUD - An old-school MUD codebase derived from DIKU, Merc, Envy, and
UltraEnvy that runs on Windows and Linux.

This codebase was used as the basis for Basternae II: Phoenix Rising MUD, which had
a good vs. evil races PvP theme. Unlike its ancestors, Magma allows players to
attack each other freely and has a system for tallying "frags", which are the
net number of kills of the enemy side minus the number of times you've died in
combat against the other side.

There was also a Basternae 3 MUD project. It did not use this code, but rather a
new codebase written using C# .NET. That codebase has been open-sourced as the
ModernMUD codebase, available at https://github.com/xangis/ModernMUD and zones from
magma can be converted to run with it using the ModernMUDConverter which is
available at https://github.com/Xangis/ModernMUDConverter

It is written in C and runs on Windows or Linux.  Check the files in 
/doc/Licenses for the various licenses that apply (there are a bunch -- this 
has inherited from a long branch of the DIKU MUD family tree).

Documentation is in the /doc directory. Start with README.TXT and go from there.
As a quick start to building and running:

### Building

This project has recently switched to CMake. If you have cmake installed, run:

cmake .

make

sudo make install

We no longer use makefiles to build. See version 3.05 or earlier for makefiles.

### Running

Run startup.sh or startup.bat in the src directory once the application has been 
built. Or, on Linux, just run "magma" from the command line.

By default the MUD runs on port 4001. You can connect to that port via telnet in
order to play the game.

The default admin login is Superuser with a password of password. You'll want to
change that.

## Development Status

Magma is not currently under active development, but I do make changes to it from
time to time to ensure that it can be built on a modern system.

Earliest versions ran on Red Hat Linux in 2000, some version before 7.0. The newest
Linux distribution this has been tested with is Ubuntu 18.04.

I am willing to accept pull requests if you have improvements to contribute.
