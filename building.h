/*
  -----Building Class Declaration-----
  Auston Sterling
  12/21/11
  austonst@gmail.com

  Contains the declaration for the Building class, for use in "Galcon"
*/

#include "SDL/SDL.h"
#include "rotationcache.h"
#include <string>

#ifndef _building_h_
#define _building_h_

const int NUM_BUILDING_ROTATIONS = 500;
const int BUILDING_WIDTH = 100;

class Building
{
 public:
  //Constructors
  Building();
  Building(SDL_Surface* surf, SDL_Surface* consSurf, std::string effect);

  //Regular use functions
  void display(int x, int y, float angle, SDL_Surface* screen, bool complete);

  //Accessors
  SDL_Surface* rotation(float angle = -1, bool complete = true);
  std::string effect() {return effect_;}
  const std::string effect() const {return effect_;}
  int buildtime() {return buildtime_;}
  int buildtime() const {return buildtime_;}
  int cd() {return cd_;}
  int cd() const {return cd_;}
  int range() {return range_;}
  int range() const {return range_;}

  //Mutators
  void setImage(SDL_Surface* surf) {image_ = RotationCache(surf, NUM_BUILDING_ROTATIONS);}
  void setConstructionImage(SDL_Surface* surf) {constructionImage_ = RotationCache(surf, NUM_BUILDING_ROTATIONS);}
  void setEffect(const std::string& effect) {effect_ = effect;}
  void setBuildTime(const int t) {buildtime_ = t;}
  void setCD(const int cd) {cd_ = cd;}
  void setRange(const int range) {range_ = range;}

 private:
  //The images for the building
  RotationCache image_;
  RotationCache constructionImage_;

  //The "effect" of the building as a string to be parsed
  //This is in form "effect1 var1 var2 ... varn and effect2 var1 var2 ... varn"
  std::string effect_;

  //The amount of time, in milliseconds, it takes to construct this building
  int buildtime_;

  //The cooldown time between shots, in milliseconds
  int cd_;

  //The maximum distance the building can fire from
  int range_;
};

#endif
