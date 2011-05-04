/*
	LZOTEST.C
	---------
	Determine the file format of an LZO file
*/
#include <stdio.h>
#include <string.h>
#include "lzoconf.h"
#include "lzo1x.h"

#define F_ADLER32_D     0x00000001L
#define F_ADLER32_C     0x00000002L
#define F_H_EXTRA_FIELD 0x00000040L
#define F_CRC32_D       0x00000100L
#define F_CRC32_C       0x00000200L
#define F_MULTIPART     0x00000400L
#define F_H_FILTER      0x00000800L

enum
{
    M_LZO1X_1     =     1,
    M_LZO1X_1_15  =     2,
    M_LZO1X_999   =     3,
    M_NRV1A       =  0x1a,
    M_NRV1B       =  0x1b,
    M_NRV2A       =  0x2a,
    M_NRV2B       =  0x2b,
    M_NRV2D       =  0x2d,
    M_ZLIB        =   128
} ;

static const unsigned char lzop_header[] = {0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a};

/*
	SHORTIZE()
	----------
	Convert a buffer into a short
*/
unsigned short shortize(unsigned char *source)
{
return ((unsigned short)source[0] << 8) | (unsigned short)source[1];
}

/*
	LONGIZE()
	---------
*/
unsigned long longize(unsigned char *source)
{
return ((unsigned long)source[0] << 24) | ((unsigned long)source[1] << 16) + ((unsigned long)source[2] << 8) + (unsigned long)source[3];
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static unsigned char buffer[1024];
FILE *fp;
long current;
unsigned short two_byte, version;
unsigned char one_byte;
unsigned long flags;
unsigned long number_of_extra_bytes, compressed_size, uncompressed_size;
unsigned char *encoded, *decoded;
long got;
long long total_size;

if ((fp = fopen(argv[1], "rb")) == NULL)
	return printf("Cannot open file:%s\n", argv[1]);

if (lzo_init() != LZO_E_OK)
	return printf("LZO initialisation failure\n");
/*
	9 bytes LZO Header
*/
fread(buffer, sizeof(lzop_header), 1, fp);
printf("HEADER:");
for (current = 0; current < sizeof(lzop_header); current++)
	printf("%02X ", buffer[current]);
if (memcmp(buffer, lzop_header, sizeof(lzop_header)) == 0)
	printf(" (Correct)\n");
else
	return printf(" (Wrong)\n");

/*
	2 bytes version number (wrong byte order)
	2 bytes lib version number (wrong byte order)
*/
fread(&buffer, sizeof(two_byte), 1, fp);
version = shortize(buffer);
printf("Version:%04X\n", version);
fread(&buffer, sizeof(two_byte), 1, fp);
printf("Library Version:%04X\n", shortize(buffer));

/*
	optionally
		2 bytes version needed to extract
*/
if (version >= 0x0940)
	{
	fread(&buffer, sizeof(two_byte), 1, fp);
	printf("Version needed to extract:%04x\n", shortize(buffer));
	}
/*
	1 byte method
*/
fread(&buffer, 1, 1, fp);
printf("Method:%02X ", *buffer);
switch (*buffer)
	{
	case M_LZO1X_1:
		puts("LZO1X-1");
		break;
	case M_LZO1X_1_15:
		puts("LZO1X-1(15)");
		break;
	case M_LZO1X_999:
		puts("LZO1X-999");
		break;
	case M_NRV1A:
	case M_NRV1B:
	case M_NRV2A:
	case M_NRV2B:
	case M_NRV2D:
		puts("NRV compression (not supported)");
		break;
	case M_ZLIB:
		puts("ZLIB in LZO not supported\n");
		break;
	default:
		puts("Unknown");
		break;
	}

/*
	optionaly
		1 byte compression level
*/
fread(&buffer, 1, 1, fp);
printf("Level:%02X\n", *buffer);

/*
	4 byte flags
*/
fread(&buffer, 4, 1, fp);
flags = longize(buffer);
printf("Flags:%08X\n", flags);
if (flags & F_MULTIPART)
	printf("Multi-Part archive (not supported)\n");
/*
	optionally
		4 bytes filter
*/
if (flags & F_H_FILTER)
	{
	fread(&buffer, 4, 1, fp);
	printf("Filter:%08X\n", longize(buffer));
	}

/*
	4 byte mode
*/
fread(&buffer, 4, 1, fp);
printf("Mode:%08X\n", longize(buffer));

/*
	4 byte Time
*/
fread(&buffer, 4, 1, fp);
printf("Time (low):%08X\n", longize(buffer));

/*
	Optional
		4 more byte Time
*/
if (version >= 0x0940)
	{
	fread(&buffer, 4, 1, fp);
	printf("Time (high):%08X\n", longize(buffer));
	}

/*
	Pascal String
		1 byte length (n)
		n byte string
*/
fread(&buffer, 1, 1, fp);
buffer[*buffer] = '\0';
fread(&buffer, *buffer, 1, fp);
printf("Source Filename:%s\n", buffer);

/*
	4 byte checksum
*/
fread(&buffer, 4, 1, fp);
printf("Checksum:%08X\n", longize(buffer));

/*
	Optional extra field
		4 bytes extrafield length (n)
		n bytes
		4 byte checksum
*/
if (flags & F_H_EXTRA_FIELD)
	{
	fread(&buffer, 4, 1, fp);
	number_of_extra_bytes = longize(buffer);
	printf("Extra Bytes:%08X\n", number_of_extra_bytes);
	
	for (current = 0; current < number_of_extra_bytes; current++)
		{
		fread(&buffer, 1, 1, fp);
		printf("%02X ", *buffer);
		}
	printf("\n");

	fread(&buffer, 4, 1, fp);
	printf("Extra Bytes Checksum:%08X\n", longize(buffer));
	}

/*
	Decompress Blocks
*/
total_size = 0;
while (1)
	{
	/*
		4 bytes uncompressed size
		4 bytes compressed size
	*/
	fread(&buffer, 4, 1, fp);
	uncompressed_size = longize(buffer);
	printf("Uncompressed Size:%ld\n", uncompressed_size);
	total_size += uncompressed_size;
	if (uncompressed_size == 0)
		break;
	fread(&buffer, 4, 1, fp);
	compressed_size = longize(buffer);
	printf("Compressed Size:%ld\n", compressed_size);

	/*
		Uncompressed Checksum
	*/
	if (flags & F_ADLER32_D)
		{
		fread(&buffer, 4, 1, fp);
		printf("Uncompressed checksum:%08X\n", longize(buffer));
		}
	if (flags & F_CRC32_D)
		{
		fread(&buffer, 4, 1, fp);
		printf("Uncompressed checksum:%08X\n", longize(buffer));
		}

	/*
		Compressed Checksum
	*/
	if (flags & F_ADLER32_C)
		{
		fread(&buffer, 4, 1, fp);
		printf("Compressed checksum:%08X\n", longize(buffer));
		}
	if (flags & F_CRC32_C)
		{
		fread(&buffer, 4, 1, fp);
		printf("Compressed checksum:%08X\n", longize(buffer));
		}

	encoded = new unsigned char [compressed_size];
	decoded = new unsigned char [uncompressed_size];
	fread(encoded, compressed_size, 1, fp);

	got = lzo1x_decompress(encoded, compressed_size, decoded, &uncompressed_size, NULL);

	switch (got)
		{
		case LZO_E_OK:
			puts("SUCCESS");
			break;
		case LZO_E_INPUT_NOT_CONSUMED:
			puts("EARLY FINISH");
			break;
		case LZO_E_INPUT_OVERRUN:
			puts("INPUT OVERRUN");
			break;
		case LZO_E_OUTPUT_OVERRUN:
			puts("OUTPUT OVERRUN");
			break;
		case LZO_E_LOOKBEHIND_OVERRUN:
		case LZO_E_EOF_NOT_FOUND:
		case LZO_E_ERROR:
			puts("CORRPUT INPUT");
			break;
		}

	//puts((const char *)decoded);
	}

printf("Total file size: %lld\n", total_size);
delete [] encoded;
delete [] decoded;

fclose(fp);
return 0;
}

