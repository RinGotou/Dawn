#include "dawn.h"
#include <iostream>
#include <vector>

using namespace dawn;
using namespace std;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  bool exit = false;
  SDL_Event e;
  WindowOption main_option;

  main_option.height = 720;
  main_option.width = 1280;
  main_option.title = "Playground";

  ManagedWindow win(new BasicWindow(main_option));
  ManagedFont font(new Font("C:\\workspace\\font.ttf", 30));
  ManagedTexture texture(
    new Texture("Sakura reincarnation", *font, win->GetRenderer(), ColorValue(0, 0, 0, 0)));
  auto dest = ProduceRect(0, 0, texture->GetWidth(), texture->GetHeight());


  win->SetDrawColor(255, 255, 255, 255);
  win->Clear();
  win->Copy(*texture, nullptr, &dest);
  win->Present();

  

  while (!exit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        exit = true;
      }
    }
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}