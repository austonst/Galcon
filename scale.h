/*
  -----Image Scaling Library Header-----
  Auston Sterling
  austonst@gmail.com
  11/3/11

  A library containing a variety of algorithms for scaling images.

  - Nearest Neighbor
  - Bilinear
  - Bicubic
  - Smooth Bresenham
*/

#ifndef _scale_h_
#define _scale_h_

#include "SDL/SDL.h"

//Nearest Neighbor
//Call given new width and height
SDL_Surface* scaleNN(SDL_Surface* inimage, int w, int h)
{return scaleNN(inimage, float(w)/float(inimage->w), float(h)/float(inimage->h));}

//Call given two ratios
SDL_Surface* scaleNN(SDL_Surface* inimage, float xratio, float yratio);

//Call given single ratio
SDL_Surface* scaleNN(SDL_Surface* inimage, float ratio)
{return scaleNN(inimage, ratio, ratio);}

//Bilinear
//Call given two ratios
SDL_Surface* scaleBL(SDL_Surface* inimage, float xratio, float yratio);

//Call given one ratio
SDL_Surface* scaleBL(SDL_Surface* inimage, float ratio)
{return scaleBL(inimage, ratio, ratio);}

//Call given new width and height
SDL_Surface* scaleBL(SDL_Surface* inimage, int w, int h)
{return scaleBL(inimage, float(w)/float(inimage->w), float(h)/float(inimage->h));}

//Bicubic
//Call given two ratios
SDL_Surface* scaleBC(SDL_Surface* inimage, float xratio, float yratio);

//Call given one ratio
SDL_Surface* scaleBC(SDL_Surface* inimage, float ratio)
{return scaleBC(inimage, ratio, ratio);}

//Call given new width and height
SDL_Surface* scaleBC(SDL_Surface* inimage, int w, int h)
{return scaleBC(inimage, float(w)/float(inimage->w), float(h)/float(inimage->h));}

#endif
