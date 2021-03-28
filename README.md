Summary
=======
splitmbox is a tool to convert an mbox into individual
files, one per message.

This can be useful if a given program only accepts
maildir formatted emails, but you use a client that
uses mbox.  Examples are:
- training spamassasin via sa-learn
- importing archives into majordomo or mailman

The directory is named 'splitmail' since it's
conceivable that other tools will be written in the
future that accomplish similar tasks.

Installation
============

To install the command and manual page somewhere
convenient, run `make install`; the Makefile defaults
to '/usr/local' but you can change the PREFIX:

```
$ make PREFIX=~ install
```


---

```
NAME
     splitmbox -- split mbox input into individual mail files

SYNOPSIS
     splitmbox [-d dir] [-hv] [file ...]

DESCRIPTION
     The splitmbox utility splits an mbox-file given on the command-line or
     via STDIN into individual mail files.  If no output directory is speci-
     fied, splitmbox will create the individual mail files in the current
     working directory.	 Each resulting file is named NNNNNN, with leading
     zero-padding.

     The following options are available:

     -d dir	 If specified, resulting files will be stored in dir.  If dir
		 does not exist, splitmbox will attempt to create it.

     -h		 Show usage and exit.

     -v		 Show version number and exit.

AUTHORS
     The splitmbox utility was written by Jan Schaumann <jschauma@netmeis-
     ter.org>.

BUGS
     The splitmbox utility can only handle mboxes with lines shorter than 1024
     characters.

     If an mbox is provided with more than 999,999 messages, then the result-
     ing files will not be alphabetically sorted correctly, as filenames for
     the next messages are not zero-padded on the left.
```
