/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----Image Rotation Cache Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  Contains the implementation of the RotationCache class.
  This is the non-threaded version, fully compatible with C++03.
*/
#ifndef _rotationcache_cpp_
#define _rotationcache_cpp_

#include "rotationcache.h"
#include "SDL/SDL.h"

#include <cmath> //REMOVE ONCE EXTERNAL ROTATION

//Default constructor, very bad!
RotationCache::RotationCache()
{
  size_ = 1;
  interval_ = 2 * rotatePi;
  SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 5, 5, 32, 0, 0, 0, 0);
  rotation_ = &temp;
}

//Regular constructor
//Takes in a pointer to a SDL_Surface and the size of the array
//This is essentially a modified resize
RotationCache::RotationCache(SDL_Surface* surf, int insize)
{
  //Allocate new array
  rotation_ = new SDL_Surface*[insize];

  //Verify they are all NULL
  for (int i = 0; i < insize; i++)
    {
      rotation_[i] = NULL;
    }

  //Copy surf to new location
  rotation_[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, surf->w, surf->h, surf->format->BitsPerPixel, 0, 0, 0, 0);

  SDL_Rect outrect = {0, 0, Uint16(surf->w), Uint16(surf->h)};
  SDL_FillRect(rotation_[0], &outrect, ROTATION_BACKGROUND_COLOR);
  SDL_BlitSurface(surf, NULL, rotation_[0], NULL);

  //Set size and set interval
  size_ = insize;
  findInterval();
}

//Copy constructor
//Calls resize, so it will erase all rotations
RotationCache::RotationCache(const RotationCache& cache)
{
  size_ = 0;
  resize(cache.size(), cache.rotation(0));
}

//Copy assignment operator
RotationCache& RotationCache::operator=(const RotationCache& cache)
{
  size_ = 0;
  resize(cache.size(), cache.rotation(0));
  return *this;
}
  
RotationCache::~RotationCache()
{ 
  for (int i = 0; i < size_; i++)
    {
      if (rotation_[i] != NULL)
	{
	  SDL_FreeSurface(rotation_[i]);
	}
    }
  delete[] rotation_;
}

//The main function for image rotation.
//Takes a pointer to a SDL_Surface, the rotation amount in radians,
//and a Uint32 for the background color.
//Returns a pointer to the rotated SDL_Surface.
//If the rotation would be negligible, returns NULL.
SDL_Surface* RotationCache::rotateImage(SDL_Surface* inimage, float angle, Uint32 color)
{
  //Bring down to range [-PI, PI]
  while (angle > 3.14159265358979323) {angle -= 3.14159265358979323 * 2;}
  while (angle < -3.14159265358979323) {angle += 3.14159265358979323 * 2;}
  
  //Check for negligible rotation
  if (angle < 0.00000001 && angle > -0.00000001) {return NULL;}
  
  //Create a new surface
  SDL_Surface* outimage = SDL_CreateRGBSurface(SDL_SWSURFACE, inimage->w, inimage->h, inimage->format->BitsPerPixel, 0, 0, 0, 0);
  SDL_Rect outrect = {0, 0, Uint16(inimage->w), Uint16(inimage->h)};
  SDL_FillRect(outimage, &outrect, 0xA09600);
  color = SDL_MapRGB(inimage->format, 160, 150, 0);
  
  //Lock surfaces
  if (SDL_MUSTLOCK(inimage)) {SDL_LockSurface(inimage);}
  if (SDL_MUSTLOCK(outimage)) {SDL_LockSurface(outimage);}

  //Extract pixels
  Uint32* inpixels = (Uint32*)inimage->pixels;
  Uint32* outpixels = (Uint32*)outimage->pixels;

  int end = outimage->w * outimage->h;

  //Calculate rotation matrix
  float rm[4];
  rm[0] = cos(angle);
  rm[2] = sin(angle);
  rm[1] = -rm[2];
  rm[3] = rm[0];

  //Cycles through all outpixels
  for (int i = 0; i < end; i++)
    {
      //Find X,Y
      int y = i / outimage->w;
      int x = i % outimage->w;

      //Adjust origin
      y -= outimage->h / 2;
      x -= outimage->w / 2;

      //Flip over X axis
      y *= -1;

      //Multiply by RM
      int x2 = (rm[0]*x) + (rm[1]*y);
      int y2 = (rm[2]*x) + (rm[3]*y);
      
      //Move origin back and reflip
      y2 *= -1;
      y2 += outimage->h / 2;
      x2 += outimage->w / 2;

      //Find "i" for these coordinates
      int source = (y2 * outimage->w) + x2;
      
      //Verify bounds
      if ((x2 > -1) && (x2 < inimage->w) && (y2 > -1) && (y2 < inimage->h))
	{
	  //Copy over pixel
	  outpixels[i] = inpixels[source];
	}
      else //Source is out of bounds
	{
	  //Fill with background color
	  outpixels[i] = color;
	}
    }

  //Unlock surfaces
  if(SDL_MUSTLOCK(inimage)) {SDL_UnlockSurface(inimage);}
  if(SDL_MUSTLOCK(outimage)) {SDL_UnlockSurface(outimage);}

  //Color key the new image if needed
  if (inimage->flags & SDL_SRCCOLORKEY)
    {
      SDL_SetColorKey(outimage, SDL_RLEACCEL | SDL_SRCCOLORKEY, inimage->format->colorkey);
    }

  //Return the SDL_Surface*
  return outimage;
}

