#ifndef __VIRTUAL_JOYSTICK_H__
#define __VIRTUAL_JOYSTICK_H__

// Include the cocos2d library
#include "cocos2d.h"

// Include for debugging purposes
#include <iostream>
using namespace std;

// Define a new class for each "scene" that is from the Cocos2d Engine
class VirtualJoystick : public cocos2d::Scene
{
public:
    // Requirement to initialize scene (found in the AppDelegate files)
    static cocos2d::Scene* createScene();

    // Not "required" but a method that initializes everything on launch, such as nodes, sprites, various objects
    virtual bool init() override;
    
    // Methods that are part of the Cocos2d Engine, allows for touch events Android and iOS
    // Works with a mouse too for minor events
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);
    
    // Method to reset the velocity for use with the Virtual Joystick and the "player"
    virtual void resetVelocity();
    
    // Not "required" but needed for FPS redrawing onscreen if nodes are moved by physics or something else.
    void update(float) override;
    
    
    
    // implement the "static create()" method manually (Based off the HelloWorld scene)
    CREATE_FUNC(VirtualJoystick);
    
private:
    // Class Variables
    
    // Nodes presented onscreen
    cocos2d::Sprite* dpad;
    cocos2d::Sprite* joystick;
    cocos2d::Sprite* player;
    
    // Variables that will require recalculations based off the trigonometry and physics
    bool stickActive;
    float angularVelocity;
    cocos2d::Point velocity;
    
};

#endif
