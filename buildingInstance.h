/*
  -----BuildingInstance Class Header-----
  Auston Sterling
  austonst@gmail.com
  4/11/12

  Header for the BuildingInstance class. This class is a particular instance of a
  building on a planet, allowing for tracking of building-specific variables
  while still saving some space on the things that are the same across all
  buildings of that type.
*/

#include "building.h"
#include "vec2f.h"

#ifndef _buildinginstance_h_
#define _buildinginstance_h_

class BuildingInstance
{
 public:
  //Constructors
  BuildingInstance();
  BuildingInstance(Building* type);

  //Accessors
  //BuildingInstance variables
  int projectileTime() const {return projectileTime_;}
  bool exists() const {return type_ != NULL;}
  //Building variables
  SDL_Surface* rotation(float angle = -1, bool complete = true)
  {return type_->rotation(angle, complete);}
  std::string effect() const {return type_->effect();}
  int buildtime() const {return type_->buildtime();}
  float cd() const {return type_->cd();}
  int range() const {return type_->range();}
  bool canFire() const {return projectileTime_ > type_->cd();}

  //Regular use functions
  //BuildingInstance
  void update();
  void destroy();
  bool fire();
  //Building
  void display(Vec2f pos, float angle, SDL_Surface* screen, bool complete)
  {type_->display(pos, angle, screen, complete);}
  
 private:
  //Pointer to the building type represented
  Building* type_;

  //The time since the last time this building fired its projectile
  int projectileTime_;

  //The time of the last progress() call
  int time_;
};

#endif
