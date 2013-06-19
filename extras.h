/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----Extra Functions File-----
  Auston Sterling
  austonst@gmail.com

  Contains extra functions which don't have a better place to be.
*/
#include "projectile.h"

//Takes in the overall list of projectiles and fleets and a planet to check
void fireAll(std::list<Projectile> & projectiles, const std::list<Fleet> & fleets, Planet* planet)
{
  //For each building
  for (unsigned int i = 0; i < planet->buildsize(); i++)
    {
      //Create a string stream and vector for tokens
      std::stringstream ss(building_[i].effect());
      std::string item;
      std::vector<std::string> tokens;
      while (std::getline(ss, item, ' '))
	{
	  tokens.push_back(item);
	}
      
      //If it can fire
      if (planet->building(i).canFire())
	{
	  //Calculate coordinates of building
	  SDL_Rect coords = planet->buildcoords(i);
	  
	  //If there is a target fleet in range
	  fleetIterConst target;
	  for (target = fleets.begin(); target != fleets.end(); target++)
	    {
	      if (sqrt((planet->building(i).x() * planet->building(i).x()) +
		       (planet->building(i).y() * planet->building(i).y()))
		  < planet->building(i).range())
		{
		  //Fire the projectile
		  planet->building(i).fire();
		  
		  //Create a new projectile
		  projectiles.push_back();
		}
	    }
	}
    }
}
