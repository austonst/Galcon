/*
  -----Image Scaling Library Implementation-----
  Auston Sterling
  austonst@gmail.com
  2/3/12

  A library containing a variety of algorithms for scaling images.
*/

#ifndef _scale_cpp_
#define _scale_cpp_

#include "SDL/SDL.h"

//Nearest Neighbor scaling, taking in a pointer to the source surface
//and the scaling ratios, returning the new surface, which must
//be manually cleaned up by the user.
SDL_Surface* scaleNN(SDL_Surface* inimage, float xratio, float yratio)
{
  //Verify ratios
  if (xratio < 0.00001 || yratio < 0.00001) {return NULL;}
  
  //Calculate X and Y for new surface
  int x = inimage->w*xratio;
  int y = inimage->h*yratio;

  //Find inverses of ratios
  float invxr = 1/xratio;
  float invyr = 1/yratio;
  
  //Create the new surface
  SDL_Surface* outimage = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, inimage->format->BitsPerPixel, 0, 0, 0, 0);

  //Lock surfaces
  if (SDL_MUSTLOCK(inimage)) {SDL_LockSurface(inimage);}
  if (SDL_MUSTLOCK(outimage)) {SDL_LockSurface(outimage);}

  //Extract pixels
  Uint32* inpixels = (Uint32*)inimage->pixels;
  Uint32* outpixels = (Uint32*)outimage->pixels;

  //Cycle through all pixels in the output image
  for (int j = 0; j < y; j++)
    {
      for (int i = 0; i < x; i++)
	{
	  outpixels[(j*x)+i] = inpixels[int((int(j*invyr)*inimage->w)+int(i*invxr))];
	}
    }

  //Unlock surfaces
  if(SDL_MUSTLOCK(inimage)) {SDL_UnlockSurface(inimage);}
  if(SDL_MUSTLOCK(outimage)) {SDL_UnlockSurface(outimage);}

  //Color key the new image if needed
  if (inimage->flags & SDL_SRCCOLORKEY)
    {
      SDL_SetColorKey(outimage, SDL_RLEACCEL | SDL_SRCCOLORKEY, inimage->format->colorkey);
    }
  
  //Return the SDL_Surface*
  return outimage;
}

//Bilinear scaling function, taking in a source image, scaling ratios,
//and returning a pointer to a new surface.
SDL_Surface* scaleBL(SDL_Surface* inimage, float xratio, float yratio)
{
  //Verify ratios
  if (xratio < 0.00001 || yratio < 0.00001) {return NULL;}
  
  //Calculate X and Y for new surface
  int x = inimage->w*xratio;
  int y = inimage->h*yratio;

  //Find inverses of ratios
  float invxr = 1/xratio;
  float invyr = 1/yratio;
  
  //Create the new surface
  SDL_Surface* outimage = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, inimage->format->BitsPerPixel, 0, 0, 0, 0);

  //Lock surfaces
  if (SDL_MUSTLOCK(inimage)) {SDL_LockSurface(inimage);}
  if (SDL_MUSTLOCK(outimage)) {SDL_LockSurface(outimage);}

  //Extract pixels
  Uint32* inpixels = (Uint32*)inimage->pixels;
  Uint32* outpixels = (Uint32*)outimage->pixels;

  //Reusable vars
  float xorig;
  float yorig;
  int xfloor;
  int yfloor;
  Uint32 pix[2][2];
  Uint8 rgb[5][3];
  float xpart1;
  float xpart2;
  float ypart1;
  float ypart2;
  

  //Cycle through all pixels in the output image
  for (int j = 0; j < y; j++)
    {
      for (int i = 0; i < x; i++)
	{
	  //Find corresponding point in source
	  xorig = float(i) * invxr;
	  yorig = float(j) * invyr;

	  //Find top left pixel coordinates
	  xfloor = int(xorig);
	  yfloor = int(yorig);

	  //Store pixel values themselves
	  pix[0][0] = inpixels[(yfloor * inimage->w) + xfloor];
	  pix[1][0] = inpixels[(yfloor * inimage->w) + xfloor + 1];
	  pix[0][1] = inpixels[((yfloor + 1) * inimage->w) + xfloor];
	  pix[1][1] = inpixels[((yfloor + 1) * inimage->w) + xfloor + 1];

	  //Extract RGB values
	  rgb[0][0] = Uint8(pix[0][0] >> 16);
	  rgb[0][1] = Uint8(pix[0][0] >> 8);
	  rgb[0][2] = Uint8(pix[0][0] >> 0);
	  rgb[1][0] = Uint8(pix[1][0] >> 16);
	  rgb[1][1] = Uint8(pix[1][0] >> 8);
	  rgb[1][2] = Uint8(pix[1][0] >> 0);
	  rgb[2][0] = Uint8(pix[0][1] >> 16);
	  rgb[2][1] = Uint8(pix[0][1] >> 8);
	  rgb[2][2] = Uint8(pix[0][1] >> 0);
	  rgb[3][0] = Uint8(pix[1][1] >> 16);
	  rgb[3][1] = Uint8(pix[1][1] >> 8);
	  rgb[3][2] = Uint8(pix[1][1] >> 0);

	  //Calculate intermediate values
	  xpart1 = xfloor - xorig + 1;
	  xpart2 = xorig - xfloor;
	  ypart1 = yfloor - yorig + 1;
	  ypart2 = yorig - yfloor;

	  rgb[4][0] = (rgb[0][0] * xpart1 * ypart1) +
	              (rgb[1][0] * xpart2 * ypart1) +
	              (rgb[2][0] * xpart1 * ypart2) +
     	              (rgb[3][0] * xpart2 * ypart2);
	  rgb[4][1] = (rgb[0][1] * xpart1 * ypart1) +
	              (rgb[1][1] * xpart2 * ypart1) +
	              (rgb[2][1] * xpart1 * ypart2) +
     	              (rgb[3][1] * xpart2 * ypart2);
	  rgb[4][2] = (rgb[0][2] * xpart1 * ypart1) +
	              (rgb[1][2] * xpart2 * ypart1) +
	              (rgb[2][2] * xpart1 * ypart2) +
     	              (rgb[3][2] * xpart2 * ypart2);

	  //Copy it over to the destination image
	  outpixels[(j * x) + i] = SDL_MapRGB(outimage->format,
					     rgb[4][0], rgb[4][1], rgb[4][2]);
	}
    }
  
  //Unlock surfaces
  if(SDL_MUSTLOCK(inimage)) {SDL_UnlockSurface(inimage);}
  if(SDL_MUSTLOCK(outimage)) {SDL_UnlockSurface(outimage);}

  //Color key the new image if needed
  if (inimage->flags & SDL_SRCCOLORKEY)
    {
      SDL_SetColorKey(outimage, SDL_RLEACCEL | SDL_SRCCOLORKEY, inimage->format->colorkey);
    }
  
  //Return the SDL_Surface*
  return outimage;
}

