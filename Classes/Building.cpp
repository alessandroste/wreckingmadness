#include "Building.h"
#include "Floor.h"

using namespace wreckingmadness;

Building::Building(){}

Building::~Building(){}

int Building::getNumber(){
	return floor_list.size();
}

void Building::addFloor(Floor * fl){
	floor_list.push_back(fl);
}

void Building::removeFloor(){
	floor_list.pop_front();
}

Floor * Building::getUpperFloor(){
	return floor_list.front();
}

Floor * Building::getLowerFloor(){
	return floor_list.back();
}

Floor * Building::getNFloor(unsigned int N){
	int i = 0;
	auto it = floor_list.begin();
	while (it != floor_list.end() && i < N) {
		++it;
		i++;
	}
	if (N > floor_list.size()) return nullptr;
	else return *it;
}
