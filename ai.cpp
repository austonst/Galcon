/*
  Copyright (c) 2013 Auston Sterling
  See license.txt for copying permission.
  
  -----Galcon AI Class Implementation-----
  Auston Sterling
  austonst@gmail.com

  A class to handle AI controlled players in "Galcon"
*/

#ifndef _ai_cpp_
#define _ai_cpp_

#include "ai.h"
#include <map>
#include <vector>
#include <sstream>
#include <cmath>

#include <iostream>

//Regular use constructor
GalconAI::GalconAI(char playerid, GalconAISettings setup):
  player_(playerid) {
  planets_.clear();
  active_ = false;
  attTotal_ = 0;
  defTotal_ = 0;
  target_ = NULL;
  set_ = setup;
  updateTime_ = -1;
}

//Initializes the AI. This really should be done after planets are set up,
//but before anything has been done. It can work to initialize halfway through a game,
//but it may throw off some of the longer term planning.
void GalconAI::init(std::list<Planet> & planets, const std::vector<ShipStats> & shipstats)
{
  std::cout << "Init) ";
    
  //Find which planets are owned by the player
  for (planetIter i = planets.begin(); i != planets.end(); i++)
    {
      if (i->owner() == player_)
	{
	  //Add it to the list
	  planets_.push_back(&(*i));
	}
    }
  
  //Count the number of total ships, distribute according to ratio
  attTotal_ = 0;
  defTotal_ = 0;
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      //Get the ships
      std::vector<int> ships = (*i)->shipcount();

      //Add attack and defense
      for (unsigned int j = 0; j < ships.size(); j++)
	{
	  attTotal_ += ships[j] * shipstats[j].attack * set_.attackFraction;
	  defTotal_ += ships[j] * shipstats[j].defense * (1-set_.attackFraction);
	}
    }
  std::cout << "Planets: " << planets_.size() << " Attack: " << attTotal_ << " Defense: " << defTotal_ << std::endl;
}

