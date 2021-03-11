#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "pch.h"

class Logic {
protected:
	static inline bool m_bEnabled = true;
	static inline constexpr float m_fScaleX = 1.33f;
	static inline constexpr float m_fScaleY = 1.0f;

protected:
	void onToggle(cocos2d::CCObject*);

public:
	static void addToggle(gd::PauseLayer* This, cocos2d::CCMenu* menu);
	static void update(cocos2d::CCScene* scene);
};

#endif
