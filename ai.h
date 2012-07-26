/*
  -----Galcon AI Class Declaration-----
  Auston Sterling
  austonst@gmail.com
  7/24/12

  Header for a class to handle AI controlled players in "Galcon"
*/

#ifndef _ai_h_
#define _ai_h_

#include <list>
#include <map>
#include "SDL/SDL.h"
#include "planet.h"
#include "fleet.h"

typedef std::list<std::pair<Planet*,std::pair<int, Planet*> > > commandList;

struct GalconAISettings
{
  //The percentage of created ships which get allocated to attacks.
  //Could be seen as the aggressiveness of the AI.
  float attackFraction;

  //The percentage of desired ships the planet must be off by to declare defecit.
  //Low values mean a lot of small adjustments over time.
  float surplusDefecitThreshold;

  //How much extra attack power is needed before attacking the target planet.
  //High numbers will make the AI very conservative, while with low numbers
  //attacks will often fail the first time around.
  float attackExtra;

  //When attacking, this percentage of ships will leave from each planet
  //High numbers expose attacking planets to counterattack, while low
  //numbers will give the enemy time to prepare.
  float perPlanetAttackStrength;

  //The delay (ms) between AI calculations when using update().
  //High numbers will improve frame rate, but make the AI less responsive
  int delay;
};

class GalconAI
{
 public:
  //Constructors
  GalconAI(char playerid, GalconAISettings setup);

  //Accessors
  char player() const {return player_;}
  bool active() const {return active_;}

  //Mutators
  void activate() {active_ = true;}
  void deactivate() {active_ = false;}
  void setPlayer(char playerid) {player_ = playerid;}

  //General use functions
  void init(std::list<Planet> & planets, const std::vector<std::pair<float, float> > & shipStats);
  commandList rebalance(const std::list<Fleet> & fleets, const std::vector<std::pair<float, float> > & shipStats);
  void computeTarget(std::list<Planet> & planets, const std::list<Fleet> & fleets, const std::vector<std::pair<float, float> > & shipStats);
  commandList attack(const std::vector<std::pair<float, float> > & shipStats);
  commandList update(std::list<Planet> & planets, const std::list<Fleet> & fleets, const std::vector<std::pair<float, float> > & shipStats);

  //Notifiers
  void notifyConstruction(float attack, float defense);
  void notifyDefendLoss(float attack);
  void notifyAttackLoss(float amount);
  void notifyPlanetLoss(Planet* loss);
  void notifyPlanetGain(Planet* gain);
  
 private:
  //The player who this AI is controlling
  char player_;

  //The planets owned
  std::list<Planet*> planets_;

  //Bool allowing the AI to take actions
  bool active_;

  //Attack and defense totals
  float attTotal_;
  float defTotal_;

  //The main target to attack
  Planet* target_;

  //The AI settings
  GalconAISettings set_;

  //The time of the last update call
  int updateTime_;
};

#endif