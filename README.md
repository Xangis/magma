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

Source code is in the /src directory. To build on a standard Linux system, run
"make std". To build on Windows, open MagmaMUD.sln with Visual Studio and build.
It's a Visual Studio 8 project, so just let it convert when asked if you have a
newer version (and you probably do).

Magma has been tested with both gcc and clang on Linux. To build with clang,
set your CC environment variable before running make, like so:

export CC=clang

### Running

Run startup.sh or startup.bat in the src directory once the application has been 
built.

## Development Status

Magma is not currently under active development, but I do make changes to it from
time to time to ensure that it can be built on a modern system.

I am willing to accept pull requests if you have improvements to contribute.
