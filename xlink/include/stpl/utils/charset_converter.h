#ifndef CHARSETCONVERTER_
#define CHARSETCONVERTER_

#include <string>
#include <stdexcept>
#include <algorithm>

#include <iconv.h>
#include <errno.h>
#include <string.h>
#include <iconv.h>


namespace stpl
{
	enum {BAD = -1, GOOD = 0};
	
	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class CharsetConverter
	{
		public:			
			CharsetConverter(const StringT &from, const StringT &to) {}
			~CharsetConverter() {}
			
			static int convert(char *fromcode, char *tocode, StringT& input, StringT& output) {
			    iconv_t cd;                     /* conversion descriptor */
			    //char *table;           /* ptr to translation table */
			    int bytesread;                  /* num bytes read into input buffer */
			    char inbuf[BUFSIZ];    /* input buffer */
			    char *inchar;          /* ptr to input character */
			    size_t inbytesleft;                /* num bytes left in input buffer */
			    char outbuf[BUFSIZ];   /* output buffer */
			    char *outchar;         /* ptr to output character */
			    size_t outbytesleft;               /* num bytes left in output buffer */
			    size_t ret_val;                 /* number of conversions */
			    IteratorT begin_pos;
			    IteratorT end_pos;
			    IteratorT cur_pos = input.begin();
			
			    /* Initiate conversion -- get conversion descriptor */
			    if ((cd = iconv_open(tocode, fromcode)) == (iconv_t)-1) {
			        printf("iconv_open false: %s ==> %s\n", fromcode, tocode);
			    }
			
			    inbytesleft = 0;                        /* no. of bytes converted */
			    //begin_pos = input.begin();
			    /* translate the characters */
			    do {
	                /*
	                 * if any bytes are leftover, they will be in the
	                 * beginning of the buffer on the next read().
	                 */
	
	                inchar = inbuf;          /* points to input buffer */
	                outchar = outbuf;        /* points to output buffer */
	                outbytesleft = BUFSIZ;   /* no of bytes to be converted */
	                
	                /*
	                if ((bytesread = read(Input, inbuf+inbytesleft,
	                                      (size_t)BUFSIZ-inbytesleft)) < 0) {
	                        perror("prog");
	                        return BAD;
	                }
	                */
	                
	                if (inbytesleft < input.length())
	                	begin_pos = cur_pos + inbytesleft;
	                else
	                	break;
	                	//begin_pos = input.end();
	                	
	                if (BUFSIZ > input.length()) {
	                	end_pos = cur_pos = input.end();
	                }
	                else {
	                	end_pos = begin_pos + (BUFSIZ-inbytesleft);
	                	cur_pos = end_pos++;
	                }
	                	
	                bytesread = end_pos - begin_pos;
	                inbytesleft = bytesread;
	
	                copy(begin_pos, end_pos, inbuf);
	                
	
	                //cout << "(inbytesleft:" << inbytesleft << ":" << bytesread << ")" << inchar; 
	                ret_val = iconv(cd, &inchar, &inbytesleft,
	                                &outchar, &outbytesleft);
	
					/*
	                if (write(1, outbuf, (size_t)BUFSIZ-outbytesleft) < 0) {
	                        perror("prog");
	                        return BAD;
	                }
	                */
	                //cout << outchar;
	                output.append(outbuf, (size_t)BUFSIZ-outbytesleft);
	                
	                /* iconv() returns the number of non-identical conversions
	                 * performed. If the entire string in the input buffer is
	                 * converted, the value pointed to by inbytesleft will be
	                 * zero. If the conversion stopped due to any reason, the
	                 * value pointed to by inbytesleft will be non-zero and
	                 * errno is set to indicate the condition.
	                 */
	                if (((int)ret_val == -1) && (errno == EINVAL)) {
	                        /* Input conversion stopped due to an incomplete
	                         * character or shift sequence at the end of the
	                         * input buffer.
	                         */
	                        /* Copy data left, to the start of buffer */
	                        memcpy((char *)inbuf, (char *)inchar,
	                               (size_t)inbytesleft);
	                } else if (((int)ret_val == -1) && (errno == EILSEQ)) {
	                        /* Input conversion stopped due to an input byte
	                         * that does not belong to the input codeset.
	                         */
	                        //error(FATAL, BAD_CONVERSION);
	                        //printf("bad conversion. \n");
	                        memcpy((char *)inbuf, (char *)inchar,
	                               (size_t)inbytesleft);	   
	                        return BAD;                     
	                } else if (((int)ret_val == -1) && (errno == E2BIG)) {
	                        /* Input conversion stopped due to lack of space
	                         * in the output buffer. inbytesleft has the
	                         * number of bytes to be converted.
	                         */
	                        memcpy((char *)inbuf, (char *)inchar,
	                               (size_t)inbytesleft);
	                }
	                
	                if (inbytesleft >= input.length()) {
	                        break;           /* end of conversions */
	                }
	                                
	                /* Go back and read from the input file. */
		        } while (cur_pos != input.end());        
		
		        /* end conversion & get rid of the conversion table */
		        if (iconv_close(cd) == BAD) {
		             //   error(FATAL, BAD_CLOSE);
		             printf("error in closing iconv. \n");
		        }
		        return GOOD;
			
			}

	};
}

#endif /*CHARSETCONVERTER_*/
