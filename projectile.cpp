/*
  -----Projectile Class Definition-----
  Auston Sterling
  4/4/12
  austonst@gmail.com

  Contains the definition of the Projectile class.
*/

#ifndef _projectile_cpp_
#define _projectile_cpp_

#include <cmath>
#include "projectile.h"

//Default constructor; avoid
Projectile::Projectile() {};

//Regular constructor
Projectile::Projectile(int xin, int yin, Fleet* dest, std::string effect, float speed):
  x_(xin),
  y_(yin),
  dest_(dest),
  speed_(DEFAULT_PROJECTILE_SPEED*speed),
  effect_(effect),
  lastTicks_(0) {}

//Updates the position of the projectile
void Projectile::update()
{
  //First frame
  if (lastTicks_ == 0)
    {
      lastTicks_ = SDL_GetTicks();
      return;
    }

  //Find change in time
  int newtime = SDL_GetTicks();
  int dt = newtime - lastTicks_;
  lastTicks_ = newtime;

  //Move projectile towards destination
  //Find target coordinates
  float tarx = dest_->x();
  float tary = dest_->y();

  //Find percentage of total distance traveled this time step
  float dist = (speed_*(dt/1000.0))/std::sqrt(((tarx-startx_)*(tarx-startx_)) + ((tary-starty_)*(tary-starty_)));

  //Change location based on that
  x_ += (tarx-startx_) * dist;
  y_ += (tary-starty_) * dist;
}

//Displays the projectile
void Projectile::display(SDL_Surface* screen, const SDL_Rect& camera)
{
  //For now, just draw a rectangle
  SDL_Rect rect = {Sint16(x_ - 5 - camera.x), Sint16(y_ - 5 - camera.y), 10, 10};
  SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
}

#endif
