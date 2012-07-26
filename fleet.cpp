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
Fleet::Fleet(const std::vector<int>& inships, Planet* begin, Planet* end):
  pos_(begin->x() + (UNSCALED_PLANET_RADIUS * begin->size()), 
       begin->y() + (UNSCALED_PLANET_RADIUS * begin->size())),
  dest_(end),
  speed_(DEFAULT_FLEET_SPEED),
  ships_(inships),
  lastTicks_(0),
  owner_(begin->owner()),
  damage_(0),
  damageTarget_(0)
{
}

//Returns the total attack power of the fleet
float Fleet::totalAttack(const std::vector<std::pair<float, float> >& shipstats) const
{
  float att = 0;
  for (unsigned int i = 0; i < ships_.size(); i++)
    {
      att += float(ships_[i]) * shipstats[i].first;
    }
  return att;
}

//Returns the total defense of the fleet
float Fleet::totalDefense(const std::vector<std::pair<float, float> >& shipstats) const
{
  float def = 0;
  for (unsigned int i = 0; i < ships_.size(); i++)
    {
      def += float(ships_[i]) * shipstats[i].second;
    }
  return def;
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
bool Fleet::takeHit(int damage, const std::vector<std::pair<float, float> >& shipstats)
{
  //Add in any extra damage from last time
  damage += damage_;

  while (damage > 0)
    {
      //Find the next ship type in the fleet
      int start = damageTarget_;
      damageTarget_ = (damageTarget_+1)%ships_.size();
      while (ships_[damageTarget_] == 0)
	{
	  damageTarget_ = (damageTarget_+1)%ships_.size();
	  if (ships_[damageTarget_] == 0 && start == damageTarget_)
	    {
	      return false;
	    }
	}
      
      //Destroy one ship of this type, if possible
      if (damage >= shipstats[damageTarget_].second)
	{
	  ships_[damageTarget_]--;
	  
	  //There is now less damage to go around
	  damage -= shipstats[damageTarget_].second;
	}
      else //Not enough damage to destroy this ship
	{
	  //Store the remaining damage for next time
	  damage_ = damage;
	  
	  //This ship will be the target of the next hit
	  damageTarget_ = (damageTarget_-1)%ships_.size();

	  //Return true: fleet not destroyed
	  return true;
	}
    }

  //I'm not sure why the code would ever get here, but just to be sure...
  return true;
}

#endif
