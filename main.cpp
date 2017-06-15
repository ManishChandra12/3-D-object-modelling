#include "Object.h"
#include "Transformation.h"
#include <SDL.h>

int main( int argc, char *argv[]){
    int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
    bool quit = false;
    Vertex3D cam(0, 0, 20), viewPlane(0,0,0);
	LightSource light({0, 100, 0},{1, 0, 0});
    Vertex3D camcopy = cam;
    SDL_Event event;
    RenderObject pitch("cricket.obj");
    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) quit = true;
            if(event.type == SDL_VIDEORESIZE){
                SCREEN_WIDTH = event.resize.w;  SCREEN_HEIGHT = event.resize.h;
            }
        }
        Uint8* keys = SDL_GetKeyState(0);
        if (keys[SDLK_LEFT]) pitch.rotate(RADIAN(0), RADIAN(0), RADIAN(-2),light);
        if (keys[SDLK_RIGHT]) pitch.rotate(RADIAN(0), RADIAN(0), RADIAN(2),light);
        if (keys[SDLK_UP]) pitch.rotate(RADIAN(2), RADIAN(0), RADIAN(0),light);
        if (keys[SDLK_DOWN]) pitch.rotate(RADIAN(-2), RADIAN(0), RADIAN(0),light);
        if(keys[SDLK_c]) pitch.rotate(RADIAN(0), RADIAN(2), RADIAN(0),light);
        if(keys[SDLK_v]) pitch.rotate(RADIAN(0), RADIAN(-2), RADIAN(0),light);
        if(keys[SDLK_l]) pitch.scale(1.5);
        if(keys[SDLK_k]) pitch.scale(0.75);
        if(keys[SDLK_t]) pitch.translate({1.0,1.0,1.0});
        if(keys[SDLK_a]) cam.x -= 4;
        if(keys[SDLK_d]) cam.x += 4;
        if(keys[SDLK_s]) cam.y -= 4;
        if(keys[SDLK_w]) cam.y += 4;
        if(keys[SDLK_z]) cam.z += 4;
        if(keys[SDLK_x]) cam.z -= 4;

        pitch.gouraudFill(SCREEN_WIDTH, SCREEN_HEIGHT, cam, viewPlane,light);

    }
    SDL_Quit();
    return 0;
}
