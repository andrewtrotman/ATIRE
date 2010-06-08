/*
 * dic2freq.h
 *
 *  Created on: May 4, 2010
 *      Author: monfee
 */

#ifndef DIC2FREQ_H_
#define DIC2FREQ_H_

class Dic;
class Freq;

class Dic2Freq
{
public:
	Dic2Freq();
	virtual ~Dic2Freq();

	static void dic2freq(Dic *dic, Freq *freq, long lang);
	static void dic2freq2(Dic *dic, Freq *freq, long lang);
};

#endif /* DIC2FREQ_H_ */
