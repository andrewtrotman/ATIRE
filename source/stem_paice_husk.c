/*
	STEM_PAICE_HUSK.C
	-----------------
	Stemmer Program 1994-5,by Andrew Stark Started 12-9-94.
	Taken and adapted for use with ANT by Andrew Trotman (2009)
	The license is not at all clear (see below), but it is assumed to be BSD-like

	When I got this code it was in C and read the rule set from an input file
	as ANT does not have config files the rules are now inlined.  The "right"
	way to do this is to embed the right structures, but the easy way is to
	just embed the contenets of the file and hack the code that reads the
	file.  I took the easy way as this code is unlikely to ever get used
	except as a baseline.

	NOTE: This code leaks on deletion

	According to the Lancaster Stemmer Website:
	http://www.comp.lancs.ac.uk/computing/research/stemming/
	 
		This is the official web-site for the Lancaster (Paice/Husk) stemming
		algorithm, and provides various implementations of the algorithm,
		together with links to other useful stemming resources.

		If you make any use of resources from this website, we would be
		interested to hear about it. In particular, please do us the courtesy of
		acknowledging the source in any resulting publications. If you want to
		have any comments or questions, please e-mail Chris Paice at
		cdp@comp.lancs.ac.uk

		DISCLAIMER:
		The software and other resources available from this Website are
		provided in good faith, and are believed to be correct and reliable.
		However, the authors accept no liability for any loss or damage which
		may be caused by the use or misuse of this material.

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stem_paice_husk.h"

#define maxlinelength 80 		/* Maximun length of line read from file */
#define stop 2 				/* Stop here */
#define dont_stop 1 			/* Continue with next rule */
#define notapply 0 			/* Rule did not apply */
#define notintact 3 			/* Word no longer intact */
#define blank '?' 				/* For when replace string is empty */

/*
	ANT_STEM_PAICE_HUSK::ANT_STEM_PAICE_HUSK()
	------------------------------------------
*/
ANT_stem_paice_husk::ANT_stem_paice_husk(void)
{
inittable(ttable);
readrules(ttable); /* Read in a rule set from the file */
}

/*
	ANT_STEM_PAICE_HUSK::TBLINDEX()
	-------------------------------
*/
int ANT_stem_paice_husk::tblindex(char *s)
{
int x;

x = 0;
while (*(s + x) != '\0')
	x++;	 /* Read to end of string */

x--; /* Go back one letter to be at end of word */

return (*(s + x) - 'a'); /* Return number from 0..25 */
}

/*
	ANT_STEM_PAICE_HUSK::ISVALIDSTR()
	---------------------------------
*/
int ANT_stem_paice_husk::isvalidstr(char *s)
{
int x;

for(x = 0; *(s + x) != '\0'; x++) /* For each letter in the word... */
	if(!islower(*(s + x)) && *(s + x) != '\'')/* If it's not lower case or an apostrophe.. */
		return 0; /* ..then it`s an error.. */
return 1; /* ..otherwise,it`s ok */
}

/*
	ANT_STEM_PAICE_HUSK::ADDRULE()
	------------------------------
*/
char ANT_stem_paice_husk::addrule(ANT_paice_husk_rule r,ANT_paice_husk_rule t[26])
{
int x;
ANT_paice_husk_rule *temp;
ANT_paice_husk_rule *trail;

x = tblindex(r.keystr); /* Find out where in table to put it */

trail = (t + x); /* Set trail pointer to address of list header */

/* Walk along linked list with this loop */
for(temp = (t + x)->next; temp != NULL; temp = temp->next)
	trail = temp;

/* Make a new instance of rule.. */
trail->next = (ANT_paice_husk_rule *)malloc(sizeof(ANT_paice_husk_rule));

memcpy(trail->next,&r,sizeof(ANT_paice_husk_rule)); /* ..copy r into it.. */
trail->next->next = NULL; /* ..and set its "next" pointer to null */
return 1;
}

