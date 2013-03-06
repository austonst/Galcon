/*
  Image Rotation Cache Class Declaration
  Auston Sterling
  austonst@gmail.com
  12/8/11

  Works with rotateImage to store rotations of a given
  SDL_Surface, caching them in case the same rotation is called later.
  This is the non-threaded version, fully compatible with C++03
*/
#ifndef _rotationcache_h_
#define _rotationcache_h_

#include "SDL/SDL.h"

const float rotatePi = 3.14159265358979323;

const unsigned int ROTATION_BACKGROUND_COLOR = 0xA09600;

class RotationCache
{
 public:
  //Default constructor, a very bad thing in this case
  //Sets it up for only one rotation
  RotationCache();

  //Regular constructor
  //Takes in a pointer to a SDL_Surface and a size for precision
  //The original SDL_Surface will be copied over, and can be deleted by the user
  RotationCache(SDL_Surface* surf, int insize);

  //Copy constructor
  RotationCache(const RotationCache& cache);

  //Assignment operator
  RotationCache& operator=(const RotationCache& cache);

  //Destructor
  ~RotationCache();

  //The rotation function, can be accessed by the outside for people who care
  //SHOULD BE REPLACED WITH LIBRARY HEADER AT SOME POINT
  SDL_Surface* rotateImage(SDL_Surface* inimage, float angle, Uint32 color);
  SDL_Surface* rotateImage(SDL_Surface* inimage, float angle);

  //Resizes the structure, erasing all currently cached rotations
  void resize(int insize, SDL_Surface* surf = NULL);

  //Accessors
  SDL_Surface* rotation(float angle);
  SDL_Surface* rotation(float angle) const;
  int size() {return size_;}
  int size() const {return size_;}

  //Computes and stores the rotation for a given index (int) or angle (float)
  void compute(int index);
  void compute(float angle);

  //Computes and stores the given number of rotations
  //Meant to be called bit by bit in times of low computational requirements
  bool precache(int count);

 private:
  //Helper function to compute the interval using the stored size
  void findInterval();
  
  //Pointer to the array of pointers to SDL_Surfaces
  SDL_Surface** rotation_;

  //Size of the array
  int size_;

  //Rotation interval, in radians, between stored images
  float interval_;
};

#endif
