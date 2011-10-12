/*
	TOKEN_COLOURS.H
	---------------
*/
#ifndef TOKEN_COLOURS_H_
#define TOKEN_COLOURS_H_

class ANT_rgb;

/*
	class ANT_TOKEN_COLOURS
	-----------------------
*/
class ANT_token_colours
{
public:
	ANT_token_colours() {}
	virtual ~ANT_token_colours() {}

	ANT_rgb *colour(long long token_type);
} ;

#endif /* TOKEN_COLOURS_H_ */
