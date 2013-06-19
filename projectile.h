/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----Projectile Class Declaration-----
  Auston Sterling
  austonst@gmail.com

  Contains the header for the Projectile class.
*/

#ifndef _projectile_h_
#define _projectile_h_

#include <string>
#include "fleet.h"
#include "vec2f.h"

const int DEFAULT_PROJECTILE_SPEED = 200;

class Projectile
{
 public:
  //Constructors
  Projectile();
  Projectile(Vec2f start, Fleet* dest, std::string effect, float speed);

  //Accessors
  Vec2f pos() const {return pos_;}
  double x() const {return pos_.x();}
  double y() const {return pos_.y();}
  Fleet* target() {return target_;}
  std::string effect() const {return effect_;}

  //General use functions
  void update();
  void display(SDL_Surface* screen, const SDL_Rect& camera);
  
 private:
  //Current coordinates of the projectile
  Vec2f pos_;

  //Target fleet
  Fleet* target_;

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
