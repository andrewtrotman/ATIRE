/*
 * name_container.h
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#ifndef NAME_CONTAINER_H_
#define NAME_CONTAINER_H_

namespace QLINK {

	class name_container
	{
	public:
		name_container();
		virtual ~name_container();
		static name_container& instance();

	private:
		void init();
	};

	inline name_container& name_container::instance() {
		static name_container inst;
		return inst;
	}

}

#endif /* NAME_CONTAINER_H_ */