//Helper function, computes the value of the color at the given ratio between the
//two other points
SDL_Color bspline(SDL_Color p1, SDL_Color p2, SDL_Color p3, SDL_Color p4, float t)
{
  //Fill in spline formula
  int r = ((1-t)*(1-t)*(1-t)*p1.r)/6 +
    ((3*t*t*t) - (6*t*t) + 4)*p2.r/6 +
    ((-3*t*t*t) + (3*t*t) + (3*t) + 1)*p3.r/6 +
    (t*t*t*p4.r)/6;
  int g = ((1-t)*(1-t)*(1-t)*p1.g)/6 +
    ((3*t*t*t) - (6*t*t) + 4)*p2.g/6 +
    ((-3*t*t*t) + (3*t*t) + (3*t) + 1)*p3.g/6 +
    (t*t*t*p4.g)/6;
  int b = ((1-t)*(1-t)*(1-t)*p1.b)/6 +
    ((3*t*t*t) - (6*t*t) + 4)*p2.b/6 +
    ((-3*t*t*t) + (3*t*t) + (3*t) + 1)*p3.b/6 +
    (t*t*t*p4.b)/6;

  //Copy values out and return
  SDL_Color ret;
  ret.r = r; ret.g = g; ret.b = b;
  return ret;
}

//Converts a Uint32 in form ARGB to a SDL_Color
SDL_Color toColor(Uint32 incolor)
{
  //Create the SDL_Color
  SDL_Color outcolor;

  //Copy over values
  outcolor.r = incolor >> 16;
  outcolor.g = incolor >> 8;
  outcolor.b = incolor >> 0;

  //Return it
  return outcolor;
}

