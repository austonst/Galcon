/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----BuildingInstance Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  Implementation of the BuildingInstance class.
*/

#include "buildingInstance.h"

#ifndef _buildinginstance_cpp_
#define _buildinginstance_cpp_

//Default constructor
BuildingInstance::BuildingInstance() :
  type_(NULL),
  projectileTime_(0),
  time_(0)
{}

//Regular constructor
BuildingInstance::BuildingInstance(Building* type) :
  type_(type),
  projectileTime_(0),
  time_(0)
{}

//Progresses the building
void BuildingInstance::update()
{
  //Find time since last call
  if (time_ == 0) {time_ = SDL_GetTicks(); return;}
  int currentTime = SDL_GetTicks();
  int dt = currentTime - time_;
  time_ = currentTime;

  //Update projectileTime_ if the building exists
  if (type_ != NULL)
    {
      projectileTime_ += dt;

      //Do not allow projectileTime to store up extra shots
      if (projectileTime_ > type_->cd()) projectileTime_ = type_->cd();
    }
}

//Destroys the building
void BuildingInstance::destroy()
{
  type_ = NULL;
  projectileTime_ = 0;
  time_ = 0;
}

//Fires a projectile. Here, this only reduces projectileTime_ appropriately
//Returns true if it completed successfully, false if not enough time has passed
bool BuildingInstance::fire()
{
  if (!canFire()) return false;
  projectileTime_ -= type_->cd();
  return true;
}
  
#endif
