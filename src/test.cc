#include "dawn.h"
#include <iostream>

using namespace dawn;
using namespace std;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);

  bool exit = false;
  SDL_Event e;
  WindowOption main_option;

  main_option.height = 1080;
  main_option.width = 1920;
  main_option.title = "Playground";

  BasicWindow win0(main_option);
  Texture texture1("C:\\workspace\\img0.jpg", kImageJPG, win0.GetRenderer());
  auto rect1 = ProduceRect(0, 0, 1500, 888);
  Texture texture2("C:\\workspace\\img1.jpg", kImageJPG, win0.GetRenderer());
  auto rect2 = ProduceRect(1500, 0, 400, 400);
  Texture sprites("C:\\workspace\\sprites.png", kImagePNG, win0.GetRenderer(),
    true, ColorValue(0, 0xff, 0xff));
  auto rect3 = ProduceRect(0, 0, 100, 100);
  auto rect4 = ProduceRect(100, 0, 100, 100);
  auto sp_rect = ProduceRect(0, 888, 100, 100);
  auto sp_rect2 = ProduceRect(100, 888, 100, 100);

  win0.SetDrawColor(255, 255, 255, 255);
  win0.Clear();
  win0.Copy(texture1, nullptr, &rect1);
  win0.Copy(texture2, nullptr, &rect2);
  win0.Copy(sprites, &rect3, &sp_rect);
  win0.Copy(sprites, &rect4, &sp_rect2);
  win0.Present();

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

  SDL_Quit();
  return 0;
}