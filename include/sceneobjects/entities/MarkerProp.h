/**
 * MarkerProp.h
 *
 * Author: nrudzicz
 * Created on: 20-Jun-2009
 */

#ifndef MARKERPROP_H_
#define MARKERPROP_H_

#include <vector>

#include "engine/Storyteller.h"
#include "sceneobjects/entities/Prop.h"

using namespace std;


class MarkerProp : public Prop {
private:
	vector<HUDMessage*> messages;

protected:
public:
	MarkerProp(PropType, int, RiddleType, PropType[]);
	virtual ~MarkerProp();

	void DescribeYourself();
};

#endif /* MARKERPROP_H_ */