/*
	ANT_STEM_PAICE_HUSK::APPLYRULE()
	--------------------------------
*/
int ANT_stem_paice_husk::applyrule(ANT_paice_husk_rule *r,char *word,int isintact)
{
/* Apply the rule r to word,leaving results in word.Return stop,continue */
/* or notapply as appropriate */
int x;

if (!strcmp(r->text, "dummy")) /* If it's just a dummy list header.. */ 
	return notapply; /* ..then it automatically doesn't apply */

if (!isintact && r->intact) /* If it should be intact,but isn't.. */
	return notapply; /* ..then rule fails */

x = (int)(strlen(word) - r->deltotal); /* Find where suffix should start */

if (!strcmp(word + x, r->keystr)) /* If ending matches key string.. */
	{
	if (!r->protect) /* ..then if not protected.. */
		strcpy(word + x, r->repstr); /* ..then swap it for rep string.. */
	else
		return stop; /* If it is protected,then stop */

	if( r->cont) /* If continue flag is set,return cont */
		return dont_stop;
	else
		return stop;
	}
else
	return notapply; /* ..otherwise,this rule is not applicable */
}

/*
	ANT_STEM_PAICE_HUSK::RULEWALK()
	-------------------------------
*/
int ANT_stem_paice_husk::rulewalk(char *word,ANT_paice_husk_rule t[26],int isintact,ANT_paice_husk_rule *used)
{
int x;
int result;
ANT_paice_husk_rule *rp;
char tempwd[maxwdsz];

x = tblindex(word); /* Find out which list to walk along */
strcpy(tempwd, word); /* Copy word for safe keeping */

/* For each rule in list.. */
for(rp = (t + x)->next; rp != NULL; rp = rp->next)
	{
	strcpy(tempwd, word);    /* Copy word for safe keeping */

	/* If rule applied to this word... */
	if((result = applyrule(rp, tempwd, isintact)) != notapply)
		{
		strcpy(word, tempwd);
		memcpy(used, rp, sizeof(ANT_paice_husk_rule));
		return result;
		}
	}
return stop; /* If no rule was used,then we can stop */
}

/*
	ANT_STEM_PAICE_HUSK::MAKERULE()
	-------------------------------
	Warning!In this form, makerule will fail (crash) if rule format is
	not exactly right!

	Format is: keystr,repstr,flags where keystr and repstr are strings,and
	flags is one of:"protect","intact","continue","protint","contint",or
	"stop" (without the inverted commas in the actual file).
*/
ANT_paice_husk_rule ANT_stem_paice_husk::makerule(char *s,int line)
{
ANT_paice_husk_rule temp;

char *tempkey;
char *temprep;
char *tempflags;

int error = 0;

tempkey = strtok(s, ","); /* The unusual(but very useful) strtok */
temprep = strtok(NULL, ","); /* function splits the line into fields */ 
tempflags = strtok(NULL, "\t"); /* delimited by commas (or tab at the end)*/

/* Handle possible errors in... */
if(!isvalidstr(tempkey)) /* ..key string.. */
	{
	printf("Invalid key string:line %d\n",line);
	error = 1;
	}
if(!isvalidstr(temprep) &&  strcmp(temprep,"?")) /* ..replace string.. */
	{
	printf("Invalid replace string:line %d\n",line);
	error = 1;
	}
if(flagerror(tempflags)) /* ..or flag field */
	{
	printf("Invalid flag:line %d\n",line);
	error = 1;
	}
if(error) /* If there was an error then don't compile this rule */
	{
	*temp.keystr = '?'; /* Error signal */
	return temp;
	}

/* If there's no replace string, then put a null char instead */ 
*(temprep) = (*(temprep) == blank) ? '\0' : *(temprep); 

strcpy(temp.keystr, tempkey); /* Copy key string into the rule struct */
strcpy(temp.repstr, temprep); /* Copy replace string to same place */

if (!strcmp(tempflags, "protect")) /* If flag field = "protect"... */
	{
	temp.cont = false; /* ..set continue to false.. */
	temp.protect = true; /* ..set protect to true */
	temp.intact = false; /* Guess what? */
	}

if(! strcmp(tempflags, "intact"))
	{
	temp.cont = false;
	temp.protect = false;
	temp.intact = true;
	}

if(! strcmp(tempflags, "continue"))
	{
	temp.cont = true;
	temp.protect = false;
	temp.intact = false;
	}

if(! strcmp(tempflags, "contint"))
	{
	temp.cont = true;
	temp.intact = true;
	temp.protect = false;
	}

if(! strcmp(tempflags, "protint"))
	{
	temp.cont = false;
	temp.protect = true;
	temp.intact = true;
	}

if(! strcmp(tempflags, "stop"))
	{
	temp.cont = false;
	temp.protect = false;
	temp.intact = false;
	}

temp.deltotal = (char)strlen(tempkey); /* Delete total = length of key string */
temp.rulenum = line; /* Line number of rule in file */

return temp;
}

