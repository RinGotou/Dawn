#pragma once
#include "dawn.common.h"

namespace dawn {
  class Texture;
  struct FlipOption;

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
      flags(SDL_WINDOW_ALLOW_HIGHDPI),
      title() {}
  };

  struct FlipOption {
    double angle_value;
    SDL_RendererFlip mode;
    SDL_Point point;
  };

  class BasicWindow {
  protected:
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    bool delegator_;

  public:
    virtual void Init(WindowOption option) {
      window_ = SDL_CreateWindow(
        option.title.data(),
        option.x, 
        option.y, 
        option.width,
        option.height,
        option.flags
      );
      renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED);
    }

    bool Copy(Texture &texture, 
      SDL_Rect *src_rect = nullptr, SDL_Rect *dest_rect = nullptr);
    bool Copy(Texture &texture,
      SDL_Rect *src_rect, SDL_Rect *dest_rect, FlipOption option);

    void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    }

    void SetSize(int width, int height) {
      SDL_SetWindowSize(window_, width, height);
    }

    bool SetViewport(SDL_Rect pos);
    SDL_Rect GetViewport();

    bool InRange(SDL_Rect pos, SDL_Point point);


    void Clear() { SDL_RenderClear(renderer_); }
    void Present() { SDL_RenderPresent(renderer_); }
    auto GetRenderer() { return renderer_; }
    auto GetWindow() { return window_; }
    auto GetId() { return SDL_GetWindowID(window_); }
    void Hide() { SDL_HideWindow(window_); }
    void Show() { SDL_ShowWindow(window_); }
  public:
    virtual ~BasicWindow() {
      if (!delegator_) {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
      }
    }

    BasicWindow() = delete;

    BasicWindow(WindowOption option) {
      Init(option);
      delegator_ = false;
    }

    virtual void operator=(BasicWindow &rhs) {
      std::swap(window_, rhs.window_);
      std::swap(renderer_, rhs.renderer_);
      std::swap(delegator_, rhs.delegator_);
    }

    virtual void operator=(BasicWindow &&rhs) {
      this->operator=(rhs);
    }

    BasicWindow(SDL_Window *window, SDL_Renderer *renderer) :
      window_(window), renderer_(renderer), delegator_(true) {}
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

    ColorValue(SDL_Color color) :
      type(kColorRGBA),
      r(color.r), g(color.g), b(color.b), a(color.a) {}

    SDL_Color Get() {
      SDL_Color result = { r,g,b,a };
      return result;
    }
  };


  //TODO:delegator mode
  class Font {
  protected:
    TTF_Font *ptr_;

  public:
    virtual ~Font() {
      if (TTF_WasInit()) {
        TTF_CloseFont(ptr_);
      }
    }

    Font() = delete;

    Font(string path, int size) : ptr_(nullptr) {
      TTF_Init();
      ptr_ = TTF_OpenFont(path.data(), size);
    }

    TTF_Font *Get() { return ptr_; }

    bool Good() const { return ptr_ != nullptr; }
  };

  class Texture {
  protected:
    SDL_Texture *ptr_;
    int width_;
    int height_;

  protected:
    int CalculateTextWidth(deque<string> &text, Font &font);

    int CalculateTextHeight(deque<string> &text, Font &font) {
      int line_height;
      TTF_SizeUTF8(font.Get(), text.front().data(), nullptr, &line_height);
      return int(text.size()) * line_height;
    }

    void WrappingSingleUnit(string line, Uint32 wrap_length,
      Font &font, deque<string> &output);

    void SpiltStringByWrapLength(string text, Uint32 wrap_length, 
      Font &font, deque<string> &output);

  public:
    virtual ~Texture() {
      if (ptr_ != nullptr) SDL_DestroyTexture(ptr_);
    }
    
    Texture() : ptr_(nullptr), width_(0), height_(0) {}

    Texture(string path, int type, SDL_Renderer *renderer,
      bool enable_colorkey = false, ColorValue key = ColorValue()) :
      ptr_(nullptr), width_(-1), height_(-1) {
      Init(path, type, renderer, enable_colorkey, key);
    }

    Texture(string text, Font &font, SDL_Renderer *renderer, 
      ColorValue color, Uint32 wrap_length = 0) :
      ptr_(nullptr), width_(-1), height_(-1) {
      Init(text, font, renderer, color, wrap_length);
    }

    Texture(Texture &rhs) {
      std::swap(ptr_, rhs.ptr_);
      std::swap(width_, rhs.width_);
      std::swap(height_, rhs.height_);
    }

    Texture(Texture &&rhs) {
      std::swap(ptr_, rhs.ptr_);
      std::swap(width_, rhs.width_);
      std::swap(height_, rhs.height_);
    }

  public:
    bool Init(string path, int type, SDL_Renderer *renderer,
      bool enable_colorkey = false, ColorValue key = ColorValue());

    bool Init(string text, Font &font, SDL_Renderer *renderer, 
      ColorValue color, Uint32 wrap_length = 0);

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

    void operator=(Texture &rhs) {
      std::swap(ptr_, rhs.ptr_);
      std::swap(width_, rhs.width_);
      std::swap(height_, rhs.height_);
    }

    void operator=(Texture &&rhs) {
      std::swap(ptr_, rhs.ptr_);
      std::swap(width_, rhs.width_);
      std::swap(height_, rhs.height_);
    }

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
  };

  using ManagedFont = shared_ptr<Font>;
  using ManagedTexture = shared_ptr<Texture>;
}
