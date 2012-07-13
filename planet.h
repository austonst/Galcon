/*
  -----Planet Class Declaration-----
  Auston Sterling
  austonst@gmail.com
  11/4/12

  Header for the Planet class in "Galcon".
*/

#include "rotationcache.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "buildingInstance.h"
#include "vec2f.h"
#include <vector>
#include <map>
#include <list>

#ifndef _planet_h_
#define _planet_h_

const int NUM_PLANET_ROTATIONS = 500;
const int UNSCALED_PLANET_RADIUS = 50;

class Planet
{
 public:
  //Constructors
  Planet();
  Planet(SDL_Surface* surf, float size, Vec2f loc, int type);

  //Regular use functions
  void display(SDL_Surface* screen, TTF_Font* font, const SDL_Rect& camera);
  void update();
  bool canBuild();
  void build(Building* inbuild);
  void build(Building* inbuild, const std::vector<std::list<Building*> >& rules);
  void destroy(int index);
  void addShips(const std::vector<int>& inships);
  std::vector<int> splitShips(float ratio);
  void takeAttack(const std::vector<int>& inships, int player, const std::vector<std::pair<float, float> >& shipstats, SDL_Surface* indicator[]);

  //Accessors
  SDL_Surface* rotation(float angle = -1);
  Vec2f pos() const {return pos_;}
  double x() const {return pos_.x();}
  double y() const {return pos_.y();}
  float size() const {return size_;}
  char type() const {return type_;}
  float shipcount(int index) {return ship_[index].first;}
  std::vector<int> shipcount();
  unsigned int buildcount() const {return building_.size();}
  BuildingInstance* building(int i) {return &(building_[i]);}
  Vec2f buildcoords(int i);
  int owner() const {return owner_;}
  int buildIndex() const {return buildIndex_;}

  //Mutators
  void setImage(SDL_Surface* insurf) {rotation_ = RotationCache(insurf, NUM_PLANET_ROTATIONS);}
  void setRotSpeed(const float& inspeed) {rotspeed_ = inspeed;}
  void setSize(const float& insize) {size_ = insize;}
  void setType(const int& intype) {type_ = intype;}
  void setOwner(const int inowner, SDL_Surface* indicator[]);
  void setShipRate(int index, float rate) {ship_[index].second = rate * size_;}
  void setDifficulty(int diff) {if (owner_==0) ship_[0].first = diff;}

 private:
  //Stores the rotations of the planet
  RotationCache rotation_;

  //Current rotation, in radians
  float rot_;

  //Rotation speed of the planet, in rad/s
  float rotspeed_;

  //Location of the planet
  Vec2f pos_;

  //Size of the planet as a scalar amount from the base
  float size_;

  //Time of last progress() call
  int time_;

  //Type of planet
  char type_;
  
  //Vector of buildings on this planet
  std::vector<BuildingInstance> building_;

  //Index of which building is being built
  int buildIndex_;

  //Time passed (ms) for this building's construction
  int buildTime_;

  //Vector of pairs of ship counts and build rates, respectively
  std::vector<std::pair<float, float> > ship_;

  //The surface with text showing the total number of ships
  SDL_Surface* countImg_;

  //The actual number from last time
  int count_;

  //The number for the player who owns the planet
  int owner_;

  //The surface for the scaled owner indicator
  SDL_Surface* indicator_;
};
  
typedef std::list<Planet>::iterator planetIter;
typedef std::list<Planet>::const_iterator planetIterConst;

#endif
