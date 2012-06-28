/*
  -----Planet Class Implementation-----
  Auston Sterling
  austonst@gmail.com
  12/25/11

  Body for the Planet class in "Galcon".
*/

#include "scale.h"
#include "planet.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "rotationcache.h"
#include "vec2f.h"
#include <cmath>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#ifndef _planet_cpp_
#define _planet_cpp_

//Default constructor
Planet::Planet()
{
  rotation_ = RotationCache();
  rot_ = 0;
  rotspeed_ = 0;
  pos_ = Vec2f(0,0);
  size_ = 1.0;
  time_ = 0;
  type_ = 0;
  building_.resize(1);
  ship_.resize(10);
  countImg_ = NULL;
  count_ = 0;
  owner_ = 0;
}

//Regular constructor
Planet::Planet(SDL_Surface* surf, float size, Vec2f loc, int type):
	    rotation_(scaleNN(surf, size), NUM_PLANET_ROTATIONS),
	    rot_(0),
	    rotspeed_(0),
	    pos_(loc),
	    size_(size),
	    time_(0),
	    type_(type),
	    buildIndex_(-1),
	    buildTime_(0),
	    countImg_(NULL),
	    count_(0),
	    owner_(0)
{
  //Figure out how many buildings this planet can hold
  float buildcount = (2 * 3.14159265358979323) / (std::asin((BUILDING_WIDTH >> 1) / (UNSCALED_PLANET_RADIUS * size_)) * 2);
  building_.resize((int)buildcount, NULL);
  ship_.resize(10);
}

//Displays the current rotation of the planet to the screen along with each building
void Planet::display(SDL_Surface* screen, TTF_Font* font, const SDL_Rect& camera)
{
  //Create rectangle
  SDL_Rect outrect;

  //For each building
  float attachAngle = 0;
  for (unsigned int i = 0; i < building_.size(); i++)
    {
      //If it exists
      if (building_[i].exists())
	{
	  float angle = attachAngle + rot_;

	  //Find radius for building
	  float rad = (UNSCALED_PLANET_RADIUS * size_) + building_[i].rotation(0)->h/5;

	  //Calculate coordinates
	  outrect.x = (std::cos(angle) * rad) + pos_.x() + (rotation_.rotation(0)->w/2) - building_[i].rotation(0)->w/2;
	  outrect.y = (std::sin(angle) * rad) + pos_.y() + (rotation_.rotation(0)->h/2) - building_[i].rotation(0)->h/2;

	  //Change by camera
	  outrect.x -= camera.x;
	  outrect.y -= camera.y;
	  
	  //Display
	  building_[i].display(Vec2f(outrect.x, outrect.y), angle + 3.14159265358979323/2, screen, (buildIndex_==Sint32(i))?false:true);
	}

      //Add to attachAngle
      attachAngle += 2 * 3.14159265358979323 / building_.size();
    }

  //Draw planet
  outrect.x = pos_.x() - camera.x;
  outrect.y = pos_.y() - camera.y;
  SDL_BlitSurface(rotation_.rotation(rot_), NULL, screen, &outrect);

  //Draw indicator
  if (owner_ != 0)
    {
      outrect.x = pos_.x() - (12 * size_) - camera.x;
      outrect.y = pos_.y() - (12 * size_) - camera.y;
      SDL_BlitSurface(indicator_, NULL, screen, &outrect);
    }

  //Draw total ship count

  //Update count image here as opposed to update() since it only needs to be done once
  //per draw, at most
  //First time through
  if (countImg_ == NULL)
    {
      //Find count
      count_ = 0;
      for (unsigned int i = 0; i < ship_.size(); i++)
	{
	  if (ship_[i].first > 0.99)
	    {
	      count_ += ship_[i].first;
	    }
	}
      
      //Create image
      std::stringstream ss;
      ss << count_;
      countImg_ = TTF_RenderText_Solid(font, ss.str().c_str(), {0,255,255});
    }
  else
    {
      //Find sum of ships
      float newCount = 0;
      for (unsigned int i = 0; i < ship_.size(); i++)
	{
	  if (ship_[i].first > 0.99)
	    {
	      newCount += ship_[i].first;
	    }
	}

      //Compare to last frame's count
      if (newCount != count_)
	{
	  //Delete old image
	  SDL_FreeSurface(countImg_);
	  std::stringstream ss;
	  ss << int(newCount);
	  countImg_ = TTF_RenderText_Solid(font, ss.str().c_str(), {0,255,255});
	}

      //Update ship count
      count_ = newCount;
    }

  //Draw ship count
  outrect.x = pos_.x() + (UNSCALED_PLANET_RADIUS * size_) - (countImg_->w/2) - camera.x;
  outrect.y = pos_.y() + (UNSCALED_PLANET_RADIUS * size_) - (countImg_->h/2) - camera.y;
  SDL_BlitSurface(countImg_, NULL, screen, &outrect);
}

