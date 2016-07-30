
/*
 *  Turn an Apple II monitor ROM hex dump back into binary data
 *	usage:  hex < hex_data > bin_data
 */


#include	<stdio.h>

main() {
char buf[100];
int i, j;
int addr;
int m[8];
unsigned char c;

	while (fgets(buf, 100, stdin) != NULL) {
		i = sscanf(buf, "%x- %x %x %x %x %x %x %x %x",
			&addr, &m[0], &m[1], &m[2], &m[3], &m[4], &m[5],
						&m[6], &m[7]);

		for (j = 1; j < i; j++) {
			c = m[j - 1];
			write(1, &c, 1);
		}
	}
}

