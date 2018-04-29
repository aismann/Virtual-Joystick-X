// Include of the VirtualJoystick header, along with Audio (just in case)
#include "VirtualJoystick.h"
#include "SimpleAudioEngine.h"

// Call to use the Cocos2D namespace
USING_NS_CC;

// Create Scene, based off of HelloWorld, is called to start the scene from AppDelegate
Scene* VirtualJoystick::createScene()
{
    // Create a scene
    auto scene = Scene::create();
    // The VirtualJoystick scene is actually a "layer" ontop of the scene
    auto layer = VirtualJoystick::create();
    // Add VirtualJoystick to began
    scene->addChild(layer);
    
    // Argument needed for AppDelegate
    return scene;
}

// on "init" you need to initialize your instance
bool VirtualJoystick::init()
{
    // This is standard for Cocos2D based programs, works like an IOException
    if ( !Scene::init() )
    {
        return false;
    }

    // Get the screens visible size, the Director class handles the math for all screen sizes
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // Set an origin using the Director class
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // Initialize Variables
    stickActive = false;
    angularVelocity = 0;
    velocity = Point(0, 0);
    
    // Background color to white
    auto background = cocos2d::LayerColor::create(Color4B(255, 255, 255, 255));
    // Add to the scene
    this->addChild(background);
    
    // Create "player" as a sprite
    player = Sprite::create("player.png");
    // Set the position to the middle of the screen
    player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    // Add the player to the scene
    addChild(player);
    
    // Create "dpad" as a sprite
    dpad = Sprite::create("dpad.png");
    // Set the position 100 pixels up and 100 pixels right from the bottom left corner
    dpad->setPosition(100, 100);
    // Set the anchorpoint to the middle of the "sprite," versus the standard bottom left
    dpad->setAnchorPoint(Vec2(0.5f, 0.5f));
    // Add dpad to the scene
    addChild(dpad);
    
    // Create "joystick" as a sprite, NOTE: this is AFTER dpad so that it rests ON TOP (otherwise touch events won't work properly
    joystick = Sprite::create("joystick.png");
    // Image is the same size as the dpad, so setting a similar position will keep things consistent
    joystick->setPosition(100, 100);
    // Set the anchorpoint to the center
    joystick->setAnchorPoint(Vec2(0.5f, 0.5f));
    // Add joystick to the scene
    addChild(joystick);
    
    // Add an Event Listener to listen for User interaction via "touch"
    auto touchListener = EventListenerTouchOneByOne::create();
    
    // The following uses touchListener to "listen" to the various Cocos2D touch callbacks
    touchListener->onTouchBegan = CC_CALLBACK_2(VirtualJoystick::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(VirtualJoystick::onTouchEnded, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(VirtualJoystick::onTouchMoved, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(VirtualJoystick::onTouchCancelled, this);
    
    // EventDispatcher is a delegate and sets priorities to what has happened with touch events
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    // Cocos2D function for all delegates and actions to work
    scheduleUpdate();
    
    return true;
}

// Cocos2D Update, being used for the VirtualJoystick scene
void VirtualJoystick::update(float delta)
{
    // If velocity is not 0...
    if ((velocity.x != 0) || velocity.y != 0) {
        // Get the current position of the player...
        auto playerPosition = player->getPosition();
        // Value to move the player based off of velocity, adding it's own vector to the velocity vector
        playerPosition = Vec2(playerPosition.x + velocity.x, playerPosition.y + velocity.y); //ccpAdd
        // Move the player itself
        player->setPosition(playerPosition);
    }
    // Again, if velocity is not 0...
    if ((velocity.x != 0) || velocity.y != 0) {
        // Value to get the player's CURRENT rotation
        auto rotation = player->getRotation();
        // Set the above value to the current angularVelocity...
        rotation = angularVelocity;
        //..then set the player's rotation
        player->setRotation(rotation);
    }
}

// If the User has made a touch...
bool VirtualJoystick::onTouchBegan(Touch* touch, Event* event)
{
    // Value to get the location of the user's touch in the view
    Point location = touch->getLocationInView();
    // That value must be converted to OpenGL, to render
    location = Director::getInstance()->convertToGL(location);
    
    // Get the entire "size" of the joystick sprite
    Rect joystickBoundingBox = joystick->getBoundingBox();
    
    // If there is a touch inside the joystick sprite...
    if (joystickBoundingBox.containsPoint(location)) {
        // ... The joystick can now be moved
        stickActive = true;
    } else {
        //... Or not
        stickActive = false;
    }
    
    // Debugging purposes to verify the whole method is working
    //cocos2d::log("Touch began");
    
    return true;
}

// If the user has lifted their finger...
void VirtualJoystick::onTouchEnded(Touch* touch, Event* event)
{
    // If the stick is allowed to move...
    if (stickActive == true) {
        // Get the position of the dpad (which should not change)
        auto dpadPosition = dpad->getPosition();
        // Value to set an action where the joystick will "move" back to its original position (center of dpad) at 0.1 seconds
        auto move = cocos2d::MoveTo::create(0.1f, dpadPosition);
        // FIX: This was a solution I found works to make the action work, like a bug fix
        joystick->stopAllActions();
        // Have the joystick move by running the action
        joystick->runAction(move);
    }
    // Reset the velocity, just to create a clean slate.
    resetVelocity();
}

// If the user is touching the screen AND moving...
void VirtualJoystick::onTouchMoved(Touch* touch, Event* event)
{
    // Similar as above, get the user's location...
    Point location = touch->getLocationInView();
    // ...and convert to OpenGL for rendering
    location = Director::getInstance()->convertToGL(location);
    
    // Get the dpad position, again shouldn't move but is used a reference point
    auto dpadPosition = dpad->getPosition();
    
    // If the joystick is allowed to move
    if (stickActive == true) {
        // Get a vector based off the location the user is touching against the current dpad position
        auto vector = Vec2(location.x - dpadPosition.x, location.y - dpadPosition.y);
        // Based off that vector, get an angle
        auto angle = atan2(vector.y, vector.x);
        // Based off that angle, get a degree
        auto degree = angle * (180 / M_PI);
        // Math to setup radians
        auto radians = M_PI / 2;
        // Get the total size of the dpad sprite
        Rect dpadBoundingBox = dpad->getBoundingBox();
        // Create a length based off height (or width) / 2, this allows a LIMIT of how far the joystick can "move" virtually
        auto length = dpadBoundingBox.size.height / 2;
        // Trig function to make a distance for X, all right angles
        auto xDist = sin(angle - radians) * length;
        // Trig function to make a distance for Y, all right angles
        auto yDist = cos(angle - radians) * length;
        
        // Get the current position of the joystick
        auto joystickPosition = joystick->getPosition();
        // With the dpads position, the above trig functions allow the position of the Joystick in relation to touch events (gives a lot of freedom for user movement)
        joystickPosition = Point(dpadPosition.x - xDist, dpadPosition.y + yDist);
        
        // If the user has selected a point inside the dpad area...
        if (dpadBoundingBox.containsPoint(location)) {
            // ...Set the joystick's sprite position to that location
            joystick->setPosition(location);
        } else {
            // ... otherwise if the user has moved off that location while moving, just move the position of the joystick. This helps for better UI (incase people have LARGE fingers)
            joystickPosition = Point(dpadPosition.x - xDist, dpadPosition.y + yDist);
        }
        
        // Since there is movement going on, use Trig function vectors and set the velocity.
        // 0.1 is a tiny amount because 60 FPS is very, very fast. xDist is multiplied by a negative because xDist itself returns negative
        velocity = Vec2(xDist * -0.1, yDist * 0.1);
        // Use this variable to set the angular velocity based off the current degree, - 90 to set orientation, but it has to return negative
        angularVelocity = -(degree - 90);
    }

}

// Not if the user lifted their finger, but if the touch was cancelled (the user moved between apps and then returned to this one)
void VirtualJoystick::onTouchCancelled(Touch* touch, Event* event)
{
    // We will be "nice" and set their velocity back to 0 so the player doesn't continue to move without their input
    resetVelocity();
}

// Method to reset velocity for both X and Y to 0
void VirtualJoystick::resetVelocity()
{
    velocity = Point(Vec2(0, 0));
}