//Finds the background color by copying the top left pixel,
//then runs the main algorithm.
SDL_Surface* RotationCache::rotateImage(SDL_Surface* inimage, float angle)
{
  //Stores the color
  Uint32 color;

  //Unlock surface if needed
  if (SDL_MUSTLOCK(inimage)) {SDL_LockSurface(inimage);}

  //Copy out top left pixel
  Uint32* inpixels = (Uint32*)inimage->pixels;
  color = (inpixels[0]);

  //Lock surface if needed
  if(SDL_MUSTLOCK(inimage)) {SDL_UnlockSurface(inimage);}

  //Call main function
  return rotateImage(inimage, angle, color);
}

//Resizes the structure to the inputted integer
//Recaclulates the interval, erases all stored rotations, and reallocates memory
void RotationCache::resize(int insize, SDL_Surface* surf)
{
  //Check for resize with no new surface
  int i = 0;
  SDL_Surface* oldsurf;
  
  if (surf == NULL)
    {
      oldsurf = rotation_[0];
      i = 1;
    }
  
  //Erase the current rotations
  while (i < size_)
    {
      if (rotation_[i] != NULL)
	{
	  SDL_FreeSurface(rotation_[i]);
	}
      i++;
    }

  if (size_ > 1)
    {
      delete[] rotation_;
    }

  //Allocate new array
  rotation_ = new SDL_Surface*[insize];

  //Verify they are all NULL
  for (i = 0; i < insize; i++)
    {
      rotation_[i] = NULL;
    }

  //Copy the inputted surface or re-insert the old one
  if (surf == NULL)
    {
      rotation_[0] = oldsurf;
    }
  else
    {
      rotation_[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, surf->w, surf->h, surf->format->BitsPerPixel, 0, 0, 0, 0);
      SDL_Rect outrect = {0, 0, Uint16(surf->w), Uint16(surf->h)};
      SDL_FillRect(rotation_[0], &outrect, ROTATION_BACKGROUND_COLOR);
      SDL_BlitSurface(surf, NULL, rotation_[0], NULL);
      SDL_SetColorKey(rotation_[0], SDL_SRCCOLORKEY, SDL_MapRGB( rotation_[0]->format, 160, 150, 0 ) );
    }

  //Set new size and set interval
  size_ = insize;
  findInterval();
}

