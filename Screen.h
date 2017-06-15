#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "VertexColorHeader.h"
#include <SDL.h>

class Screen
{
	SDL_Surface* screen; //SDL_Surface
	float* zBuffer; //Z-buffer to detect visible surface (pixel)
public:
	Screen(const int, const int);
	void clear();
	void refresh();
	void setPixel(Vertex3D, Color);
	void setPixel(int, int, float, Color);
	void setPixel(int, int, int, Uint32);
	~Screen(){
		if(screen){
			SDL_FreeSurface(screen);
			screen = NULL;
		}
		delete[] zBuffer;
	}
};

Screen::Screen(const int width, const int height){
	if((SDL_Init(SDL_INIT_EVERYTHING)) == -1) return; //initialize SDL
	if((screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE | SDL_RESIZABLE)) == NULL) return; //set sdl videomode in software buffer and make it resizable
	zBuffer = new float [width*height];
	for (int i = 0; i < width*height; i++)
		zBuffer[i] = 0;
}

//clear the whole screen
void Screen::clear(){
	SDL_FillRect(screen, &screen->clip_rect, 0xdadada);
}

//refresh the screen
void Screen::refresh(){
	SDL_Flip(screen);
}

//pixel plot function with pixel as 3D vertex
void Screen::setPixel(Vertex3D v, Color c = {0xff, 0xff, 0xff, 0xff}){
	setPixel(ROUNDOFF(v.x), ROUNDOFF(v.y), v.z, c);
}

//pixel plot with x and y supplied differently considering depth
void Screen::setPixel(int xx, int yy, float depth, Color c = {0xff, 0xff, 0xff, 0xff}){
	int *pixmem32;
    int colour, width = screen->w, height = screen->h;
    xx=ROUNDOFF(xx); yy=ROUNDOFF(yy);
    if (xx < 0 || xx >= width || yy < 0 || yy >= height)
		return;
	if (depth > zBuffer[xx * height + yy])
		return;
	zBuffer[xx * height + yy] = depth;
    colour = SDL_MapRGB ( screen->format, 255*MIN(1,c.r), 255*MIN(1,c.g), 255*MIN(1,c.b));
    yy = yy*screen->pitch/4;
    pixmem32 = (int*) screen->pixels+yy+xx;
    *pixmem32 = colour;
}

void Screen::setPixel(int xx, int yy, int depth, Uint32 color){
	int *pixmem32, width = screen->w, height = screen->h;
    xx=ROUNDOFF(xx); yy=ROUNDOFF(yy);
    if (xx < 0 || xx >= width || yy < 0 || yy >= height)
		return;
	if (depth > zBuffer[xx * height + yy])
		return;
	zBuffer[xx * height + yy] = depth;
    yy = yy*screen->pitch/4;
    pixmem32 = (int*) screen->pixels+yy+xx;
    *pixmem32 = color;
}

#endif
