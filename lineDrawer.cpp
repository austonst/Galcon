/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----LineDrawer Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  Contains the implementation of the LineDrawer class, which draws (currently) Bresenham
  lines on an assigned SDL_Surface*.
*/

#ifndef _linedrawer_cpp_
#define _linedrawer_cpp_

#include "lineDrawer.h"
#include <cmath>

//Default constructor
LineDrawer::LineDrawer() : surf_(NULL)
{}

//Better constructor
LineDrawer::LineDrawer(SDL_Surface* insurf) : surf_(insurf)
{}

//Draws a line on the SDL_Surface from p1 to p2 with color c
void LineDrawer::line(Vec2f p1, Vec2f p2, SDL_Color c)
{
  //Find if the surface is longer in X or Y direction
  bool steep = abs(p2.y() - p1.y()) > abs(p2.x() - p1.x());
  if (steep)
    {
      float swapper = p2.y();
      p2.sety(p2.x());
      p2.setx(swapper);
      swapper = p1.y();
      p1.sety(p1.x());
      p1.setx(swapper);
    }

  //Ensure the line increases from p1 to p2
  if (p1.x() > p2.x())
    {
      float swapper = p2.y();
      p2.sety(p1.y());
      p1.sety(swapper);
      swapper = p2.x();
      p2.setx(p1.x());
      p1.setx(swapper);
    }

  //Find the change in x and y and initialize variables
  int dx = p2.x() - p1.x();
  int dy = abs(p2.y() - p1.y());
  int error = dx/2;
  int ystep = (p1.y() < p2.y())?1:-1;
  int y = p1.y();

  //Set up the SDL_Surface
  if (SDL_MUSTLOCK(surf_)) SDL_LockSurface(surf_);
  Uint32* pixels = (Uint32*)surf_->pixels;

  //Loop over line
  for (int x = p1.x(); x < p2.x(); x++)
    {
      //Plot the point
      if (!steep)
	{
	  pixels[x + y*surf_->w] = SDL_MapRGB(surf_->format, c.r, c.g, c.b);
	}
      else
	{
	  pixels[y + x*surf_->w] = SDL_MapRGB(surf_->format, c.r, c.g, c.b);
	}

      //Modify the error
      error -= dy;
      if (error < 0)
	{
	  y += ystep;
	  error += dx;
	}
    }

  //Unlock the surface if needed
  if (SDL_MUSTLOCK(surf_)) SDL_UnlockSurface(surf_);
}

//Draws a line on the SDL_Surface from p1 (with color c1) to p2 (with color c2)
void LineDrawer::line(Vec2f p1, Vec2f p2, SDL_Color c1, SDL_Color c2)
{
  //Find if the surface is longer in X or Y direction
  bool steep = abs(p2.y() - p1.y()) > abs(p2.x() - p1.x());
  if (steep)
    {
      float swapper = p2.y();
      p2.sety(p2.x());
      p2.setx(swapper);
      swapper = p1.y();
      p1.sety(p1.x());
      p1.setx(swapper);
    }

  //Ensure the line increases from p1 to p2
  if (p1.x() > p2.x())
    {
      float swapper = p2.y();
      p2.sety(p1.y());
      p1.sety(swapper);
      swapper = p2.x();
      p2.setx(p1.x());
      p1.setx(swapper);
      SDL_Color colorswap = c1;
      c1 = c2;
      c2 = colorswap;
    }

  //Find the change in x and y and initialize variables
  int dx = p2.x() - p1.x();
  int dy = abs(p2.y() - p1.y());
  int error = dx/2;
  int ystep = (p1.y() < p2.y())?1:-1;
  int y = p1.y();

  //Find the change in color per x step
  float cr, cg, cb;
  cr = c1.r;
  cg = c1.g;
  cb = c1.b;
  
  float dcr, dcg, dcb;
  if (dx != 0)
    {
      dcr = float(c2.r - c1.r) / float(dx);
      dcg = float(c2.g - c1.g) / float(dx);
      dcb = float(c2.b - c1.b) / float(dx);
    }
  else
    {
      dcr = dcg = dcb = 0;
    }

  //Set up the SDL_Surface
  if (SDL_MUSTLOCK(surf_)) SDL_LockSurface(surf_);
  Uint32* pixels = (Uint32*)surf_->pixels;

  //Loop over line
  for (int x = p1.x(); x < p2.x(); x++)
    {
      //Plot the point
      if (!steep)
	{
	  pixels[x + y*surf_->w] = SDL_MapRGB(surf_->format, cr, cg, cb);
	}
      else
	{
	  pixels[y + x*surf_->w] = SDL_MapRGB(surf_->format, cr, cg, cb);
	}

      //Modify the error
      error -= dy;
      if (error < 0)
	{
	  y += ystep;
	  error += dx;
	}

      //Modify color
      cr += dcr;
      cg += dcg;
      cb += dcb;
    }

  //Unlock the surface if needed
  if (SDL_MUSTLOCK(surf_)) SDL_UnlockSurface(surf_);
}

#endif
