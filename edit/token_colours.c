/*
	TOKEN_COLOURS.C
	---------------
*/
#include "token_colours.h"
#include "rgb.h"

static ANT_rgb token_set[] = 
{
{0x00, 0x00, 0x00},		// UNKNOWN
{0x00, 0x00, 0x00},		// TAB
{0x00, 0x00, 0x00},		// SPACE
{0x00, 0x00, 0x00},		// IDENTIFIER
{0x80, 0x00, 0x80},		// NUMBER
{0x80, 0x00, 0x80},		// STRING
{0x80, 0x00, 0x80},		// CHARACTER
{0x00, 0x00, 0x80},		// RESERVED_WORDS
{0x00, 0x80, 0x00},		// LINE_COMMENT
{0x00, 0x80, 0x00},		// OPEN_BLOCK_COMMENT
{0x00, 0x80, 0x00},		// BLOCK_COMMENT
{0x00, 0x80, 0x00},		// CLOSE_BLOCK_COMMENT
{0x80, 0x00, 0x00},		// PREPROCESSOR_DIRECTIVE
{0xFF, 0x00, 0x00},		// KNOWN_ERROR
} ;

static long long size_of_colour_set = sizeof(token_set) / sizeof(*token_set);

/*
	ANT_TOKEN_COLOURS::COLOUR()
	---------------------------
*/
ANT_rgb *ANT_token_colours::colour(long long token_type)
{

if (token_type >= size_of_colour_set)
	token_type = 0;								// overflow -> UNKNOWN

return token_set + token_type;
}
