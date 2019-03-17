#include "dawn.h"
#include <iostream>
#include <vector>

using namespace dawn;
using namespace std;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);

  bool exit = false;
  SDL_Event e;
  WindowOption main_option;

  main_option.height = 800;
  main_option.width = 800;
  main_option.title = "Playground";

  ManagedWindow win(new BasicWindow(main_option));
  ManagedTexture texture(new Texture("C:\\workspace\\img2.png", kImagePNG, win->GetRenderer()));
  auto flip_option = ProduceFlipOption(45, 200, 200, SDL_FLIP_HORIZONTAL);
  auto dest = ProduceRect(200, 200, 320, 320);
  win->SetDrawColor(255, 255, 255, 255);
  win->Clear();

  win->Copy(*texture, nullptr, &dest, flip_option);
  win->Present();

  while (!exit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        exit = true;
      }
    }
  }

  SDL_Quit();
  return 0;
}