/*
  -----Projectile Class Declaration-----
  Auston Sterling
  4/4/12
  austonst@gmail.com

  Contains the header for the Projectile class.
*/

#ifndef _projectile_h_
#define _projectile_h_

#include <string>
#include "fleet.h"

const int DEFAULT_PROJECTILE_SPEED = 300;

class Projectile
{
 public:
  //Constructors
  Projectile();
  Projectile(int xin, int yin, Fleet* dest, std::string effect, float speed);

  //Accessors
  float x() {return x_;}
  float x() const {return x_;}
  float y() {return y_;}
  float y() const {return y_;}

  //General use functions
  void update();
  void display(SDL_Surface* screen, const SDL_Rect& camera);
  
 private:
  //Current coordinates of the projectile
  float x_, y_;

  //Source coordinates and target fleet
  int startx_, starty_;
  Fleet* dest_;

  //The speed, in pixels per second
  int speed_;

  //The effect as a parsable string
  std::string effect_;

  //The ticks at the last time update was called
  int lastTicks_;
};

typedef std::list<Projectile>::iterator projectileIter;
typedef std::list<Projectile>::const_iterator projectileIterConst;

#endif
