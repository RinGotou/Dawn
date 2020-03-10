#include "dawn.ui.h"
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
  main_option.flags = SDL_WINDOW_ALLOW_HIGHDPI;

  PlainWindow window(main_option);
  Texture image0("C:\\workspace\\img1.jpg", kImageJPG, window.GetRenderer());
  Texture image1("C:\\workspace\\img2.png", kImagePNG, window.GetRenderer());

  window.AddElement("image0", Element(image0, ProduceRect(0, 0, 400, 400)));
  window.AddElement("image1", Element(image1, ProduceRect(200, 200, 400, 400)));

  auto &element0 = *window.GetElementById("image0");
  auto &element1 = *window.GetElementById("image1");
  element0.SetPriority(1);

  window.DrawElements();

  auto *clicked = window.FindElementByPoint(ProducePoint(250, 250));

  if (clicked != nullptr) {
    cout << clicked->first << endl;
  }
  else {
    cout << "failed" << endl;
  }

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