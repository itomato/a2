##This is a preliminary Apple ][ emulator for NeXTstep.

> It is based on a UNIX emulator that runs termcap and
> curses in a Terminal window.  To get it to run, you
> need to (1) get copies of the Apple ][ ROMs.  Since
> the ROMs are copyrighted, they cannot be distributed
> freely.  There is a file inside the "a2" directory
> called "Snarf" that gives one approach to getting
> the ROM images.  Once you have the ROMs, start up a
> Terminal window, 'cd' to the "a2" directory, and
> type "a2".  The Apple ][ text screen (40 colummns)
> appears in the Terminal window and the hires screen
> appears simultaneously in a NeXTstep window.
> 
> This is a *bad* hack, and the emulator will eventually
> become a very nice NeXTstep application.  For more
> information about the Apple ][ emulator project,
> send a message to "na2sig-request@byu.edu" and you
> will be placed on the project mailing list.  Eaves-
> droppers are welcome.
> 
> Credits:  see the various text files inside the
> emulator.  Complete source is included, as well as
> a stripped executable.
> 
> unknown (Mat -- mjhostet@athena.mit.edu?) Mar 20  1992

----

He improved the original emulator several ways.

-   It compiles and runs under Nextstep 3.3
-   It's now quad-fat and will run on Intel and
    Sparc hardware in addition to NeXT hardware.  It
    will also probably run on HP hardware, but I
    can't test that.  There should be no more hardware
    dependencies that aren't handled at compiletime.
-   It's in color rather than just B&W.
-   It supports hires page #2 as well as #1.
-   The command-line options to a2 now work.
-   The escape character now works and is really set to ~
-   Decimal mode arithemetic is now implemented.  (Sabotage works!)
-   Compile options for Intel Joystick support, non-buffered keyboard
    support, color/B&W support, and text page #2 support.

There are several compile options now available.  Change them in the
Makefile.preamble file.

-   There is minimal joystick support if you compile with -DJOYSTICK_SUPPORT.
    This also requires the use of the Intel joystick device driver.
    Use the "recalibrate" command to calibrate the joystick.
-   There is a compile-time option to determine whether keyboard buffering
    is enabled.  Buffering is nice, but some apple II games don't work
    if buffering is enabled.  Use -DKEYBOARD_BUFFER to turn it back on.
-   There's a compile-time option to select color or greyscale graphics.
    Greyscale works much faster, but color looks better on systems that
    support color.  Use either -DUSE_COLOR=1  or -DUSE_GREYSCALE=1
-   Text page #2 might work if you compile with -DTEXT2
    
###usage:
    a2
        -m      Load MONITOR.ROM instead of AUTOSTART.ROM at $F800
        -i      Load INTEGER.ROM instead of APPLESOFT.ROM at $D000
        -c      Enter command mode before executing any instructions
        -d <FILE>       Insert <FILE> as disk 1 on startup

The default disk is Samp.disk.

Note that you still need to supply your own ROMs.
I installed my ROMs and disks in ~/Library so I could easily
upgrade and/or switch between emulators.  This should also
make it obvious for users what ROMs are needed as well as
what they are named (It wasn't obvious in the original
documentation).

For the record,  you need:

- APPLESOFT.ROM

- AUTOSTART.ROM

- DISK.PROM


*From a2 as posted up by:*

> Mike Kienenberger
> 
> mkienenb@alaska.net
> 
> Nov 3, 1997