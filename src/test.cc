#include "dawn.h"

using namespace dawn;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);

  bool exit = false;
  SDL_Event e;
  WindowOption main_option;
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = 1500;
  rect.h = 888;
  main_option.height = 1280;
  main_option.width = 1920;
  main_option.title = "Playground";

  BasicWindow win0(main_option);
  win0.SetViewport(rect);

  IMG_Init(IMG_INIT_JPG);
  auto texture = win0.ProduceTextureFromRenderer("C:\\workspace\\img0.jpg");
  win0.Present(texture);

  while (!exit) {
    while (SDL_PollEvent(&e) == 1) {
      switch (e.type) {
      case SDL_QUIT:
        exit = true;
        break;
      default:
        break;
      }
    }
  }
  

  SDL_DestroyTexture(texture);

  SDL_Quit();
  return 0;
}