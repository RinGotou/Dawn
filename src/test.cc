#include "dawn.window.h"
#include "dawn.sound.h"

#include <iostream>
#include <vector>

using namespace dawn;
using namespace std;

int main(int argc, char **argv) {
  EnvironmentSetup();

  bool exit = false;
  SDL_Event e;
  WindowOption main_option;

  main_option.height = 720;
  main_option.width = 1280;
  main_option.title = "Playground";

  ManagedWindow win(new BasicWindow(main_option));
  ManagedFont font(new Font("C:\\workspace\\font.ttf", 30));
  ManagedTexture texture(
    new Texture("Playing:Moving Go On", *font, win->GetRenderer(), ColorValue(0, 0, 0, 0)));
  auto dest = ProduceRect(0, 0, texture->GetWidth(), texture->GetHeight());
  ManagedMusic music(new Music("C:\\workspace\\moving_go_on.mp3"));
  
  win->SetDrawColor(255, 255, 255, 255);
  win->Clear();
  win->Copy(*texture, nullptr, &dest);
  win->Present();
  music->Play();

  while (!exit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        exit = true;
      }
    }
  }

  EnvironmentCleanup();
  return 0;
}