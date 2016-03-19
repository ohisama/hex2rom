#define VER "0.1"
#include <stdio.h>
#include <string.h>
#define BUFLEN 200
#define PROGNAME "hex2rom.exe"
#ifndef HIBYTE
#define HIBYTE(x) (((x)>>8)&0xff)
#endif
#ifndef LOBYTE
#define LOBYTE(x) ((x)&0xff)
#endif
enum REC_TYPE
{
	REC_DATA, REC_END
};
int atohex (char * s);
int main (int argc, char * argv[])
{
	FILE * in, * out;
	char inname[5], outname[5];
	char * p;
	unsigned int linecnt, bytecount, address, type, i, j;
	unsigned char rom[8192] = {0x41, 0x42, 0x10, 0x40, 0, 0, 0, 0};
	unsigned char data[BUFLEN];
	unsigned char sum, checksum;
	int is_filter;
	char buf[BUFLEN];
	char tmp[12];
	int is_first;
	is_filter = 0;
	if (argc == 1)
	{
		in = stdin;
		out = stdout;
		is_filter = 1;
	}
	else
	{
		in = NULL;
		out = NULL;
		for (i = 1; i < argc; i++)
		{
			if (* argv[i] == '-')
			{
				argv[i] ++;
				if (* argv[i] == '-')
				argv[i] ++;
				switch (* argv[i])
				{
				case 'v':
					printf("\n%s V%s, %s\n\n", PROGNAME, VER, __DATE__);
					return 0;
				case '\0':
					if (in == NULL)
					{
						in = stdin;
						is_filter = 1;
						break;
					}
					else if (out == NULL)
					{
						out = stdout;
						is_filter = 1;
						break;
					}
				case 'h':
				default:
					fprintf(stderr, "\nusage: %s infile[.hex] [outfile[.rom]]\n", PROGNAME);
					return 0;
				}
			}
			if (in == NULL)
			{
				strcpy(inname, argv[i]);
				p = strrchr(inname, '.');
				if (p == NULL) strcat(inname, ".hex");
				in = fopen(inname, "rt");
				if (in == NULL)
				{
					perror(inname);
					return 1;
				}
				continue;
			}
			else if (out == NULL)
			{
				strcpy(outname, argv[i]);
				if (outname[0] == '\0')  strcpy(outname, "$$file$$");
				p = strrchr(outname, '.');
				if (p == NULL) strcat(outname, ".rom");
				out = fopen(outname, "wb");
				if (out == NULL)
				{
					perror (outname);
					return 1;
				}
			}
		}
	}
	if (!is_filter)  printf("\n%s V%s, %s\n", PROGNAME, VER, __DATE__);
	if (out == NULL)
	{
		strcpy(outname, inname);
		p = strrchr (outname, '.');
		if (p)   * p = '\0';
		strcat(outname, ".rom");
		out = fopen(outname, "wb");
		if (out == NULL)
		{
			fclose(in);
			perror(outname);
			return 1;
		}
	}
	if (!is_filter) printf("\nProcessing file \"%s\" ...	\n", inname);
	linecnt = 0;
	is_first = 1;
	while (fgets(buf, BUFLEN, in))
	{
		linecnt ++;
		if (!is_filter)  printf("%4d\n", linecnt);
		tmp[0] = buf[1];
		tmp[1] = buf[2];
		tmp[2] = '\0';
		bytecount = atohex(tmp);
		tmp[0] = buf[3];
		tmp[1] = buf[4];
		tmp[2] = buf[5];
		tmp[3] = buf[6];
		tmp[4] = '\0';
		address = atohex(tmp);
		tmp[0] = buf[7];
		tmp[1] = buf[8];
		tmp[2] = '\0';
		type = atohex(tmp);
		printf(": 0x%X 0x%X 0x%X \n", bytecount, address, type);
		sum = bytecount + HIBYTE(address) + LOBYTE(address) + type;
		for (i = 0; i <= bytecount; i++)
		{
			j = i * 2 + 9;
			tmp[0] = buf[j];
			j ++;
			tmp[1] = buf[j];
			tmp[2] = '\0';
			if (i < bytecount)
			{
				data[i] = atohex(tmp);
				rom[address - 0x4000 + i] = data[i];
				sum += data[i];
			}
			else
			{
				checksum = atohex(tmp);
				sum = (-sum) & 0xff;
			}
		}
		if (sum != checksum)
		{
			fprintf(stderr,"\nError in line %d: checksum failure (%2.2x/%2.2x)\n", linecnt, (unsigned char) sum, (unsigned char) checksum);
		}
		//if (type != REC_END)	fwrite (data, sizeof (*data), BUFLEN, out);
	}
	if (!is_filter)
	{
		fwrite (rom, sizeof(* rom), 8192, out);
		printf ("%4d lines done.\n", linecnt);
		fclose (in);
		fclose (out);
	}
	return 0;
}
int atohex (char * s)
{
	int i, digit, number;
	i = 0;
	number = 0;
	while (s[i])
	{
		switch (toupper(s[i]))
		{
		case '0':
			digit =  0;
		break;
		case '1':
			digit =  1;
		break;
		case '2':
			digit =  2;
		break;
		case '3':
			digit =  3;
		break;
		case '4':
			digit =  4;
		break;
		case '5':
			digit =  5;
		break;
		case '6':
			digit =  6;
		break;
		case '7':
			digit =  7;
		break;
		case '8':
			digit =  8;
		break;
		case '9':
			digit =  9;
		break;
		case 'A':
			digit = 10;
		break;
		case 'B':
			digit = 11;
		break;
		case 'C':
			digit = 12;
		break;
		case 'D':
			digit = 13;
		break;
		case 'E':
			digit = 14;
		break;
		case 'F':
			digit = 15;
		break;
		}
		number = (number << 4) + digit;
		i++;
	}
	/* printf("Number = %i, 0x%X\n", number, number);*/
	return (number);
}
