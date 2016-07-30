
#include	<stdio.h>


/*
 *  Map a disk image in Prodos block ordering to DOS 3.3 block ordering
 *	usage:  mapper < old_image > new_image
 */

main() {
unsigned char buf[4096];
int track;

	for (track = 0; track < 35; track++) {
		if (read(0, buf, 4096) != 4096) {
			perror("bad read");
			exit(1);
		}

		write(1, buf, 256);
		write(1, &buf[0xE00], 256);
		write(1, &buf[0xD00], 256);
		write(1, &buf[0xC00], 256);
		write(1, &buf[0xB00], 256);
		write(1, &buf[0xA00], 256);
		write(1, &buf[0x900], 256);
		write(1, &buf[0x800], 256);
		write(1, &buf[0x700], 256);
		write(1, &buf[0x600], 256);
		write(1, &buf[0x500], 256);
		write(1, &buf[0x400], 256);
		write(1, &buf[0x300], 256);
		write(1, &buf[0x200], 256);
		write(1, &buf[0x100], 256);
		write(1, &buf[0xF00], 256);
	}
}


