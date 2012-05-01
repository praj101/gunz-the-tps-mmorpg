#pragma once

#include <set>
using namespace std;

#include "CCQuestNPC.h"


class ZActorSoundManager : public set<CCQUEST_NPC>
{
private:
public:
	ZActorSoundManager() {}
	virtual ~ZActorSoundManager() {}
};