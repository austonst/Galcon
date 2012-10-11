/*
  -----Fleet Class Definition-----
  Auston Sterling
  6/28/12
  austonst@gmail.com

  Contains the definition for the Fleet class.
*/

#ifndef _fleet_cpp_
#define _fleep_cpp_

#include "fleet.h"
#include <cmath>

//Default constructor, should probably not be used
Fleet::Fleet():pos_(0,0), dest_(NULL), speed_(0), lastTicks_(0), owner_(0)
{
}

//Regular constructor
Fleet::Fleet(int inships, int intype, ShipStats shipstats, Planet* begin, Planet* end):
  pos_(begin->x() + (UNSCALED_PLANET_RADIUS * begin->size()), 
       begin->y() + (UNSCALED_PLANET_RADIUS * begin->size())),
  dest_(end),
  speed_(shipstats.speed),
  ships_(inships),
  type_(intype),
  lastTicks_(0),
  owner_(begin->owner()),
  damage_(0)
{
}

//Returns the total attack power of the fleet
float Fleet::totalAttack(const std::vector<ShipStats> & shipstats) const
{
  return float(ships_) * shipstats[type_].attack;
}

//Returns the total defense of the fleet
float Fleet::totalDefense(const std::vector<ShipStats> & shipstats) const
{
  return float(ships_) * shipstats[type_].defense;
}

//Update function
void Fleet::update()
{
  //Check for first call
  if (lastTicks_ == 0)
    {
      lastTicks_ = SDL_GetTicks();
      return;
    }

  //Find change in time
  int newtime = SDL_GetTicks();
  int dt = newtime - lastTicks_;
  lastTicks_ = newtime;

  //Move fleet towards destination
  //Find target coordinates
  Vec2f tar(dest_->x() + (UNSCALED_PLANET_RADIUS * dest_->size()),
	    dest_->y() + (UNSCALED_PLANET_RADIUS * dest_->size()));

  //Find the proper vector to add
  Vec2f diff = tar-pos_;
  diff.normalize();
  diff *= speed_*(dt/1000.0);

  //Move the fleet
  pos_ += diff;
}

//Display function
void Fleet::display(SDL_Surface* screen, const SDL_Rect& camera)
{
  //For now, just draw a rectangle
  SDL_Rect rect = {Sint16(pos_.x() - 10 - camera.x), Sint16(pos_.y() - 10 - camera.y), 20, 20};
  SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
}

//Applies damage to the fleet
//Returns false if this hit would destroy the fleet
bool Fleet::takeHit(int damage, const std::vector<ShipStats> & shipstats)
{
  //Add in any extra damage from last time
  damage += damage_;

  //Convert damage to ship loss
  int shiploss = damage / shipstats[type_].defense;

  //If we can wipe out the whole fleet
  if (shiploss >= ships_) return false;

  //Otherwise, deal some damage
  ships_ -= shiploss;

  //Keep track of any extra accumulated damage
  damage_ = damage % int(shipstats[type_].defense);
  return true;
}

#endif
