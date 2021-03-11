#ifndef __HOOKS_H__
#define __HOOKS_H__

#include "pch.h"
#include "../logic/Logic.h"

namespace gates {
	void(__thiscall* PauseLayer_customSetup)(gd::PauseLayer*);
	bool(__thiscall* CCDirector_replaceScene)(cocos2d::CCDirector*, cocos2d::CCScene*);
}

namespace hooks {
	void __fastcall PauseLayer_customSetup(gd::PauseLayer* This) {
		gates::PauseLayer_customSetup(This);
		Logic::addToggle(This, static_cast<cocos2d::CCMenu*>(This->getChildren()->objectAtIndex(12)));
	}
	bool __fastcall CCDirector_replaceScene(cocos2d::CCDirector* This, void*, cocos2d::CCScene* scene) {
		
		if (auto layer = gd::GameManager::sharedState()->getPlayLayer()) {
			if (layer->getParent() == scene) Logic::update(scene);
		}
		return gates::CCDirector_replaceScene(This, scene);
	}
}

#endif