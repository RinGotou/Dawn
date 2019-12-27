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

  void RegisterWindow(BasicWindow *window, Uint32 id) {
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

  BasicWindow *GetWindowById(Uint32 id) {
    auto &storage = GetWindowMgmtStorage();
    auto it = storage.find(id);
    if (it != storage.end()) return it->second;
    return nullptr;
  }
}