/*
	ANT_STEM_PAICE_HUSK::READRULES()
	--------------------------------
*/
void ANT_stem_paice_husk::readrules(ANT_paice_husk_rule ttable[26])
{
char copy[maxlinelength];
const char **line;
int n = 0;
ANT_paice_husk_rule temp;

for (line = ANT_paice_husk_rule_set; *line != NULL; line++)
	{
	strcpy(copy, *line);
	n += 1; /* Increment line counter */
	temp = makerule(copy, n); /* Make line into a rule */
//	printf("Line: %d Key: %s Rep: %s\n",n,temp.keystr,temp.repstr); 
	addrule(temp, ttable); /* Add rule to the table */
	}
} 

/*
	ANT_STEM_PAICE_HUSK::FLAGERROR()
	--------------------------------
*/
int ANT_stem_paice_husk::flagerror(char *s)
{
return (strcmp(s, "continue") && /* If s is not equal to "continue".. */
	strcmp(s, "intact") && /* ..or "intact".. */
	strcmp(s, "stop") && /* ..or "stop",etc... */
	strcmp(s, "protect") &&
	strcmp(s, "protint") &&
	strcmp(s, "contint"));
}

/*
	ANT_STEM_PAICE_HUSK::RAW_STEM()
	-------------------------------
	Stem the word,using the given table
*/
ANT_paice_husk_rule ANT_stem_paice_husk::raw_stem(char *s)
{
int isintact = 1;
int result;
char tx[maxwdsz];
char trail[maxwdsz];
ANT_paice_husk_rule r;

r.rulenum = 0; /* Initialise rulenum to 0,in case no rule is used */

/* If s is already an invalid stem before we even start.. */
if(!acceptable(s))
	{
	strcpy(r.text, s); /* ..then just return an unaltered s.. */
	return r;
	}

strcpy(tx, s); /* Make a copy of s,so we don't inadvertently alter it*/
strcpy(trail, s);

/* While there is still stemming to be done.. */
while((result = rulewalk(tx, ttable, isintact, &r)) != stop)
	{
	isintact = false; /* Because word is no longer intact */

	if(!acceptable(tx)) /* Exit from loop if not acceptable stem */
		break;
	strcpy(trail, tx); /* Set up trail for next iteration */
	}

/* Package stemmer output along with rule */
strcpy(r.text, (result == stop) ? tx : trail);

/* -------Remove apostrophe if it exists ---------------- */
r.text[strcspn(r.text,"\'")] = '\0';
        
return r; /* Return the rule,stemmed */
} 

/*
	ANT_STEM_PAICE_HUSK::STEM()
	---------------------------
*/
size_t ANT_stem_paice_husk::stem(const char *term, char *destination)
{
if (strlen(term) < maxwdsz)
	{
	/*
		We're smaller than the internal buffers so we can stem
	*/
	ANT_paice_husk_rule trule = raw_stem((char *)term);
	strcpy(destination, trule.text);
	}
else
	strcpy(destination, term);
return 3;
}

/*
	ANT_STEM_PAICE_HUSK::INITTABLE()
	--------------------------------
*/
void ANT_stem_paice_husk::inittable(ANT_paice_husk_rule t[26])
{
int x;

/* For each item in the list.. */
for(x = 0; x < 26;x++)
	{ 
	(t + x)->next = NULL; /* Set the 'next' pointer to NULL,and.. */
	strcpy((t + x)->text,"dummy"); /* ..set the text field to 'dummy' */
	}
}

