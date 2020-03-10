#include "dawn.window.h"

namespace dawn {
  FlipOption ProduceFlipOption(double angle, int x, int y, SDL_RendererFlip mode) {
    FlipOption result = { angle, mode, SDL_Point{ x, y } };
    return result;
  }

  bool BasicWindow::Copy(Texture &texture, SDL_Rect *src_rect, SDL_Rect *dest_rect) {
    return SDL_RenderCopy(renderer_, texture.Get(), src_rect, dest_rect) == 0;
  }

  bool BasicWindow::Copy(Texture &texture, SDL_Rect *src_rect, SDL_Rect *dest_rect, 
    FlipOption option) {
    return SDL_RenderCopyEx(renderer_, texture.Get(), src_rect, dest_rect,
      option.angle_value, &option.point, option.mode) == 0;
  }

  bool BasicWindow::SetViewport(SDL_Rect pos) {
    return SDL_RenderSetViewport(renderer_, &pos) == 0;
  }

  SDL_Rect BasicWindow::GetViewport() {
    SDL_Rect result;
	SDL_RenderGetViewport(renderer_, &result);
    return result;
  }

  bool BasicWindow::InRange(SDL_Rect pos, SDL_Point point) {
    int width = 0, height = 0;
    SDL_GetWindowSize(window_, &width, &height);

    if (pos.x > width || pos.y > height) return false;
    int left = pos.x, right = pos.x + pos.w;
    int top = pos.y, bottom = pos.y + pos.h;

    bool result_x = point.x > left && point.x < right;
    bool result_y = point.y > top && point.y < bottom;
    return result_x && result_y;
  }

  int Texture::CalculateTextWidth(deque<string> &text, Font &font) {
    int result = -1;
    auto comparing = [&result](const char *value, Font &font) -> int {
      int temp;
      TTF_SizeUTF8(font.Get(), value, &temp, nullptr);
      if (temp > result) return temp;
      return result;
    };

    for (auto it = text.begin(); it != text.end(); ++it) {
      result = comparing(it->data(), font);
    }

    return result;
  }

  void Texture::WrappingSingleUnit(string unit, Uint32 wrap_length,
    Font &font, deque<string> &output) {
    string temp;
    wstring wtemp;
    wstring wunit = s2ws(unit);
    int width = -1;
    int64_t index;

    TTF_SizeUTF8(font.Get(), unit.data(), &width, nullptr);
    if (Uint32(width) <= wrap_length) {
      output.push_back(unit);
      return;
    }

    for (index = wunit.size() - 1; index >= 1; index -= 1) {
      wtemp = wunit.substr(0, index);
      temp = ws2s(wtemp);
      
      TTF_SizeUTF8(font.Get(), temp.data(), &width, nullptr);
      if (Uint32(width) <= wrap_length) {
        output.push_back(temp);
        break;
      }
    }

    //processing right-side
    wtemp = wunit.substr(index, wunit.size() - index);
    temp = ws2s(wtemp);
    WrappingSingleUnit(temp, wrap_length, font, output);
  }

  void Texture::SpiltStringByWrapLength(string text, Uint32 wrap_length, 
    Font &font, deque<string> &output) {
    deque<string> first_stage_buf;
    string temp;
    wstring wstr_temp;

    for (auto it = text.begin(); it != text.end(); ++it) {
      if (*it == '\n') {
        first_stage_buf.push_back(temp);
        temp.clear();
        continue;
      }

      temp.append(1, *it);
    }

    if (!temp.empty()) first_stage_buf.push_back(temp);
    temp.clear();
    temp.shrink_to_fit();

    if (first_stage_buf.size() == 1) {
      WrappingSingleUnit(text, wrap_length, font, output);
    }
    else if (first_stage_buf.size() > 1) {
      for (auto it = first_stage_buf.begin(); it != first_stage_buf.end(); ++it) {
        WrappingSingleUnit(*it, wrap_length, font, output);
      }
    }
  }

