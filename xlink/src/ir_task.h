/*
 * ir_task.h
 *
 *  Created on: Aug 18, 2009
 *      Author: monfee
 */

#ifndef IR_TASK_H_
#define IR_TASK_H_

namespace QLINK {

	class ir_task
	{
	public:
		enum task_type { IR_TASK_NONE = 0,
						IR_TASK_LTW = 1
						};

	private:
		task_type	type_;

	public:
		ir_task() : type_(IR_TASK_NONE) {}
		ir_task(task_type type) : type_(type) {}
		virtual ~ir_task() { }

		void type(task_type type) { type_ = type; }
		task_type type() { return type_; }
	};

}
#endif /* IR_TASK_H_ */
