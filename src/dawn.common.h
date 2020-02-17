/*
  Common header of Dawn
*/
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <deque>
#include <string>
#include <utility>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#ifdef _MSC_VER
#include <Windows.h>
#endif

#ifdef _DEBUG
#include <cstdio>
#endif

//for Windows(MSVC) environment
#ifdef _WIN32
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")
#pragma warning(disable:4996)
#ifdef _DEBUG
#pragma comment(lib,"SDL2test.lib")
#endif
#pragma comment(lib,"SDL2_image.lib")
#pragma comment(lib,"SDL2_mixer.lib")
#pragma comment(lib,"SDL2_ttf.lib")
#endif

#define DAWN_VERSION "0.1 Alpha"
#define DEFAULT_DEPTH 32

namespace dawn {
  using std::deque;
  using std::string;
  using std::wstring;
  using std::shared_ptr;
  using std::make_shared;
  using std::map;
  using std::pair;

  class PlainWindow;

  using WindowManagementStorage = std::unordered_map<Uint32, PlainWindow *>;
  struct AudioOption {
    int frequency;
    uint16_t format;
    int channels;
    int chunksize;
    int flags;
  };

  const AudioOption kDefaultAudioOpt = {
    44100,
    MIX_DEFAULT_FORMAT,
    MIX_DEFAULT_CHANNELS,
    2048,
    MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG
  };

  int EnvironmentSetup(AudioOption audio = kDefaultAudioOpt);
  void EnvironmentCleanup();
  bool IsAudioSubsystemLoaded();
  void RegisterWindow(PlainWindow *window, Uint32 id);
  bool DisposeWindow(Uint32 id);
  PlainWindow *GetWindowById(Uint32 id);
  WindowManagementStorage &GetWindowMgmtStorage();

  wstring s2ws(const string &s);
  string ws2s(const wstring &s);
}