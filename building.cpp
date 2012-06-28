/*
  -----Building Class Definition-----
  Auston Sterling
  12/21/11
  austonst@gmail.com

  Contains the definition for the Building class, for use in "Galcon"
*/

#include "building.h"

#ifndef _building_cpp_
#define _building_cpp_

//Default constructor, won't do much
Building::Building()
{
  effect_ = "none";
}

//Regular constructor
//The inputted SDL_Surface will be copied away and does not have to be maintained
Building::Building(SDL_Surface* surf, SDL_Surface* consSurf, std::string effect):
  image_(RotationCache(surf, NUM_BUILDING_ROTATIONS)),
  constructionImage_(RotationCache(consSurf, NUM_BUILDING_ROTATIONS)),
  effect_(effect),
  buildtime_(1000),
  cd_(2000),
  range_(1000)
{
}

//Displays the building to the given coordinates
void Building::display(int x, int y, float angle, SDL_Surface* screen, bool complete)
{
  //Blit it
  SDL_Rect outrect;
  outrect.x = x;
  outrect.y = y;
  if (complete)
    {
      SDL_BlitSurface(image_.rotation(angle), NULL, screen, &outrect);
    }
  else
    {
      SDL_BlitSurface(constructionImage_.rotation(angle), NULL, screen, &outrect);
    }
}

//Returns either the unrotated image if no args, or the rotation at the given angle
SDL_Surface* Building::rotation(float angle, bool complete)
{
  //Complete
  if (complete)
    {
      //No args, or uninformed user
      if (angle < 0)
	{
	  return image_.rotation(0);
	}
      return image_.rotation(angle);
    }
  else
    {
      //No args, or uninformed user
      if (angle < 0)
	{
	  return constructionImage_.rotation(0);
	}
      return constructionImage_.rotation(angle);
    }
}

#endif
