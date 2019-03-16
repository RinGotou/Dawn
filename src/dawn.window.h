#pragma once
#include "dawn.common.h"

namespace dawn {

  struct WindowOption {
    int width;
    int height;
    int x;
    int y;
    uint32_t flags;
    string title;

    WindowOption() :
      width(1024),
      height(768),
      x(SDL_WINDOWPOS_CENTERED),
      y(SDL_WINDOWPOS_CENTERED),
      flags(),
      title() {}
  };

  class BasicWindow {
  public:
    virtual void Init(WindowOption option) {
      window_ = SDL_CreateWindow(
        option.title.c_str(),
        option.x, 
        option.y, 
        option.width,
        option.height,
        option.flags
      );
      renderer_ = SDL_CreateRenderer(window_, -1, 
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC);
    }

    SDL_Texture *ProduceTextureFromRenderer(string path);
    bool Present(SDL_Texture *texture, const SDL_Rect *src_rect = nullptr,
      const SDL_Rect *dest_rect = nullptr);
    bool SetViewport(SDL_Rect &pos);
    SDL_Rect GetViewport();

    auto GetRenderer() { return renderer_; }
    auto GetWindow() { return window_; }
  public:
    virtual ~BasicWindow() {
      SDL_DestroyRenderer(renderer_);
      SDL_DestroyWindow(window_);
    }

    BasicWindow() :
      window_(nullptr),
      renderer_(nullptr) {}

    BasicWindow(WindowOption option) {
      Init(option);
    }

  protected:
    SDL_Window *window_;
    SDL_Renderer *renderer_;
  };
}
