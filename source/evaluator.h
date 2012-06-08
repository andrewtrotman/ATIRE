/*
	EVALUATOR.H
	-----------
*/
#ifndef EVALUATOR_H_
#define EVALUATOR_H_

class ANT_memory;
class ANT_relevant_topic;
class ANT_relevant_document;
class ANT_search_engine;
class ANT_evaluation;

class ANT_evaluator
{
friend class ANT_evaluation;

public:
	long is_initialised;
	long number_evaluations_used;
	char **evaluation_names;

private:
	ANT_memory *memory;
	ANT_relevant_document *relevant_document_list;
	long long relevant_document_list_length;

	ANT_relevant_topic *relevant_topic_list;
	long long relevant_topic_list_length;

	long evaluations_performed;
	long number_evaluation_spaces;
	ANT_evaluation **evaluators;
	double *evaluations;

public:
	ANT_evaluator();
	~ANT_evaluator();

	static void help(char *title, char switch_char);
	
	long long initialise(ANT_memory *memory, ANT_relevant_document *relevant_document_list, long long relevant_document_list_length);
	ANT_relevant_topic *setup(long topic, long subtopic);

	long add_evaluation(char *which);
	double *perform_evaluation(ANT_search_engine *search_engine, long topic);

private:
	long get_parameter(char *from, double *into);
};

#endif /* EVALUATOR_H_ */
