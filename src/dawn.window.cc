#include "dawn.window.h"

namespace dawn {
  SDL_Texture *BasicWindow::ProduceTextureFromRenderer(string path) {
    SDL_Surface *surface = IMG_Load(path.c_str());
    SDL_Texture *result = nullptr;

    if (surface == nullptr) return result;

    result = SDL_CreateTextureFromSurface(renderer_, surface);

    SDL_FreeSurface(surface);

    return result;
  }

  bool BasicWindow::Present(SDL_Texture *texture, const SDL_Rect *src_rect,
    const SDL_Rect *dest_rect) {
    SDL_RenderClear(renderer_);
    bool result = SDL_RenderCopy(renderer_, texture, src_rect, dest_rect);
    SDL_RenderPresent(renderer_);
    return result == 0;
  }

  bool BasicWindow::SetViewport(SDL_Rect &pos) {
    return SDL_RenderSetViewport(renderer_, &pos) == 0;
  }

  SDL_Rect BasicWindow::GetViewport() {
    SDL_Rect result;
    SDL_RenderGetViewport(renderer_, &result);
    return result;
  }
}