//Progresses anything that needs to be progressed
void Planet::update()
{
  int dt;
  
  //See if this is the first run
  if (time_ == 0)
    {
      time_ = SDL_GetTicks();
      dt = 0;
    }
  else
    {
      //Find the change in time
      int newtime = SDL_GetTicks();
      dt = newtime - time_;
      time_ = newtime;
    }

  //Adjust the rotation amount
  rot_ += rotspeed_ * ((float)dt / 1000);

  //If the planet is controlled by a player and not constructing a building
  if (owner_ != 0 && buildIndex_ == -1)
    {
      //Update ship amounts from the planet itself
      for (unsigned int i = 0; i < ship_.size(); i++)
	{
	  ship_[i].first += ship_[i].second * ((float)dt / 1000);
	}
    }

  //If a building is under construction
  if (buildIndex_ != -1)
    {
      //Increase the build time
      buildTime_ += dt;

      //Check for completion
      if (buildTime_ >= building_[buildIndex_].buildtime())
	{
	  //Reset variables
	  buildIndex_ = -1;
	  buildTime_ = 0;
	}
    }

  //Cycle through buildings
  for (unsigned int i = 0; i < building_.size(); i++)
    {
      //Update the building
      building_[i].update();
      
      //Skip over nonexistant and incomplete buildings
      if (!building_[i].exists() || i == Uint32(buildIndex_)) continue;
      //Create a string stream and vector for tokens
      std::stringstream ss(building_[i].effect());
      std::string item;
      std::vector<std::string> tokens;
      while (std::getline(ss, item, ' '))
	{
	  tokens.push_back(item);
	}

      //Only go over effects to the planet itself, effects to other objects
      //should be handled in main().
      
      //Analyze it
      //Build ships: build <shiptype> <secondinterval>
      if (tokens[0] == "build")
	{
	  //Ensure size of 3
	  if (tokens.size() != 3) continue;

	  //Add to the ship count
	  ship_[std::atoi(tokens[1].c_str())].first += (float)dt/float(atoi(tokens[2].c_str()))/1000.0;
	}
    }
}

//Returns true if there is an open building spot, false if not
bool Planet::canBuild()
{
  for (unsigned int i = 0; i < building_.size(); i++)
    {
      if (!building_[i].exists())
	{
	  return true;
	}
    }
  return false;
}

//Builds a building on the planet regardless of whether or not it's allowed
//There must be an open spot, though
void Planet::build(Building* inbuild)
{
  //Find an open spot
  for (unsigned int i = 0; i < building_.size(); i++)
    {
      if (!building_[i].exists())
	{
	  //This building is under construction for a bit
	  buildIndex_ = i;

	  //Build it
	  building_[i] = BuildingInstance(inbuild);
	  return;
	}
    }
}

//Builds a building onthe planet so long as a building with that effect exists in
//the map of rules for this planet type.
//An empty list means anything's allowed
void Planet::build(Building* inbuild, const std::vector<std::list<Building*> >& rules)
{
  //If there is no room or a building is currently under construction, return
  if (!canBuild() || buildIndex_ != -1) {return;}
  
  //If the list is empty, simply build
  if (rules[type_].size() == 0) {build(inbuild); return;}
  
  //Cycle through all available buildings
  for (std::list<Building*>::const_iterator i = rules[type_].begin(); i != rules[type_].end(); i++)
    {
      //If the effects are the same
      if (inbuild->effect() == (*i)->effect())
	{
	  //Build it
	  build(inbuild);
	  return;
	}
    }
}

//Destroys the building at the given index
void Planet::destroy(int index)
{
  //Remove it
  building_[index].destroy();
}

//Adds the given ships to the planet
void Planet::addShips(const std::vector<int>& inships)
{
  //Copy them over
  for (unsigned int i = 0; i < inships.size(); i++)
    {
      ship_[i].first += inships[i];
    }
}

//Splits ratio% of ships off from the planet, returning them as a vector of floats
std::vector<int> Planet::splitShips(float ratio)
{
  //Make the vector
  std::vector<int> outships(ship_.size());

  //Find correct values
  for (unsigned int i = 0; i < ship_.size(); i++)
    {
      //Must be at least 1
      if (ship_[i].first > 0.999)
	{
	  //Round up
	  outships[i] = int((ship_[i].first * ratio) + .5);
	  
	  ship_[i].first -= outships[i];
	}
      else {outships[i] = 0;}
    }

  //Return
  return outships;
}