//Rebalances the distribution of ships on owned planets, minus the number of incoming
//enemy ships. This is for defense against attackers. Returns a list of commands to be carried out.
commandList GalconAI::rebalance(const std::list<Fleet> & fleets, const std::vector<ShipStats> & shipstats)
{
  std::cout << "Rebalance) ";
  commandList ret;

  //Find total effective defense to balance
  float effectiveDefense = 0;
  float totalSize = 0;
  std::map<Planet*, float> def;
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      //Find this planet's overall defense
      
      def[(*i)] = (*i)->totalDefense(shipstats);

      //Add or subtract incoming ships
      for (fleetIterConst j = fleets.begin(); j != fleets.end(); j++)
	{
	  if (j->dest() == *i)
	    {
	      int fleetShips = j->ships();
	      
	      if (j->owner() == player_)
		{
		  def[(*i)] += float(fleetShips) * shipstats[j->type()].attack;
		}
	      else
		{
		  def[(*i)] -= float(fleetShips) * shipstats[j->type()].attack;
		}
	    }
	}

      //Add it to the total
      effectiveDefense += def[(*i)];

      //Add the size to the total size
      totalSize += (*i)->size();
    }
  std::cout << "Effective Defense: " << effectiveDefense << " Total Size: " << totalSize << std::endl;

  //If there's a negative effective defense, for now just freeze up and pray
  if (effectiveDefense <= 0)
    {
      return ret;
    }

  //Sort planets by surplus and defecit
  std::list<Planet*> surplus;
  std::list<Planet*> defecit;
  
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      //Find defense desired
      float desired = ((*i)->size() / totalSize) * effectiveDefense;

      //If there's a surplus, add it to the surplus list
      if (def[(*i)] > desired * (1+set_.surplusDefecitThreshold))
	{
	  surplus.push_back(*i);
	}

      //If there's a defecit, add it to the defecit list
      if (def[(*i)] < desired / (1+set_.surplusDefecitThreshold))
	{
	  defecit.push_back(*i);
	}
    }

  //Go through all planets with surplus and find other planets to help out
  planetPtrIter i = surplus.begin();
  while (i != surplus.end())
    {
      //If there are no more planets with defecit, break out
      if (defecit.size() == 0) break;
      
      //Find nearest planet with defecit
      Planet* nearPlanet = NULL;
      float nearDist = -1;

      for (planetPtrIter j = defecit.begin(); j != defecit.end(); j++)
	{
	  //Don't send to self
	  if (i == j) break;

	  float dist = ((*i)->center() - (*j)->center()).length();
	  
	  if (dist < nearDist || nearPlanet == NULL)
	    {
	      nearDist = dist;
	      nearPlanet = (*j);
	    }
	}
      
      //Send a fleet with defense values adding to min(defecit, surplus)
      //to the destination planet

      //Find how much defense to send
      float sendDefense = std::min(def[(*i)]-(((*i)->size() / totalSize) * effectiveDefense),
			      ((nearPlanet->size() / totalSize) * effectiveDefense)-def[nearPlanet]);

      //Add the command to the list
      ret.push_back(std::make_pair((*i), std::make_pair(sendDefense, nearPlanet)));
      std::cout << "  Sending " << sendDefense << " from " << (*i) << " to " << nearPlanet << std::endl;

      //Adjust these planet's values
      def[(*i)] -= sendDefense;
      def[nearPlanet] += sendDefense;

      //If the destination planet is now happy, remove it from the list
      if (def[nearPlanet] > ((nearPlanet->size() / totalSize) * effectiveDefense) / (1+set_.surplusDefecitThreshold))
	{
	  for (planetPtrIter j = defecit.begin(); j != defecit.end(); j++)
	    {
	      if ((*j) == nearPlanet)
		{
		  defecit.erase(j);
		  break;
		}
	    }
	}

      //If the sending planet has no more reserves, move to the next one
      if (def[(*i)] < (((*i)->size() / totalSize) * effectiveDefense) * (1+set_.surplusDefecitThreshold))
	{
	  i++;
	}
    }

  //Now all the rebalancing commands should be in the list.
  //Return it and let the main program sort it out.
  return ret;
}

//Computes the optimal target to attack and stores the result.
void GalconAI::computeTarget(std::list<Planet> & planets, const std::list<Fleet> & fleets, const std::vector<ShipStats> & shipstats)
{
  //Store the distance from each planet to the nearest owned planet
  std::map<Planet*, float> distWeight;
  float maximin = 0;
  for (planetIter i = planets.begin(); i != planets.end(); i++)
    {
      //Don't store distance to planets you own
      if (i->owner() == player_) continue;

      //Find the closest
      float closestDist = -1;

      for (planetPtrIter j = planets_.begin(); j != planets_.end(); j++)
	{
	  //If it's closer
	  if (((*j)->center() - i->center()).length() < closestDist || closestDist == -1)
	    {
	      closestDist = ((*j)->center() - i->center()).length();
	    }
	}

      //We want the distance weighting to grow exponentially
      closestDist = pow(closestDist, set_.distancePower);

      //Add it to the map
      distWeight[&(*i)] = closestDist;

      //Update maximin if needed
      if (closestDist > maximin) maximin = closestDist;
    }
  
  //Find the best target
  Planet* bestPlanet = NULL;
  float bestRatio = -1;

  for (planetIter i = planets.begin(); i != planets.end(); i++)
    {
      //Don't attack a planet you own
      if (i->owner() == player_) continue;
      
      //Find total defense
      float defense = i->totalDefense(shipstats);

      //Take into account any fleets moving to this planet
      for (fleetIterConst k = fleets.begin(); k != fleets.end(); k++)
	{
	  //Only do stuff for fleets going to this planet
	  if (k->dest() != &(*i)) continue;

	  //Fleet owned by owner of planet
	  if (k->owner() == i->owner())
	    {
	      //Add the defense
	      defense += k->totalDefense(shipstats);
	    }
	  else if (k->owner() != i->owner() && k->owner() != player_) //Third party
	    {
	      //Subtract the attack of the fleet
	      defense -= k->totalAttack(shipstats);

	      //If the attacker would win, its ships take defense
	      if (defense < 0) defense *= -1;
	    }
	  //We don't care about our own fleets
	}

      //Add any ships that would be created during the flight
      //Ignore construction from buildings for now...
      float travelTime = pow(distWeight[&(*i)], 1.0/set_.distancePower)/float(DEFAULT_FLEET_SPEED);
      defense += travelTime * i->size();

      
      //Compute ratio
      float ratio = (defense+3) / i->size();

      //Weight it by distance
      ratio *= distWeight[&(*i)] / maximin;

      //Weight it by size (prioritizing small)
      ratio *= i->size();

      //Compare it
      if (ratio < bestRatio || bestPlanet == NULL)
	{
	  bestPlanet = &(*i);
	  bestRatio = ratio;
	}
    }

  //Store the result
  std::cout << "ComputeTarget) Targeting " << bestPlanet << std::endl;
  target_ = bestPlanet;
  return;
}

