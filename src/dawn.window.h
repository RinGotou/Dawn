#pragma once
#include "dawn.common.h"

namespace dawn {
  class Texture;
  struct FlipOption;

  SDL_Rect ProduceRect(int x, int y, int w, int h);
  SDL_Point ProducePoint(int x, int y);
  FlipOption ProduceFlipOption(double angle, int x, int y, SDL_RendererFlip mode);

  enum ImageType {
    kImageJPG = IMG_INIT_JPG,
    kImagePNG = IMG_INIT_PNG,
    kImageTIF = IMG_INIT_TIF,
    kImageWEBP = IMG_INIT_WEBP
  };

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

  struct FlipOption {
    double angle_value;
    SDL_RendererFlip mode;
    SDL_Point point;
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

    bool Copy(Texture &texture, 
      SDL_Rect *src_rect = nullptr, SDL_Rect *dest_rect = nullptr);
    bool Copy(Texture &texture,
      SDL_Rect *src_rect, SDL_Rect *dest_rect, FlipOption option);

    void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    }

    bool SetViewport(SDL_Rect pos);
    SDL_Rect GetViewport();


    void Clear() { SDL_RenderClear(renderer_); }
    void Present() { SDL_RenderPresent(renderer_); }
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

  using ManagedWindow = shared_ptr<BasicWindow>;

  enum ColorValueType {
    kColorRGB, kColorRGBA
  };

  struct ColorValue {
    ColorValueType type;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    ColorValue() :
      type(kColorRGB),
      r(0), g(0), b(0), a(0) {}

    ColorValue(int r, int g, int b) :
      type(kColorRGB),
      r(r), g(g), b(b), a(0) {}

    ColorValue(int r, int g, int b, int a) :
      type(kColorRGBA),
      r(r), g(g), b(b), a(a) {}
  };

  class Texture {
  protected:
    SDL_Texture *ptr_;

  public:
    virtual ~Texture() {
      SDL_DestroyTexture(ptr_);
    }

    Texture() : ptr_(nullptr) {}

    Texture(SDL_Texture *ptr) : ptr_(ptr) {}

    Texture(string path, int type, SDL_Renderer *renderer,
      bool enable_colorkey = false, ColorValue key = ColorValue()) :
      ptr_(nullptr) {
      Init(path, type, renderer, enable_colorkey, key);
    }

  public:
    bool Init(string path, int type, SDL_Renderer *renderer,
      bool enable_colorkey = false, ColorValue key = ColorValue());

    auto Get() { return ptr_; }

    bool SetColorMod(uint8_t r, uint8_t g, uint8_t b) {
      return SDL_SetTextureColorMod(ptr_, r, g, b) == 0;
    }

    bool SetBlendMode(SDL_BlendMode mode) {
      return SDL_SetTextureBlendMode(ptr_, mode) == 0;
    }

    bool SetAlpha(uint8_t a) {
      return SDL_SetTextureAlphaMod(ptr_, a) == 0;
    }
  };

  using ManagedTexture = shared_ptr<Texture>;
}
