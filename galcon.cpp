/*
  -----Galcon Game Main Body-----
  Auston Serling
  austonst@gmail.com
  6/28/12

  Contains the main body for the game "Galcon" (Unnamed so far).
*/

#include "planet.h"
#include "fleet.h"
#include "projectile.h"
#include "vec2f.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <list>
#include <cmath>
#include <sstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LEVEL_WIDTH = 1600;
const int LEVEL_HEIGHT = 1200;
const int CAMERA_SPEED = 200;

SDL_Surface* loadImage(std::string filename)
{
  //The image that's loaded
  SDL_Surface *loadedImage = NULL;

  //The optimized surface
  SDL_Surface *optimizedImage = NULL;

  //Make all images look in images folder
  filename = "images/" + filename;

  //Load the file
  loadedImage = IMG_Load(filename.c_str());

  //If the image loaded
  if (loadedImage != NULL)
	{
	  //Create an optimized surface
	  optimizedImage = SDL_DisplayFormat(loadedImage);

	  //Free the old surface
	  SDL_FreeSurface(loadedImage);

	  //If the surface was optimized
	  if (optimizedImage != NULL)
		{
		  //Color key surface
		  SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 160, 150, 0 ) );
		}
	}
  return optimizedImage;
}

//Main function
int main(int argc, char* argv[])
{
  /*
    -----
    // INITIALIZATION
    -----
  */
  
  //Initialize all SDL subsystems
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
      return 1;
    }

  //Initialize SDL_TTF
  TTF_Init();
  TTF_Font * planetFont = TTF_OpenFont("corbel.ttf", 20);

  //Set up the screen
  SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);

  //Make sure screen set up
  if (screen == NULL)
    {
      return false;
    }

  //Set the window caption
  SDL_WM_SetCaption("GAEM", NULL);

  //Create an event manager
  SDL_Event event;

  //Store keystates
  Uint8* keystates;

  //Set up camera
  SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  float camerax = 0;
  float cameray = 0;

  /*
    -----
    GAME SETUP
    -----
  */

  //Set up ship stats
  std::vector<std::pair<float, float> > shipStats(10);
  for (int i = 0; i < 10; i++)
    {
      shipStats[i].first = i+1;
      shipStats[i].second = i+1;
    }

  //Set up buildings
  std::list<Building> buildings;
  std::vector<std::list<Building*> > buildRules;
  buildRules.resize(2);
  SDL_Surface* buildimg = loadImage("building.png");
  SDL_Surface* buildconstimg = loadImage("buildingconstruct.png");
  buildings.push_back(Building(buildimg, buildconstimg, "build 1 5"));
  buildings.push_back(Building(buildimg, buildconstimg, "fire damage 3 1"));
  std::list<Building>::iterator bi = buildings.begin();
  buildRules[0].push_back(&(*bi));
  bi->setBuildTime(10000);
  bi++;
  buildRules[1].push_back(&(*bi));
  bi->setBuildTime(5000);
  SDL_FreeSurface(buildimg);
  SDL_FreeSurface(buildconstimg);

  //Create a list of planets
  std::list<Planet> planets;

  //The standard rate of production of basic ship 0
  float ship0rate = 1.0;

  //Add a few planets
  SDL_Surface* planetimg = loadImage("planet.png");
  planets.push_back(Planet(planetimg, 1.2, Vec2f(200, 200), 1));
  planets.push_back(Planet(planetimg, 0.6, Vec2f(750, 550), 0));
  planets.push_back(Planet(planetimg, 1.0, Vec2f(1000, 900), 0));
  SDL_FreeSurface(planetimg);

  //The array of indicators
  SDL_Surface* indicator[3];
  indicator[1] = loadImage("selectorb.png");
  indicator[2] = loadImage("selectorr.png");

  //Give them some attributes
  planetIter pi = planets.begin();
  (*pi).setRotSpeed(.0314159265358979323);
  (*pi).build(&(*bi), buildRules);
  (*pi).setOwner(2, indicator);
  (*pi).setShipRate(0, ship0rate);
  bi--;
  pi++;
  (*pi).setRotSpeed(.314159265358979323);
  //(*pi).build(&(*bi), buildRules);
  //(*pi).build(&(*bi), buildRules);
  bi++;
  //(*pi).build(&(*bi), buildRules);
  (*pi).setOwner(1, indicator);
  (*pi).setShipRate(0, ship0rate);
  pi++;
  (*pi).setRotSpeed(.0628);
  (*pi).setOwner(0, indicator);
  (*pi).setShipRate(0, ship0rate);
  (*pi).setDifficulty(30);

  //Set up fleet list
  std::list<Fleet> fleets;

  //Set up projectile list
  std::list<Projectile> projectiles;

  //Filler to act as NULL
  planetIter planNull;

  //The currently selected planet
  planetIter selectPlanet = planNull;

  //The number of the locally playing player
  char localPlayer = 1;
  
  /*
    -----
    MAIN LOOP
    -----
  */

  int time = SDL_GetTicks();
  uint8_t quit = 0;
  while (quit == 0)
    {
      //Update time and dt
      int dt = SDL_GetTicks() - time;
      time = SDL_GetTicks();

      //Update keystates
      keystates = SDL_GetKeyState(NULL);

      //Check for arrow keys/wasd
      if (keystates[SDLK_UP] || keystates[SDLK_w])
	{
	  cameray -= CAMERA_SPEED * (dt/1000.0);
	  if (cameray < 0) cameray = 0;
	}
      
      if (keystates[SDLK_RIGHT] || keystates[SDLK_d])
	{
	  camerax += CAMERA_SPEED * (dt/1000.0);
	  if (camerax > LEVEL_WIDTH - SCREEN_WIDTH) camerax = LEVEL_WIDTH - SCREEN_WIDTH;
	}
      
      if (keystates[SDLK_DOWN] || keystates[SDLK_s])
	{
	  cameray += CAMERA_SPEED * (dt/1000.0);
	  if (cameray > LEVEL_HEIGHT - SCREEN_HEIGHT) cameray = LEVEL_HEIGHT - SCREEN_HEIGHT;
	}
      
      if (keystates[SDLK_LEFT] || keystates[SDLK_a])
	{
	  camerax -= CAMERA_SPEED * (dt/1000.0);
	  if (camerax < 0) camerax = 0;
	}

      //Update camera from camerax and cameray to struct
      camera.x = camerax;
      camera.y = cameray;
      
      //Handle events
      while (SDL_PollEvent(&event))
	{
	  //Quit if requested
	  if (event.type == SDL_QUIT)
	    {
	      quit = 1;
	    }

	  //Check for escape key and number presses to construct buildings
	  //BUILDING CONSTRUCTION THIS WAY IS TEMPORARY
	  if (event.type == SDL_KEYDOWN)
	    {
	      switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
		  quit = 1;
		  break;
		case SDLK_1:
		  if (selectPlanet != planNull)
		    {
		      if (selectPlanet->owner() == localPlayer)
			{
			  selectPlanet->build(&(*(buildings.begin())),buildRules);
			}
		    }
		  break;
		case SDLK_2:
		  if (selectPlanet != planNull)
		    {
		      if (selectPlanet->owner() == localPlayer)
			{
			  selectPlanet->build(&(*(++buildings.begin())),buildRules);
			}
		    }
		  break;
		default:
		  break;
		}
	    }

	  //Check for mouse clicks
	  if (event.type == SDL_MOUSEBUTTONDOWN)
	    {
	      //Left click
	      if (event.button.button == SDL_BUTTON_LEFT)
		{
		  //Used to select a planet
		  //Check if any are being clicked on
		  selectPlanet = planNull;

		  //Adjust mouse coordinates based on camera
		  Vec2f click(event.button.x + camera.x, event.button.y + camera.y);
		  
		  for (planetIter i = planets.begin(); i != planets.end(); i++)
		    {
		      //See if distance from center is less than planet radius
		      Vec2f center(i->x() + (UNSCALED_PLANET_RADIUS * i->size()),
				   i->y() + (UNSCALED_PLANET_RADIUS * i->size()));

		      if ((click-center).length() < UNSCALED_PLANET_RADIUS * i->size())
			{
			  //Ensure the planet belongs to this person
			  if ((*i).owner() == localPlayer)
			    {
			      selectPlanet = i;
			      break;
			    }
			}
		    }
		}

	      //Right click
	      if (event.button.button == SDL_BUTTON_RIGHT)
		{
		  //Used to choose the destination for a fleet
		  //See if we have a selected planet
		  if (selectPlanet != planNull)
		    {

		      //Adjust mouse coordinates based on camera
		      Vec2f click(event.button.x + camera.x, event.button.y + camera.y);
		      
		      //Check to see if any are being clicked on
		      for (planetIter i = planets.begin(); i != planets.end(); i++)
			{
			  Vec2f center(i->x() + (UNSCALED_PLANET_RADIUS * i->size()),
				       i->y() + (UNSCALED_PLANET_RADIUS * i->size()));
			  
			  //See if distance from center is less than planet radius
			  if ((click-center).length() < UNSCALED_PLANET_RADIUS * i->size())
			    {
			      //Split ships from the source planet
			      std::vector<int> transfer = (*selectPlanet).splitShips(0.5);
			      //Make sure we actually have a ship in the fleet
			      for (unsigned int j = 0; j < transfer.size(); j++)
				{
				  if (transfer[j] > 0)
				    {
				      //Add the new fleet
				      fleets.push_back(Fleet(transfer, &(*selectPlanet), &(*i)));
				      break;
				    }
				}
			    }
			}
		    }
		}
	    }
	}

      //Draw a white background
      SDL_Rect back = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
      SDL_FillRect(screen, &back, 0xFFFFFF);

      //Update and display fleets
      for (fleetIter i = fleets.begin(); i != fleets.end(); i++)
	{
	  (*i).update();

	  //See if distance from center is less than planet radius
	  Vec2f tar((*i).dest()->x() + (UNSCALED_PLANET_RADIUS*(*i).dest()->size()),
		    (*i).dest()->y() + (UNSCALED_PLANET_RADIUS*(*i).dest()->size()));
	  
	  if ((tar-i->pos()).length() < UNSCALED_PLANET_RADIUS * (i->dest())->size())
	    {
	      //Check if friendly or hostile
	      if ((*i).dest()->owner() == (*i).owner())
		{
		  //Add the fleet to the new planet
		  (*((*i).dest())).addShips((*i).ships());
		}
	      else //Hostile
		{
		  //Attack!
		  (*((*i).dest())).takeAttack((*i).ships(), (*i).owner(), shipStats, indicator);
		}

	      //Delete the fleet
	      i = fleets.erase(i);
	      i--;
	      continue;
	    }
	  
	  (*i).display(screen, camera);
	}

      //Update and display planets
      for (planetIter i = planets.begin(); i != planets.end(); i++)
	{
	  (*i).update();

	  //If this planet is selected, add an indicator
	  if (i == selectPlanet)
	    {
	      SDL_Rect temprect = {Sint16((*i).x()-10 - camera.x), Sint16((*i).y()-10 - camera.y), Uint16(UNSCALED_PLANET_RADIUS * (*i).size() * 2 + 20), Uint16(UNSCALED_PLANET_RADIUS * (*i).size() * 2 + 20)};
	      SDL_FillRect(screen, &temprect, SDL_MapRGB(screen->format, 100, 100, 100));
	    }

	  //Iterate over all buildings to handle effects from buildings to other objects

	  for (unsigned int j = 0; j < i->buildcount(); j++)
	    {
	      //Get the building
	      BuildingInstance* b = i->building(j);
	      
	      //Skip over nonexistant and incomplete buildings
	      if (!(b->exists()) || j == Uint32(i->buildIndex())) continue;
	      //Create a string stream and vector for tokens
	      std::stringstream ss(b->effect());
	      std::string item;
	      std::vector<std::string> tokens;
	      while (std::getline(ss, item, ' '))
		{
		  tokens.push_back(item);
		}
	      
	      //Ensure the size is at least two
	      if (tokens.size() < 3) continue;
	      
	      //Parse it and apply effects that involve multiple objects
	      //Fire projectile: fire <effect> <effectvars> <speed as multiplier>
	      if (tokens[0] == "fire")
		{
		  //Ensure size of four
		  if (tokens.size() != 4) continue;
		  
		  //Loop over all potential target fleets, find closest
		  Fleet* closest = NULL;
		  float closestDist = -1;
		  Vec2f coords = i->buildcoords(j);
		  for (fleetIter k = fleets.begin(); k != fleets.end(); k++)
		    {
		      //Compute the distance between them
		      double dist = (coords-k->pos()).length();
		      
		      //Continue if the fleet is out of range
		      if (dist > b->range()) continue;
		      
		      //Compare with previous best
		      if (dist < closestDist || closestDist < -0.5)
			{
			  closestDist = dist;
			  closest = &(*k);
			}
		    }
		  
		  //Fire a projectile from the building to the fleet
		  if (closest != NULL)
		    {
		      if (b->fire())
			{
			  projectiles.push_back(Projectile(coords, closest, b->effect(), std::atof(tokens[tokens.size()-1].c_str())));
			}
		    }
		}
	    }

	  (*i).display(screen, planetFont, camera);
	}

      //Update and display projectiles
      for (projectileIter i = projectiles.begin(); i != projectiles.end(); i++)
	{
	  (*i).update();
	  (*i).display(screen, camera);
	}

      //Flipoo
      if (SDL_Flip(screen) == -1)
	{
	  return 1;
	}
    }

  //Free surfaces
  SDL_FreeSurface(indicator[1]);
  SDL_FreeSurface(indicator[2]);

  //Clean up TTF
  TTF_CloseFont(planetFont);
  TTF_Quit();

  SDL_Quit();
}
