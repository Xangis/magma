Magma MUD version 3.04

https://github.com/Xangis/magma

=== The player directory

Player files are ASCII text files organized in separate directories.
The average player file is 10KB long.  You may compress the playerfiles
using 'gzip -9 -r *' in the player directory periodically.  The server
will uncompress as needed and leave them untouched afterwards until
you compress them manually again.

Player files are saved in subdirectories based on the first initial of
the character.

The default administrator (immortal) character has a login name of
"Superuser" and a password of "password".  You will, of course, want to
change the password (and possibly the name).

Note that passwords are stored in PLAIN TEXT, which is nice if you want
to change someone's password using a text editor, but terrible for
security.
