/*
  -----Galcon AI Class Implementation-----
  Auston Sterling
  austonst@gmail.com
  7/24/12

  A class to handle AI controlled players in "Galcon"
*/

#ifndef _ai_cpp_
#define _ai_cpp_

#include "ai.h"
#include <map>
#include <vector>

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
void GalconAI::init(std::list<Planet> & planets, const std::vector<std::pair<float, float> > & shipStats)
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
	  attTotal_ += ships[j] * shipStats[j].first * set_.attackFraction;
	  defTotal_ += ships[j] * shipStats[j].second * (1-set_.attackFraction);
	}
    }
  std::cout << "Planets: " << planets_.size() << " Attack: " << attTotal_ << " Defense: " << defTotal_ << std::endl;
}

//Rebalances the distribution of ships on owned planets, minus the number of incoming
//enemy ships. This is for defense against attackers. Returns a list of commands to be carried out.
commandList GalconAI::rebalance(const std::list<Fleet> & fleets, const std::vector<std::pair<float, float> > & shipStats)
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
      std::vector<int> ships = (*i)->shipcount();
      def[(*i)] = 0;
      
      for (unsigned int j = 0; j < ships.size(); j++)
	{
	  def[(*i)] += ships[j] * shipStats[j].second;
	}

      //Add or subtract incoming ships
      for (fleetIterConst j = fleets.begin(); j != fleets.end(); j++)
	{
	  if (j->dest() == *i)
	    {
	      std::vector<int> fleetShips = j->ships();
	      for (unsigned int k = 0; k < fleetShips.size(); k++)
		{
		  if (j->owner() == player_)
		    {
		      def[(*i)] += float(fleetShips[k]) * shipStats[k].first;
		    }
		  else
		    {
		      def[(*i)] -= float(fleetShips[k]) * shipStats[k].first;
		    }
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
void GalconAI::computeTarget(std::list<Planet> & planets, const std::vector<std::pair<float, float> > & shipStats)
{
  //Find the best one
  Planet* bestPlanet = NULL;
  float bestRatio = -1;

  for (planetIter i = planets.begin(); i != planets.end(); i++)
    {
      //Don't attack a planet you own
      if (i->owner() == player_) continue;
      
      //Find total defense
      float defense = 0;
      std::vector<int> ships = i->shipcount();
      for (unsigned int j = 0; j < ships.size(); j++)
	{
	  defense += float(ships[j]) * shipStats[j].second;
	}
      
      //Compute ratio
      if (defense / i->size() < bestRatio || bestPlanet == NULL)
	{
	  bestPlanet = &(*i);
	  bestRatio = defense / i->size();
	}
    }

  //Store the result
  std::cout << "ComputeTarget) Targeting " << bestPlanet << std::endl;
  target_ = bestPlanet;
  return;
}

//Checks to see if it's ready to attack the target planet.
//If so, return some commands to be executed
commandList GalconAI::attack(const std::vector<std::pair<float, float> > & shipStats)
{
  std::cout << "Attack) ";
  //Find total target defense
  float defense = 0;
  std::vector<int> targetShips = target_->shipcount();
  for (unsigned int j = 0; j < targetShips.size(); j++)
    {
      defense += float(targetShips[j]) * shipStats[j].second;
    }

  //Create return commandList
  commandList ret;

  //If there is no target planet, we cannot attack
  if (target_ == NULL) return ret;
  
  //Compare attack reserves to the defense of the target
  float attack = defense * (1+set_.attackExtra);
  std::cout <<  "Defense: " << attack << " Attack: " << attTotal_ << std::endl;
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
      float planetAttack = 0;
      std::vector<int> planetShips = nearestPlanet->shipcount();
      for (unsigned int j = 0; j < planetShips.size(); j++)
	{
	  planetAttack += float(planetShips[j]) * shipStats[j].first;
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

//An easy to use, do-everything-in-one-call sort of function
commandList GalconAI::update(std::list<Planet> & planets, const std::list<Fleet> & fleets, const std::vector<std::pair<float, float> > & shipStats)
{
  //Set up the list of commands
  commandList rb;
  
  //See if we have waited long enough and the AI is active
  int time = SDL_GetTicks();
  if ((time - updateTime_ < set_.delay && updateTime_ != -1) || !active_) return rb;
  updateTime_ = time;
  
  //Compute the best target
  computeTarget(planets, shipStats);

  //Get the commands from rebalancing and attacking
  rb = rebalance(fleets, shipStats);
  commandList at = attack(shipStats);

  //Append at to rb
  for (commandList::iterator i = at.begin(); i != at.end(); i++)
    {
      rb.push_back(*i);
    }

  //Return the combined list of commands
  return rb;
}

//Notify the AI that ships with the inputted totals have been constructed
void GalconAI::notifyConstruction(float attack, float defense)
{
  std::cout << "Construction) " << attack << " attack and " << defense << " defense" << std::endl;
  attTotal_ += attack;
  defTotal_ += defense;
}

//Notify the AI that it has lost ships while defending a planet
void GalconAI::notifyDefendLoss(float attack)
{
  std::cout << "DefendLoss) " << attack << " total" << std::endl;
  attTotal_ -= attack * set_.attackFraction;
  defTotal_ -= attack * (1-set_.attackFraction);
  if (attTotal_ < 0) attTotal_ = 0;
  if (defTotal_ < 0) defTotal_ = 0;
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
  
#endif
