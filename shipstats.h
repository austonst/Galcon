/*
  -----Shipstats Struct-----
  Auston Sterling
  austonst@gmail.com

  Contains the Shipstats struct for "Galcon", which stores the statistics for ship
  types.
*/

#ifndef _shipstats_h_
#define _shipstats_h_

struct ShipStats
{
  //The base attack and defense of this ship type
  float attack;
  float defense;

  //The base speed of this ship type in pixels per second
  int speed;
};

#endif