//Checks to see if it's ready to attack the target planet.
//If so, return some commands to be executed
commandList GalconAI::attack(const std::vector<ShipStats> & shipstats)
{
  //Create return commandList
  commandList ret;
  
  std::cout << "Attack) ";
  //Refuse to attack a NULL target
  if (!target_) return ret;
  
  //Find total target defense
  float defense = target_->totalDefense(shipstats);

  //Ensure at least one ship is sent each attack
  if (defense < 1) defense = 1;

  //Add one to the defense to help with attacking small planets
  defense++;

  //If there is no target planet, we cannot attack
  if (target_ == NULL) return ret;
  
  //Compare attack reserves to the defense of the target
  float attack;
  if (target_->owner() == 0)
    {
      attack = defense * (1+set_.attackExtraNeutral);
    }
  else
    {
      attack = defense * (1+set_.attackExtraEnemy);
    }
  
  std::cout << "Attackers: " << attTotal_ << " Defenders: " << attack << std::endl;
  if (attTotal_ < attack) return ret;

  //Since we have enough ships to attack, send from nearest planets
  //Copy the list of owned planets
  std::list<Planet*> unused;
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      unused.push_back(*i);
    }
  
  //While we have not met the required amount
  float currentTotal = 0;
  while (currentTotal < attack)
    {
      //If there are no unused planets left, get out
      if (unused.size() == 0) break;
      
      //Find the nearest unused planet
      Planet* nearestPlanet = NULL;
      float nearestDist = -1;

      planetPtrIter i;
      for (i = unused.begin(); i != unused.end(); i++)
	{
	  float dist = (target_->center() - (*i)->center()).length();
	  if (dist < nearestDist || nearestPlanet == NULL)
	    {
	      nearestPlanet = (*i);
	      nearestDist = dist;
	    }
	}

      //Send ships from this planet to the target
      //Find total attack potential
      float planetAttack = nearestPlanet->totalAttack(shipstats);

      //Send only the required amount
      if (currentTotal + planetAttack > attack)
	{
	  planetAttack -= currentTotal + planetAttack - attack;
	}

      //Don't send them all
      //planetAttack *= set_.perPlanetAttackStrength;

      //Send them
      std::cout << "  Sending " << planetAttack << " from " << nearestPlanet << " to " << target_ << std::endl;
      ret.push_back(std::make_pair(nearestPlanet, std::make_pair(planetAttack, target_)));

      //Increase the current total
      currentTotal += planetAttack;

      //Don't use this planet again
      for (i = unused.begin(); i != unused.end(); i++)
	{
	  if ((*i) == nearestPlanet)
	    {
	      unused.erase(i);
	      break;
	    }
	}
    }

  //Move currentTotal ships from attack to defense
  attTotal_ -= currentTotal;
  defTotal_ += currentTotal;
  std::cout << "  Attack adjusted to: " << attTotal_ << " Defense adjusted to: " << defTotal_ << std::endl;

  //Return the commands
  return ret;
}

