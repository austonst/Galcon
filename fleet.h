/*
  -----Fleet Class Declaration-----
  Auston Sterling
  6/28/12
  austonst@gmail.com

  Contains the declaration for the Fleet class.

  As a note: For a small increase in memory cost, we could lose the dependency on
  planet.h and just store the planet's size. Could help if circular dependencies
  need resolving.
*/

#ifndef _fleet_h_
#define _fleet_h_

#include <vector>
#include "vec2f.h"
#include "planet.h"

const int DEFAULT_FLEET_SPEED = 100;

class Fleet
{
 public:
  //Constructors
  Fleet();
  Fleet(const std::vector<int>& inships, Planet* begin, Planet* end);

  //Accessors
  Vec2f pos() const {return pos_;}
  double x() const {return pos_.x();}
  double y() const {return pos_.y();}
  std::vector<int> ships() const {return ships_;}
  Planet* dest() {return dest_;}
  int owner() const {return owner_;}

  //General use functions
  void update();
  void display(SDL_Surface* screen, const SDL_Rect& camera);
  bool takeHit(int damage, const std::vector<std::pair<float, float> >& shipstats);
  
 private:
  //Current coordinates of the fleet
  Vec2f pos_;

  //Destination planet
  Planet* dest_;

  //The speed, in pixels/second
  int speed_;

  //The count of ships
  std::vector<int> ships_;

  //The ticks at the last time update was called
  int lastTicks_;

  //The owner of the fleet
  int owner_;

  //Variables to keep track of accumulated, yet unapplied, damage
  int damage_;
  int damageTarget_;
};

typedef std::list<Fleet>::iterator fleetIter;
typedef std::list<Fleet>::const_iterator fleetIterConst;

#endif
