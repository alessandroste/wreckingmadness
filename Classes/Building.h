#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Floor.h"

namespace wreckingmadness {
	class Building {
	private:
		std::list<Floor *> floor_list;
	public:
		Building();
		~Building();
		int getNumber();
		void addFloor(Floor *fl); //add floor at the end
		void removeFloor(); //remove floor from the top
		Floor *getUpperFloor();
		Floor *getLowerFloor();
		Floor *getNFloor(unsigned int N);
	};
}

#endif