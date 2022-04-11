#include <stdio.h>
#include <stdlib.h>

struct binary_item {
	unsigned long xy;// : 22;
	unsigned long color;// : 24;
	//unsigned int color : 4;
	//unsigned long who : 32;
	float tdiff;
};

struct csv_row {
	char utc [30];
	char user [90];
	unsigned long color;
	unsigned int x, y;
};

int main (int argc, char *argv []) {
	if (argc != 3) {
		fprintf (stderr, "USAGE: %s INFILE OUTFILE\n", argv [0]);
		exit (1);
	}
	FILE *csv = fopen (argv [1], "r");
	FILE *outfile = fopen (argv [2], "w");
	
	double last = 0;
	unsigned long long line = 0;
	for (;; line ++) {
		struct csv_row row;
		struct binary_item bin;
		
		if (fscanf (csv, "%[^,],%[^,],#%lx,\"%u,%u\"\n", row.utc, row.user, &row.color, &row.x, &row.y) == EOF)
			break;
		
		// parse UTC timestamp
		{	unsigned short day, hour, minute;
			float second;
			sscanf (row.utc, "2022-04-%hu %hu:%hu:%f UTC", &day, &hour, &minute, &second);
			double now = last -
				(
					( (
						( (
							long (day * 24)
						+ hour) * 60)
					+ minute) * 60)
				+ second);
			
			if (last == 0) {
				now = -now;
				last = now;
				bin.tdiff = 0;
			} else {
				bin.tdiff = now - last;
				last = now;
			}
		}
		
		// TODO: user id
		
		// color
		bin.color = row.color;
		/*switch (row.color) {
			case 0xFFFFFF: bin.color = 0; break;
			case 0xE4E4E4: bin.color = 1; break;
			case 0x888888: bin.color = 2; break;
			case 0x222222: bin.color = 3; break;
			case 0xFFA7D1: bin.color = 4; break;
			case 0xE50000: bin.color = 5; break;
			case 0xE59500: bin.color = 6; break;
			case 0xA06A42: bin.color = 7; break;
			case 0xE5D900: bin.color = 8; break;
			case 0x94E044: bin.color = 9; break;
			case 0x02BE01: bin.color = 10; break;
			case 0x00E5F0: bin.color = 11; break;
			case 0x0083C7: bin.color = 12; break;
			case 0x0000EA: bin.color = 13; break;
			case 0xE04AFF: bin.color = 14; break;
			case 0x820080: bin.color = 15; break;
			default: exit (10);
		}*/
		
		// x, y -> xy
		bin.xy = (row.y * 2000) + row.x;
		
		fwrite (&bin, sizeof (bin), 1, outfile);
	}
	printf ("%llu lines read.\n", line);
}
