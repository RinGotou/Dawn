/*
  Common header of Dawn
*/
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <utility>
#include <memory>

#if defined(_WIN32)
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")
#if defined(_DEBUG)
#pragma comment(lib,"SDL2test.lib")
#endif
#pragma comment(lib,"SDL2_image.lib")
#pragma comment(lib,"SDL2_mixer.lib")
#pragma comment(lib,"SDL2_ttf.lib")
#else
/* Reserved for unix-like environment */
#endif

#define FRAMEWORK_ID "Dawn"

namespace dawn {
  using std::string;
  using std::shared_ptr;
  using std::make_shared;
}