#include "pch.h"
#include "Logic.h" 

using namespace cocos2d;

void Logic::addToggle(gd::PauseLayer* This, cocos2d::CCMenu* menu) {
	This->createToggleButton(menu_selector(Logic::onToggle), !m_bEnabled, menu, "Stretched", { 110.0f, 265.0f });
}

void Logic::onToggle(cocos2d::CCObject*) {
	m_bEnabled = !m_bEnabled;
	Logic::update(CCDirector::sharedDirector()->getRunningScene());
}

void Logic::update(cocos2d::CCScene* scene) {
	if (m_bEnabled) {
		scene->setScale(m_fScaleX, m_fScaleY);
	}
	else {
		scene->setScale(1.0f, 1.0f);
	}
}