//Starts construction of a building if the AI thinks the time is right
//Returns a comandList sending ships from a planet to itself, with the number
//of ships being the index of the requested building in buildRules
commandList GalconAI::build(const std::vector<std::list<Building*> > buildRules, const std::vector<ShipStats> & shipstats)
{
  std::cout << "Build) ";
  //Find the total build rate of the AI's planets
  float totalBuildRate = 0;
  float currentBuildRate = 0;
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      //To get a sense of the general build rate, sum attack and defense incomes
      std::vector<float> rates = (*i)->shiprate();
      for (unsigned int j = 0; j < rates.size(); j++)
	{
	  totalBuildRate += rates[j] *
	    (shipstats[j].attack + shipstats[j].defense) *
	    (*i)->size();

	  //If it is currently working on a building, add to currentBuildRate
	  if ((*i)->buildIndex() != -1)
	    {
	      currentBuildRate += rates[j] *
		(shipstats[j].attack + shipstats[j].defense) *
		(*i)->size();
	    }
	}

      //Also include production from buildings
      for (unsigned int j = 0; j < (*i)->buildcount(); j++)
	{
	  //Get the BuildingInstance
	  BuildingInstance* build = (*i)->building(j);

	  //Make sure it's currently operational
	  if (!build->exists() || int(j) == (*i)->buildIndex()) continue;

	  //Parse it
	  std::stringstream ss(build->effect());
	  std::string item;
	  std::vector<std::string> tokens;
	  while (std::getline(ss, item, ' '))
	    {
	      tokens.push_back(item);
	    }

	  //Only handle buildings that build ships
	  if (tokens[0] == "build")
	    {
	      //Must have a size of 3
	      if (tokens.size() == 3)
		{
		  //Add this building's production to the total
		  int index = atoi(tokens[1].c_str());
		  totalBuildRate += (1/atoi(tokens[2].c_str())) *
		    (shipstats[index].attack + shipstats[index].defense);
		}
	    }
	}
    }

  //Now find the total production that can be sacrificed for building construction
  float maxBuildCost = totalBuildRate * set_.maximumBuildingFraction;
  std::cout << "Total Production: " << totalBuildRate << " Max for Buildings: " << maxBuildCost << std::endl;

  //Build as much as possible
  commandList ret;
  std::list<Planet*> commanded;
  while (currentBuildRate < maxBuildCost)
    {
      //Build on the larget planet that still fits within the limit
      Planet* largestPlanet = NULL;
      float largestRate = -1;

      for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
	{
	  //Cannot select planets that are already building, full, or do
	  //not meet the minimum defense requirement
	  if ((*i)->buildIndex() != -1) continue;
	  if ((*i)->totalDefense(shipstats) < set_.minimumDefenseForBuilding) continue;
	  bool getout = false;
	  for (planetPtrIter j = commanded.begin(); j != commanded.end(); j++)
	    {
	      if ((*j) == (*i))
		{
		  getout = true;
		  break;
		}
	    }
	  if (getout) continue;
	  getout = true;
	  for (unsigned int j = 0; j < (*i)->buildcount(); j++)
	    {
	      if (!(*i)->building(j)->exists())
		{
		  getout = false;
		  break;
		}
	    }
	  if (getout) continue;
	  
	  //Find this planet's inherent build rate
	  std::vector<float> rates = (*i)->shiprate();
	  float planetBuildRate = 0;
	  for (unsigned int j = 0; j < rates.size(); j++)
	    {
	      planetBuildRate += rates[j] * (shipstats[j].attack + shipstats[j].defense);
	    }

	  //Scale by size
	  planetBuildRate *= (*i)->size();

	  //Compare it to the current best and the upper limit
	  if ((planetBuildRate > largestRate || largestPlanet == NULL) && planetBuildRate + currentBuildRate <= maxBuildCost)
	    {
	      std::cout << "  Planet " << (*i) << " has rate " << planetBuildRate << std::endl;
	      largestPlanet = (*i);
	      largestRate = planetBuildRate;
	    }
	}

      //If it's NULL, no planets are cheap enough
      if (largestPlanet == NULL) break;

      //Build something on this planet
      //For now, naievely pick at random      
      int numBuildTypes = 0;
      for (std::list<Building*>::const_iterator bi = buildRules[largestPlanet->type()].begin(); bi != buildRules[largestPlanet->type()].end(); bi++)
	{
	  numBuildTypes++;
	}
      int buildType = rand()%numBuildTypes;

      //Add the command
      ret.push_back(std::make_pair(largestPlanet, std::make_pair(buildType, largestPlanet)));
      std::cout << "  Constructing building " << buildType << " on " << largestPlanet << std::endl;
      commanded.push_back(largestPlanet);

      //Add the rate to the current build rate
      currentBuildRate += largestRate;
    }

  //Send the commands!
  return ret;
}
  
