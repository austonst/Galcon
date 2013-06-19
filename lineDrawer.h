/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----LineDrawer Class Declaration-----
  Auston Sterling
  austonst@gmail.com

  Contains the declaration of the LineDrawer class, which draws (currently) Bresenham
  lines on an assigned SDL_Surface*.
*/

#ifndef _linedrawer_h_
#define _linedrawer_h_

#include "vec2f.h"
#include <SDL/SDL.h>

class lineDrawer
{
 public:
  //Constructors
  lineDrawer();
  lineDrawer(SDL_Surface* insurf);

  //Mutators
  void setSurface(SDL_Surface* insurf) {surf_ = insurf;}

  //General use functions
  void line(Vec2f p1, Vec2f p2, SDL_Color c);
  void line(Vec2f p1, Vec2f p2, SDL_Color c1, SDL_Color c2);
  
 private:
  //The SDL_Surface to draw to
  SDL_Surface* surf_;

  //Any other settings?
  //...
};

#endif