  bool Texture::Init(string path, int type, SDL_Renderer *renderer,
    bool enable_colorkey, ColorValue key) {
    if (ptr_ != nullptr) {
      SDL_DestroyTexture(ptr_);
    }

    IMG_Init(type);
    SDL_Surface *surface = IMG_Load(path.data());

    if (surface == nullptr) return false;

    if (enable_colorkey) {
      uint32_t value = 0;
      switch (key.type) {
      case kColorRGB:
        value = SDL_MapRGB(surface->format, key.r, key.g, key.b);
        break;
      case kColorRGBA:
        value = SDL_MapRGBA(surface->format, key.r, key.g, key.b, key.a);
        break;
      default:
        break;
      }

      if (SDL_SetColorKey(surface, 1, value) != 0) {
        return false;
      }
    }

    ptr_ = SDL_CreateTextureFromSurface(renderer, surface);
    width_ = surface->w;
    height_ = surface->h;

    SDL_FreeSurface(surface);

    return ptr_ != nullptr;
  }

  bool Texture::Init(string text, Font &font, SDL_Renderer *renderer, 
    ColorValue color, Uint32 wrap_length) {
    if (ptr_ != nullptr) {
      SDL_DestroyTexture(ptr_);
    }
    
    SDL_Surface *surface = nullptr;
    //wstring wtext = s2ws(text);
    //This function is broken in SDL_TTF 2.0.15
    //surface = TTF_RenderUTF8_Blended_Wrapped(font.Get(), text.data(), color.Get(), wrap_length);

    //Before SDL2 fix this issue, I will try my best.
    //Spilt text string by '\n' and render every line of the original text
    deque<string> spilted_string;
    string temp;

    if (wrap_length == 0) {
      for (auto it = text.begin(); it != text.end(); ++it) {
        if (*it == '\n') {
          spilted_string.push_back(temp);
          temp.clear();
          continue;
        }

        temp.append(1, *it);
      }
    }
    else {
      SpiltStringByWrapLength(text, wrap_length, font, spilted_string);
    }

    if (!temp.empty()) spilted_string.push_back(temp);
    temp.clear(); 
    temp.shrink_to_fit();
    
    if (spilted_string.size() == 1) {
      //Function without line wrapping
      surface = TTF_RenderUTF8_Blended(font.Get(), text.data(), color.Get());
      //surface = TTF_RenderUNICODE_Blended(font.Get(), (Uint16 *)wtext.data(), color.Get());
    }
    else if (spilted_string.size() > 1) {
      auto surface_width = CalculateTextWidth(spilted_string, font);
      auto surface_height = CalculateTextHeight(spilted_string, font);
      int line_height = -1;
      SDL_Surface *line_buf = nullptr;
      SDL_Rect src_rect{}, dest_rect{};

      //from https://wiki.libsdl.org/SDL_CreateRGBSurface
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      const Uint32 rmask = 0xff000000;
      const Uint32 gmask = 0x00ff0000;
      const Uint32 bmask = 0x0000ff00;
      const Uint32 amask = 0x000000ff;
#else
      const Uint32 rmask = 0x000000ff;
      const Uint32 gmask = 0x0000ff00;
      const Uint32 bmask = 0x00ff0000;
      const Uint32 amask = 0xff000000;
#endif

      surface = SDL_CreateRGBSurface(0, surface_width, surface_height, 32,
        rmask, gmask, bmask, amask);
      TTF_SizeUTF8(font.Get(), spilted_string.front().data(), nullptr, &line_height);

      for (size_t i = 0; i < spilted_string.size(); i += 1) {
        line_buf = TTF_RenderUTF8_Blended(font.Get(), spilted_string[i].data(), color.Get());
        src_rect = SDL_Rect{ 0,0,line_buf->w, line_buf->h };
        dest_rect = SDL_Rect{ 0, int(i) * line_height, line_buf->w, line_buf->h };
        SDL_BlitSurface(line_buf, &src_rect, surface, &dest_rect);
        SDL_FreeSurface(line_buf);
      }
    }
    //end.

    if (surface == nullptr) return false;

    ptr_ = SDL_CreateTextureFromSurface(renderer, surface);
    width_ = surface->w;
    height_ = surface->h;

    SDL_FreeSurface(surface);

    return ptr_ != nullptr;
  }
}