/*
	ANT_STEM_PAICE_HUSK::ISVOWEL()
	------------------------------
	Return true (1) if it's a vowel,or false otherwise
	NB - Treats 'y' as a vowel for the stemming purposes!
*/
int ANT_stem_paice_husk::isvowel(char s)
{
switch (s)
	{
	case 'a':case 'e':case 'i': case 'o':case 'u':case 'y':
		return 1;
	}
return 0;
}

/*
	ANT_STEM_PAICE_HUSK::ISCONSONANT()
	----------------------------------
	Return true (1) if it's a consonant,or false otherwise
*/
int ANT_stem_paice_husk::isconsonant(char s)
{
return islower(s) && !isvowel(s);
} 

/*
	ANT_STEM_PAICE_HUSK::ACCEPTABLE()
	---------------------------------
	Acceptability condition:if the stem begins with a vowel,then it
	must contain at least 2 letters,one of which must be a consonant
	If, however, it begins with a vowel then it must contain three
	letters and at least one of these must be a vowel or 'y', ok?
*/
int ANT_stem_paice_husk::acceptable(char *s)
{
int x;
char *ch;

/* must be a sequence of alphabetic characters (ie, not a number) */
for (ch = s; *ch != '\0'; ch++)
	if (!islower((unsigned char)*ch))
		return 0;

/* If longer than 3 chars then don't worry */
if((x = (int)strlen(s)) > 3)
	return 1;

/* If first is a vowel,then second must be a consonant */
if(isvowel(*s))
	return isconsonant(*(s + 1));

/* If first is a consonant,then second or third must be */
/* a vowel and length must be >3 */
return isvowel((*(s + 1)) || isvowel(*(s + 2))) && (x > 3);
}

