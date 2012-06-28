/*
  -----Fleet Class Declaration-----
  Auston Sterling
  4/4/12
  austonst@gmail.com

  Contains the declaration for the Fleet class.
*/

#ifndef _fleet_h_
#define _fleet_h_

#include <vector>
#include "planet.h"

const int DEFAULT_FLEET_SPEED = 100;

class Fleet
{
 public:
  //Constructors
  Fleet();
  Fleet(const std::vector<int>& inships, Planet* begin, Planet* end);

  //Accessors
  float x() {return x_;}
  float x() const {return x_;}
  float y() {return y_;}
  float y() const {return y_;}
  std::vector<int> ships() {return ships_;}
  Planet* dest() {return dest_;}
  Planet* start() {return start_;}
  int onwer() {return owner_;}
  int owner() const {return owner_;}

  //General use functions
  void update();
  void display(SDL_Surface* screen, const SDL_Rect& camera);
  bool takeHit(int damage, const std::vector<std::pair<float, float> >& shipstats);
  
 private:
  //Current coordinates of the fleet
  float x_, y_;

  //Target and source planets
  Planet* dest_;
  Planet* start_;

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
