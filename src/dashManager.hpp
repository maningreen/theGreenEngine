#ifndef dashManager_h
#define dashManager_h

#include <raylib.h>

// a semi-abstract class representing dashes/resources
// it manages general dashing, the delays, recharge, etc
class DashManager {
  private:
    // the progress on dashing, if >1 then that means you can dash, every
    // integer is one more dash available
    float dashProgress;
    float getNextDashProgress(float delta);

    // the direction the dash is
    Vector2 dashVelocity;

    bool dashing;

    // time elapsed since last use
    float deltaDash;

    // returns a new vector for the velocity based off of the input
    // **input should be a normalized vector**
    Vector2 applyInput(float delta, Vector2 input);
  public:
    // the speed at which you dash
    float dashSpeed;
    // the amount of control you have over a dash
    float dashControl;
    // the delay after dashing you begin to regenerate
    float dashRegenDelay;
    // the rate at which the dash regenerates per second
    float dashRegenRate;
    // the length a dash takes (s)
    float dashLength;

    // the maximum amount of dashes available
    // if 0, no dashes are available, and this class is redundant
    unsigned maxDashCount;

    // returns 0 able to dash
    // other return codes:
    //  1: already dashing
    //  2: not enough dash progress
    int beginDash(Vector2 dir);

    // returns the amount of available dashes
    int getAvailableDashes();

    // returns true if dashCount >= 1 and not dashing
    bool canDash();

    bool isDashing();

    float getDashProgress();
    
    void addDashProgress();

    // removes the equivilent of one dash
    // if there're no available dashes does nothing.
    void removeDashProgress();

    // returns the direction one dashes, in a normalized vector
    // for a scaled vector use `Vector2 getDashVelocity()`
    Vector2 getDashDirection();

    // returns the velocity of the dash
    // for a normalized vector use `Vector2 getDashDirection()`
    Vector2 getDashVelocity();

    // returns the elapsed time since the last time `removeDashProgress()` was called
    float getDeltaDash();

    // input {0, 0} as the input vector for no change
    // similar to Process(float delta) on entities.
    // returns the equivilent to getDashVelocity
    Vector2 manageDash(float delta, Vector2 inputVector);

    // given delta, it will return a vector to increment a position by
    // useful only when the parent hasn't a velocity
    Vector2 getPositionIncrement(float delta);

    DashManager(unsigned dashCount,
      float dashLength,
      float dashRegenDelay,
      float dashControl,
      float dashSpeed
    );
    ~DashManager();
};

#endif
