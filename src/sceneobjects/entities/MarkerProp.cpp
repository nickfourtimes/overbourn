/**
 * MarkerProp.cpp
 *
 * Author: nrudzicz
 * Created on: 20-Jun-2009
 */

#include "common.h"
#include "sceneobjects/entities/MarkerProp.h"


 /************************************************************************************************** PRIVATE HELPERS */

void pushSingleProp(vector<const char*>& msgv, PropType type) {
	switch (type) {
	case PropType::TOADSTOOL:
		msgv.push_back("Toadstool");
		break;
	case PropType::SHRUB:
		msgv.push_back("Shrub");
		break;
	case PropType::RUINS:
		msgv.push_back("Ruins");
		break;
	case PropType::FLOWER:
		msgv.push_back("Flower");
		break;
	case PropType::LIVETREE:
		msgv.push_back("Tree");
		break;
	case PropType::BERRY:
		msgv.push_back("Berry bush");
		break;
	case PropType::TOMB:
		msgv.push_back("Tomb");
		break;
	case PropType::STUMP:
		msgv.push_back("Stump");
		break;
	default:
		break;
	}

	return;
}


void pushPropList(vector<const char*>& msgv, PropType type[]) {
	int indices[] = { 0, 1, 2, 3 };
	FisherYatesShuffle<int>(indices, 4);

	for (int i = 0; i < 4; ++i) {
		switch (type[indices[i]]) {
		case PropType::TOADSTOOL:
			msgv.push_back("(T)oadstool");
			break;
		case PropType::SHRUB:
			msgv.push_back("(S)hrub");
			break;
		case PropType::RUINS:
			msgv.push_back("(R)uins");
			break;
		case PropType::FLOWER:
			msgv.push_back("(F)lower");
			break;
		case PropType::LIVETREE:
			msgv.push_back("Tre(e)");
			break;
		case PropType::BERRY:
			msgv.push_back("(B)erry bush");
			break;
		case PropType::TOMB:
			msgv.push_back("T(o)mb");
			break;
		case PropType::STUMP:
			msgv.push_back("Stum(p)");
			break;
		default:
			break;
		}
	}

	return;
}


/************************************************************************************************** {CON|DE}STRUCTORS */

MarkerProp::MarkerProp(PropType type, int nn, RiddleType riddle, PropType props[]) : Prop(type, nn) {
	vector<const char*> msg;

	// say that this is the marker
	msg.push_back("A bright pedestal stands");
	msg.push_back("in stark contrast");
	msg.push_back("to the dim marsh.");
	msg.push_back("An inscription reads:");
	messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	// identify the type of riddle and the important props
	switch (riddle) {
	case RiddleType::HOWMANY:
		msg.push_back("\"prettie marshie overbourn");
		msg.push_back("with many shinee friends.");
		msg.push_back("   you counts the");
		msg.push_back("   number of them or");
		msg.push_back("the nightmare never ends.\"");
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		msg.clear();

		msg.push_back("How many (1-3)");
		msg.push_back("of the following?");
		pushSingleProp(msg, props[0]);
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		break;  // end HOWMANY

	case RiddleType::SUBMERGED:
		msg.push_back("\"eatsie marshie overbourn");
		msg.push_back("takes all to swampie bed.");
		msg.push_back("   tell us if it");
		msg.push_back("   takes this thing");
		msg.push_back("or we takes you instead.\"");
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		msg.clear();

		msg.push_back("Is the following");
		msg.push_back("submerged? (y/n)");
		pushSingleProp(msg, props[0]);
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		break;  // end SUBMERGED

	case RiddleType::ODDMAN:
		msg.push_back("\"harshie marshie overbourn");
		msg.push_back("wants everything the same.");
		msg.push_back("   finds us wicked");
		msg.push_back("   different things");
		msg.push_back("or never wins the game.\"");
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		msg.clear();

		msg.push_back("Which does not belong?");
		pushPropList(msg, props);
		messages.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
		msg.clear();

		break;	// end ODDMAN
	}

	return;
}


MarkerProp::~MarkerProp() {
	for (vector<HUDMessage*>::iterator iter = messages.begin(); iter < messages.end(); ++iter) {
		delete* iter;
	}
	return;
}


/************************************************************************************************** METHODS */

void MarkerProp::DescribeYourself() {
	for (vector<HUDMessage*>::iterator iter = messages.begin(); iter < messages.end(); ++iter) {
		hud->PushMessage(new HUDMessage(**iter));
	}
	return;
}