//Resolves an attack on the planet given the attacking fleet, the player, and ship stats
void Planet::takeAttack(const std::vector<int>& inships, int player, const std::vector<std::pair<float, float> >& shipstats, SDL_Surface* indicator[])
{
  //Create some new vectors for sorting
  std::vector<std::pair<int, std::pair<int, int> > > attack(ship_.size());
  std::vector<std::pair<int, std::pair<int, int> > > defense(ship_.size());

  //First is attack/defense
  //Second.first is ship count
  //Second.second is orginal index
  for (unsigned int i = 0; i < shipstats.size(); i++)
    {
      attack[i] = std::make_pair(shipstats[i].first, std::make_pair(inships[i], i));
      defense[i] = std::make_pair(shipstats[i].second, std::make_pair(ship_[i].first, i));
    }

  //Sort 'em
  std::sort(attack.begin(), attack.end(),
	    [](std::pair<int, std::pair<int, int> > arg1, std::pair<int, std::pair<int, int> > arg2) -> bool
	    {
	      return arg1.first > arg2.first;
	    });
  std::sort(defense.begin(), defense.end(),
	    [](std::pair<int, std::pair<int, int> > arg1, std::pair<int, std::pair<int, int> > arg2) -> bool
	    {
	      return arg1.first > arg2.first;
	    });

  //Resolve conflict
  int acount = attack[0].first  * attack[0].second.first;
  int dcount = defense[0].first * defense[0].second.first;
  unsigned int a = 0;
  unsigned int d = 0;
  while (a < ship_.size() && d < ship_.size())
    {
      //If either are zero, move down until we can get some more ships
      while (acount == 0 && a < shipstats.size())
	{
	  acount = attack[a].first * attack[a].second.first;
	  if (acount == 0) a++;
	}
      while (dcount == 0 && d < shipstats.size())
	{
	  dcount = defense[d].first * defense[d].second.first;
	  if (dcount == 0) d++;
	}

      //Run a check to ensure no bounds are passed
      if (a == shipstats.size()) break;
      if (d == shipstats.size()) break;
      
	  
      //If attacker is currently more powerful
      if (acount > dcount)
	{
	  //Subtract defender total
	  acount -= dcount;
	  dcount = 0;

	  //Adjust ship counts
	  attack[a].second.first = acount/attack[a].first;
	  defense[d].second.first = 0;

	  //Move to next defender
	  d++;
	}
      else if (acount < dcount) //Defender is currently more powerful
	{
	  //Subtract attacker total
	  dcount -= acount;
	  acount = 0;

	  //Adjust ship counts
	  attack[a].second.first = 0;
	  defense[d].second.first = dcount/defense[d].first;

	  //Move to next attacker
	  a++;
	}
      else //Equal counts of attackers and defenders
	{
	  //Decrease both counts to zero
	  attack[a].second.first  = 0;
	  defense[d].second.first = 0;
	  acount = 0;
	  dcount = 0;

	  //Move to next best types
	  a++;
	  d++;
	}
    }

  //Add up any extras who "didn't fight"
  while (a < attack.size())
    {
      acount += attack[a].second.first;
      a++;
    }
  while (d < defense.size())
    {
      dcount += defense[d].second.first;
      d++;
    }

  //Should be resolved now...
  //See who won
  if (acount == 0) //Defender wins, or tie
    {
      //Copy defender values to the planet's count, preserving incremental values
      for (unsigned int i = 0; i < ship_.size(); i++)
	{
	  ship_[defense[i].second.second].first = defense[i].second.first + ship_[i].first - float(int(ship_[i].first));
	}
    }
  else if (dcount == 0) //Attacker wins
    {
      //Copy attacker values to the planet's count
      for (unsigned int i = 0; i < ship_.size(); i++)
	{
	  ship_[attack[i].second.second].first = attack[i].second.first;
	}

      //Attacker now owns the planet
      setOwner(player, indicator);
    }

  attack.clear();
  defense.clear();
}

//Accesses the rotation at a given angle, or uses the current rotation
SDL_Surface* Planet::rotation(float angle)
{
  if (angle < 0)
    {
      return rotation_.rotation(rot_);
    }
  return rotation_.rotation(angle);
}

//Returns a vector of ship counts
std::vector<int> Planet::shipcount()
{
  //Make the vector
  std::vector<int> outships(ship_.size());

  //Copy values
  for (unsigned int i = 0; i < ship_.size(); i++)
    {
      outships[i] = ship_[i].first;
    }

  //Return
  return outships;
}

//Returns the coordinates of the base of a building
Vec2f Planet::buildcoords(int i)
{
  //Get the building
  BuildingInstance b = building_[i];

  //Construct the coordinates
  float angle = i*rot_;
  float rad = (UNSCALED_PLANET_RADIUS * size_) + b.rotation(0)->h/5;

  return Vec2f((std::cos(angle) * rad) + pos_.x() + (rotation_.rotation(0)->w/2),
	       (std::sin(angle) * rad) + pos_.y() + (rotation_.rotation(0)->h/2));
}

//Sets the owner and adjusts the indicator
void Planet::setOwner(const int inowner, SDL_Surface* indicator[])
{
  owner_ = inowner;
  if (owner_ != 0) indicator_ = scaleNN(indicator[owner_], size_);
}

#endif
