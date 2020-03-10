#include "dawn.common.h"

namespace dawn {
  bool &GetAudioSubsystemState() {
    static std::mutex state_lock;
    std::lock_guard<std::mutex> guard(state_lock);
    static bool state = false;
    return state;
  }

  WindowManagementStorage &GetWindowMgmtStorage() {
    static std::mutex state_lock;
    std::lock_guard<std::mutex> guard(state_lock);
    static WindowManagementStorage storage;
    return storage;
  }

  int EnvironmentSetup(AudioOption audio) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      return -1;
    }

    if (Mix_Init(audio.flags) == 0) {
      return -2;
    }

    if (Mix_OpenAudio(
      audio.frequency, 
      audio.format, 
      audio.channels, 
      audio.chunksize) != 0) {
      return -3;
    }

    GetAudioSubsystemState() = true;

    return 0;
  }

  void EnvironmentCleanup() {
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();

    GetAudioSubsystemState() = false;

    SDL_Quit();
  }

  bool IsAudioSubsystemLoaded() {
    return GetAudioSubsystemState();
  }

  void RegisterWindow(PlainWindow *window, Uint32 id) {
    auto &storage = GetWindowMgmtStorage();
    storage.insert(std::make_pair(id, window));
  }

  bool DisposeWindow(Uint32 id) {
    auto &storage = GetWindowMgmtStorage();
    auto it = storage.find(id);
    bool result = false;

    if (it != storage.end()) {
      storage.erase(it);
      result = true;
    }

    return result;
  }

  PlainWindow *GetWindowById(Uint32 id) {
    auto &storage = GetWindowMgmtStorage();
    auto it = storage.find(id);
    if (it != storage.end()) return it->second;
    return nullptr;
  }

  wstring s2ws(const string &s) {
    if (s.empty()) return wstring();
    size_t length = s.size();
    //wchar_t *wc = (wchar_t *)malloc(sizeof(wchar_t) * (length + 2));
    wchar_t *wc = (wchar_t *)calloc(length + 64, sizeof(wchar_t));
    auto res = mbstowcs(wc, s.data(), s.length() + 1);
    wstring str(wc);
    free(wc);
    return str;
  }

  string ws2s(const wstring &s) {
    if (s.empty()) return string();
    size_t length = s.size();
    //char *c = (char *)malloc(sizeof(char) * (length + 1) * 2);
    char *c = (char *)calloc((length + 64) * 2, sizeof(char));
    auto res = wcstombs(c, s.data(), (length + 64) * 2);
    string result(c);
    free(c);
    return result;
  }
}