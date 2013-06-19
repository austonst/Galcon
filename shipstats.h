/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
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

  //The interception stats of this ship type
  int interceptRange;
  float interceptDamage;
  int interceptCD;
};

#endif
