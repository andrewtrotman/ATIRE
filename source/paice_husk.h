/*
	PAICE_HUSK.H
	------------
*/
#ifndef PAICE_HUSK_H_
#define PAICE_HUSK_H_

#define maxwdsz 250 			/* Maximum length of word stemmable */
#define maxsuffsz 8 			/* Maximum length of suffix */

class ANT_paice_husk_rule
{
public:
	char text[maxwdsz]; 		/* To return stemmer output */
	char keystr[maxsuffsz]; 	/* Key string,ie,suffix to remove */
	char repstr[maxsuffsz]; 	/* string to replace deleted letters */
	char intact; 				/* Boolean-must word be intact? */
	char cont; 					/* Boolean-continue with another rule? */
	int rulenum; 				/* Line number of rule in rule list file */
	char protect; 				/* Boolean-protect this ending? */
	char deltotal; 				/* Delete how many letters? */
	ANT_paice_husk_rule *next;	/* Next item in linked list */
};

class ANT_paice_husk
{
private:
	ANT_paice_husk_rule ttable[26];

private:
	int tblindex(char *s);
	int isvalidstr(char *s);
	char addrule(ANT_paice_husk_rule r, ANT_paice_husk_rule t[26]);
	int applyrule(ANT_paice_husk_rule *r, char *word, int isintact);
	int rulewalk(char *word, ANT_paice_husk_rule t[26], int isintact, ANT_paice_husk_rule *used);
	ANT_paice_husk_rule makerule(char *s, int line);
	void readrules(ANT_paice_husk_rule ttable[26]);
	int flagerror(char *s);
	void inittable(ANT_paice_husk_rule t[26]);
	int isvowel(char s);
	int isconsonant(char s);
	int acceptable(char *s);
public:
	ANT_paice_husk();
	ANT_paice_husk_rule stem(char *s);
} ;

extern const char *ANT_paice_husk_rule_set[];

#endif  /* PAICE_HUSK_H_ */