//Returns a pointer to the SDL_Surface closest to the given angle
//If it does not exist, it is created immediately
//For small images, this isn't bad, as the workload will never be too large
//For larger images, try to find time to precache
SDL_Surface* RotationCache::rotation(float angle)
{
  //Bring down to proper range
  while (angle > (2 * 3.14159265358979323)) {angle -= 3.14159265358979323 * 2;}
  while (angle < 0) {angle += 3.14159265358979323 * 2;}
  
  //Caclulate closest index
  float index = angle/interval_;

  //Round to nearest index
  index = int(index + .5) + .01;

  //Ensure that we haven't gone too far
  if (index > size_)
    {
      index = size_ - 0.9;
    }

  //If that rotation does not exist, calculate it
  if (rotation_[(int)index] == NULL)
    {
      compute((int)index);
    }

  //Return it
  return rotation_[(int)index];
}

//Returns a pointer to the SDL_Surface closest to the given angle
//The const version will always find the closest, never creating a new rotation
//For small images, this isn't bad, as the workload will never be too large
//For larger images, try to find time to precache
SDL_Surface* RotationCache::rotation(float angle) const
{//Bring down to proper range
  while (angle > (2 * 3.14159265358979323)) {angle -= 3.14159265358979323 * 2;}
  while (angle < 0) {angle += 3.14159265358979323 * 2;}
  
  //Caclulate closest index
  float index = angle/interval_;

  //Round to nearest index
  index = int(index + .5) + .01;

  //Ensure that we haven't gone too far
  if (index > size_)
    {
      index = size_ - 0.9;
    }

  //Expand outward until a suitable rotation is found
  for (int i = (int)index, j = (int)index; i > -1; i++, j++)
    {
      if (rotation_[i] != NULL)
	{
	  return rotation_[i];
	}
      else if (j < size_)
	{
	  if (rotation_[j] != NULL)
	    {
	      return rotation_[j];
	    }
	}
    }
  
  //If somehow we got past that last step, return rotation_[0]
  return rotation_[0];
}

//Computes the rotation using rotateImage and stores it in the cache
void RotationCache::compute(int index)
{
  //Ensure nothing gets double-caclulated
  if (rotation_[index] != NULL)
    {
      return;
    }
  
  //Compute the required angle
  float angle = float(index) * interval_;

  //Rotate!
  rotation_[index] = rotateImage(rotation_[0], angle);
  SDL_SetColorKey(rotation_[index], SDL_SRCCOLORKEY, SDL_MapRGB( rotation_[index]->format, 160, 150, 0 ) );
}

void RotationCache::compute(float angle)
{
  //Bring down to proper range
  while (angle > (2 * 3.14159265358979323)) {angle -= 3.14159265358979323 * 2;}
  while (angle < 0) {angle += 3.14159265358979323 * 2;}
  
  //Caclulate closest index
  float index = angle/interval_;

  //Round to nearest index
  index = int(index + .5) + .01;

  //Ensure that we haven't gone too far
  if (index > size_)
    {
      index = size_ - 0.9;
    }

  compute((int)index);
}

//Computes "count" rotations, skipping over rotations which are already calculated
//Will return false if anything is left to be calculated afterwards
//Returns true if the structure is now full (or was full to begin with)
bool RotationCache::precache(int count)
{
  int i;
  
  //Go until count is empty or the end is reached
  for (i = 0; i < size_ && count > 0; i++)
    {
      //If this one's not done yet
      if (rotation_[i] == NULL)
	{
	  //Compute it
	  compute(i);
	  count--;
	}
    }

  //Return the proper result
  if (i == size_ - 1)
    {
      return true;
    }
  return false;
}

//Calculates the interval from size_ and stores it in interval_
void RotationCache::findInterval()
{
  interval_ = (2 * rotatePi) / (float)size_;
}

#endif