//An easy to use, do-everything-in-one-call sort of function
commandList GalconAI::update(std::list<Planet> & planets, const std::list<Fleet> & fleets, const std::vector<ShipStats> & shipstats, std::vector<std::list<Building*> > buildRules)
{
  //Set up the list of commands
  commandList rb;
  
  //See if we have waited long enough and the AI is active
  int time = SDL_GetTicks();
  if ((time - updateTime_ < set_.delay && updateTime_ != -1) || !active_) return rb;
  updateTime_ = time;
  std::cout << "Update) Attack: " << attTotal_ << " Defense: " << defTotal_ << std::endl;

  //Compute the best target
  computeTarget(planets, fleets, shipstats);

  //Get the commands from rebalancing, attacking, and building
  rb = rebalance(fleets, shipstats);
  commandList at = attack(shipstats);
  commandList bd = build(buildRules, shipstats);

  //Append at to rb
  for (commandList::iterator i = at.begin(); i != at.end(); i++)
    {
      rb.push_back(*i);
    }
  for (commandList::iterator i = bd.begin(); i != bd.end(); i++)
    {
      rb.push_back(*i);
    }

  //Return the combined list of commands
  return rb;
}

//Notify the AI that ships with the inputted totals have been constructed
void GalconAI::notifyConstruction(float attack, float defense)
{
  attTotal_ += attack * set_.attackFraction;
  defTotal_ += defense * (1-set_.attackFraction);
}

//Notify the AI that it has lost ships while defending a planet
void GalconAI::notifyDefendLoss(float attack)
{
  std::cout << "DefendLoss) " << attack << " total" << std::endl;
  if (defTotal_ >= attack)
    {
      defTotal_ -= attack;
    }
  else
    {
      attTotal_ -= attack - defTotal_;
      defTotal_ = 0;
      if (attTotal_ < 0) attTotal_ = 0;
    }
  /*
  attTotal_ -= attack * set_.attackFraction;
  defTotal_ -= attack * (1-set_.attackFraction);
  if (attTotal_ < 0) attTotal_ = 0;
  if (defTotal_ < 0) defTotal_ = 0;*/
}

//Notify the AI that it has lost ships while attacking another planet
void GalconAI::notifyAttackLoss(float amount)
{
  std::cout << "AttackLoss) " << amount << " total" << std::endl;
  defTotal_ -= amount;
  if (defTotal_ < 0) defTotal_ = 0;
}

//Notify the AI that it has lost control of a planet
void GalconAI::notifyPlanetLoss(Planet* loss)
{
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      if ((*i) == loss)
	{
	  planets_.erase(i);
	  return;
	}
    }
}

//Notify the AI that it has successfully taken a planet
void GalconAI::notifyPlanetGain(Planet* gain)
{
  //Ensure there are no duplicates
  for (planetPtrIter i = planets_.begin(); i != planets_.end(); i++)
    {
      if ((*i) == gain)
	{
	  return;
	}
    }
  planets_.push_back(gain);
}

//Notify the AI that its fleet has taken damage
void GalconAI::notifyFleetDamage(float amount)
{
  defTotal_ -= amount;
  if (defTotal_ < 0) defTotal_ = 0;
}
  
#endif