/*
	ANT_PAICE_HUSK_RULE_SET
	-----------------------
	The Paice-Husk rule set embedded from the rules file that came with this code
*/
const char *ANT_paice_husk_rule_set[] =
{
"ia,?,intact\t;-ia > -- if intact \n",
"a,?,intact\t;-a > -- if intact\n",
"bb,b,stop\t;-bb > -b \n",
"ytic,ys,stop\t;-ytic > -ys\n",
"ic,?,continue\t;-ic > -- cont\n",
"nc,nt,continue\t;-nc > -nt cont\n",
"dd,d,stop\t;-dd > -d \n",
"ied,y,continue\t;-ied > -y cont\n",
"ceed,cess,stop\t;-ceed > -cess\n",
"eed,ee,stop\t;-eed > -ee\n",
"ed,?,continue\t;-ed > -- cont\n",
"hood,?,continue\t;-hood > -- cont\n",
"e,?,continue\t;-e > -- cont\n",
"lief,liev,stop\t;-lief > -liev\n",
"if,?,continue\t;-if > -- cont\n",
"ing,?,continue\t;-ing > -- cont\n",
"iag,y,stop\t;-iag > -y\n",
"ag,?,continue\t;-ag > -- cont\n",
"gg,g,stop\t;-gg > -g\n",
"th,?,intact\t;-th > -- if intact\n",
"guish,ct,stop\t;-guish > -ct\n",
"ish,?,continue\t;-ish > -- cont\n",
"i,?,intact\t;-i > -- if intact\n",
"i,y,continue\t;-i > -y\n",
"ij,id,stop\t;-ij > -id \n",
"fuj,fus,stop\t;-fuj > -fus\n",
"uj,ud,stop\t;-uj > -ud\n",
"oj,od,stop\t;-oj > -od\n",
"hej,her,stop\t;-hej > -her\n",
"verj,vert,stop\t;-verj > -vert\n",
"misj,mit,stop\t;-misj > -mit\n",
"nj,nd,stop\t;-nj > -nd\n",
"j,s,stop\t;-j > -s\n",
"ifiabl,?,stop\t;-ifiable > --\n",
"iabl,y,stop\t;-iabl > -y\n",
"abl,?,continue\t;-abl > -- cont\n",
"ibl,?,stop\t;-ibl > --\n",
"bil,bl,continue\t;-bil > -bl cont\n",
"cl,c,stop\t;-cl > -c\n",
"iful,y,stop\t;-iful > -y\n",
"ful,?,continue\t;-ful > -- cont\n",
"ul,?,stop\t;-ul > --\n",
"ial,?,continue\t;-ial > -- cont\n",
"ual,?,continue\t;-ual > -- cont\n",
"al,?,continue\t;-al > -- cont\n",
"ll,l,stop\t;-ll > -l\n",
"ium,?,stop\t;-ium > --\n",
"um,?,intact\t;-um > -- if intact\n",
"ism,?,continue\t;-ism > -- cont\n",
"mm,m,stop\t;-mm > -m\n",
"sion,j,continue\t;-sion > -j cont\n",
"xion,ct,stop\t;-xion > -ct\n",
"ion,?,continue\t;-ion > -- cont\n",
"ian,?,continue\t;-ian > -- cont\n",
"an,?,continue\t;-an > -- cont\n",
"een,?,protect\t; Protect -een\n",
"en,?,continue\t;-en > -- cont\n",
"nn,n,stop\t;-nn > -n\n",
"ship,?,continue\t;-ship > -- cont\n",
"pp,p,stop\t;-pp > -p\n",
"er,?,continue\t;-er > -- cont\n",
"ear,?,protect\t; Protect -ear\n",
"ar,?,stop\t;-ar > --\n",
"or,?,continue\t;-or > -- cont\n",
"ur,?,continue\t;-ur > -- cont\n",
"rr,r,stop\t;-rr > -\n",
"tr,t,continue\t;-tr > -t cont\n",
"ier,y,continue\t;-ier > -y cont\n",
"ies,y,continue\t;-ies > -y cont\n",
"sis,s,stop\t;-sis > -s\n",
"is,?,continue\t;-is > -- cont\n",
"ness,?,continue\t;-ness > -- cont\n",
"ss,?,protect\t; Protect -ss\n",
"ous,?,continue\t;-ous > -- cont\n",
"us,?,intact\t;-us > -- if intact\n",
"s,?,contint\t;-s > -- if intact,cont\n",
"s,?,stop\t;-s > --\n",
"plicat,ply,stop\t;-plicat > ply\n",
"at,?,continue\t;-at > -- cont\n",
"ment,?,continue\t;-ment > -- cont\n",
"ent,?,continue\t;-ent > -- cont\n",
"ant,?,continue\t;-ant > -- cont\n",
"ript,rib,stop\t;-ript > -rib\n",
"orpt,orb,stop\t;-orpt > -orb\n",
"duct,duc,stop\t;-duct > -duc\n",
"sumpt,sum,stop\t;-sumpt > -sum\n",
"cept,ceiv,stop\t;-cept > -ceiv\n",
"olut,olv,stop\t;-olut > -olv\n",
"sist,?,protect\t; Protect -sist\n",
"ist,?,continue\t;-ist > -- cont\n",
"tt,t,stop\t;-tt > -t\n",
"iqu,?,stop\t;-iqu > --\n",
"ogu,og,stop\t;-ogu > -og\n",
"siv,j,continue\t;-siv > -j cont\n",
"eiv,?,protect\t; Protect -eiv\n",
"iv,?,continue\t;-iv > -- cont\n",
"bly,bl,continue\t;-bly > -bl cont\n",
"ily,y,continue\t;-ily > -y cont\n",
"ply,?,protect\t; Protect -ply\n",
"ly,?,continue\t;-ly > -- cont\n",
"ogy,og,stop\t;-ogy > -og\n",
"phy,ph,stop\t;-phy > -ph\n",
"omy,om,stop\t;-omy > -om\n",
"opy,op,stop\t;-opy > -op\n",
"ity,?,continue\t;-ity > -- cont\n",
"ety,?,continue\t;-ety > -- cont\n",
"lty,l,stop\t;-lty > l\n",
"istry,?,stop\t;-istry > --\n",
"ary,?,continue\t;-ary > -- cont\n",
"ory,?,continue\t;-ory > -- cont\n",
"ify,?,stop\t;-ify > --\n",
"ncy,nt,continue\t;-ncy > -nt cont\n",
"acy,?,continue\t;-acy > -- cont\n",
"iz,?,continue\t;-iz > -- cont\n",
"yz,ys,stop\t;-yz > ys\n",
NULL
} ;
