# linux_tools

Here is me trying to reproduce some common linux programs. Most of them will be written in C to more easily do things that the UNIX implementation is likely to be doing as well.

Most of these implementations do not have functionality as extensive as that of their unix counterparts, but they should be at least sufficient to get the basic job done.

Implemented:
* ls [-l] filename
* stat filename
* tail [-n5] filename
* find filename `(this doesn't work like the real unix find, it does recursively print out filenames though, hence it is once step away from finding a file based on a given input pattern)`

