This is a random dungeon generator C program for ToME-SX, written by Amy.

Usage:
1. Run makedungeons.bat
2. Copy the d_info.txt file it creates over the existing d_info.txt file in lib/edit
3. Run ToME-SX

In order to revert back to the "standard" dungeon definition file:
1. Delete any d_info.txt file created by the "makedungeons" program
2. Copy the "orig d_info.txt" file to "d_info.txt"
3. Copy that file to the lib/edit folder

*Make sure you don't accidentally delete the "orig d_info.txt" file; it contains the original dungeon definitions!*

The file tomedungeons.c contains the source code of this utility.
The tomedung.exe is the compiled binary of this utility.

This utility has been tested on Windows XP. It should run on most other Windows OSes too.

Disclaimer: You may use this utility for whatever purpose you can think of and also modify it, but Amy takes no responsibility for damage caused by running the utility. If you make changes to this program and then use it for your own game, it'd be nice if you gave credit.
-- Amy Bluescreenofdeath, June 2024