//Bicubic scale
SDL_Surface* scaleBC(SDL_Surface* inimage, float xratio, float yratio)
{
  //Verify ratios
  if (xratio < 0.00001 || yratio < 0.00001) {return NULL;}
  
  //Calculate X and Y for new surface
  int x = inimage->w*xratio;
  int y = inimage->h*yratio;

  //Find inverses of ratios
  float invxr = 1/xratio;
  float invyr = 1/yratio;
  
  //Create the new surface
  SDL_Surface* outimage = SDL_CreateRGBSurface(SDL_SWSURFACE, x, y, inimage->format->BitsPerPixel, 0, 0, 0, 0);

  //Lock surfaces
  if (SDL_MUSTLOCK(inimage)) {SDL_LockSurface(inimage);}
  if (SDL_MUSTLOCK(outimage)) {SDL_LockSurface(outimage);}

  //Extract pixels
  Uint32* inpixels = (Uint32*)inimage->pixels;
  Uint32* outpixels = (Uint32*)outimage->pixels;

  //Other variables
  float xorig, yorig;
  int xfloor, yfloor;

  //Cycle through all pixels in the output image
  for (int j = 0; j < y; j++)
    {
      for (int i = 0; i < x; i++)
	{
	  //Find corresponding point in source
	  xorig = float(i) * invxr;
	  yorig = float(j) * invyr;

	  //Find top left pixel coordinates
	  xfloor = int(xorig);
	  yfloor = int(yorig);

	  //Call bspline on what's needed
	  //Calls within calls to handle bilinearity
	  SDL_Color newcolor;
	  newcolor = bspline(bspline(toColor(inpixels[((yfloor-1<0?0:yfloor-1) * inimage->w) + (xfloor-1<0?0:xfloor-1)]),
				     toColor(inpixels[((yfloor-1<0?0:yfloor-1) * inimage->w) + (xfloor<0?0:xfloor)]),
				     toColor(inpixels[((yfloor-1<0?0:yfloor-1) * inimage->w) + (xfloor+1>=inimage->h?inimage->h-1:xfloor+1)]),
				     toColor(inpixels[((yfloor-1<0?0:yfloor-1) * inimage->w) + (xfloor+2>=inimage->h?inimage->h-1:xfloor+2)]), xorig-xfloor),
			     bspline(toColor(inpixels[((yfloor<0?0:yfloor) * inimage->w) + (xfloor-1<0?0:xfloor-1)]),
				     toColor(inpixels[((yfloor<0?0:yfloor) * inimage->w) + (xfloor<0?0:xfloor)]),
				     toColor(inpixels[((yfloor<0?0:yfloor) * inimage->w) + (xfloor+1>=inimage->h?inimage->h-1:xfloor+1)]),
				     toColor(inpixels[((yfloor<0?0:yfloor) * inimage->w) + (xfloor+2>=inimage->h?inimage->h-1:xfloor+2)]), xorig-xfloor),
			     bspline(toColor(inpixels[((yfloor+1>=inimage->w?inimage->w-1:yfloor+1) * inimage->w) + (xfloor-1<0?0:xfloor-1)]),
				     toColor(inpixels[((yfloor+1>=inimage->w?inimage->w-1:yfloor+1) * inimage->w) + (xfloor<0?0:xfloor)]),
				     toColor(inpixels[((yfloor+1>=inimage->w?inimage->w-1:yfloor+1) * inimage->w) + (xfloor+1>=inimage->h?inimage->h-1:xfloor+1)]),
				     toColor(inpixels[((yfloor+1>=inimage->w?inimage->w-1:yfloor+1) * inimage->w) + (xfloor+2>=inimage->h?inimage->h-1:xfloor+2)]), xorig-xfloor),
			     bspline(toColor(inpixels[((yfloor+2>=inimage->w?inimage->w-1:yfloor+2) * inimage->w) + (xfloor-1<0?0:xfloor-1)]),
				     toColor(inpixels[((yfloor+2>=inimage->w?inimage->w-1:yfloor+2) * inimage->w) + (xfloor<0?0:xfloor)]),
				     toColor(inpixels[((yfloor+2>=inimage->w?inimage->w-1:yfloor+2) * inimage->w) + (xfloor+1>=inimage->h?inimage->h-1:xfloor+1)]),
				     toColor(inpixels[((yfloor+2>=inimage->w?inimage->w-1:yfloor+2) * inimage->w) + (xfloor+2>=inimage->h?inimage->h-1:xfloor+2)]), xorig-xfloor),
			     yorig-yfloor);

	  //Write the output pixel
	  outpixels[(j * x) + i] = SDL_MapRGB(outimage->format, newcolor.r, newcolor.g, newcolor.b);
	}
    }

  //Unlock surfaces
  if(SDL_MUSTLOCK(inimage)) {SDL_UnlockSurface(inimage);}
  if(SDL_MUSTLOCK(outimage)) {SDL_UnlockSurface(outimage);}

  //Color key the new image if needed
  if (inimage->flags & SDL_SRCCOLORKEY)
    {
      SDL_SetColorKey(outimage, SDL_RLEACCEL | SDL_SRCCOLORKEY, inimage->format->colorkey);
    }
  
  //Return the SDL_Surface*
  return outimage;
}

#endif
