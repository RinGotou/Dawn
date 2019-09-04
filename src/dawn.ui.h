#pragma once
#include "dawn.window.h"

namespace dawn {
  /* Experimental implementation. BE CAREFUL */

  /* Basic UI element class */
  class Element {
  protected:
    SDL_Rect src_;
    SDL_Rect dest_;
    int priority_;
    SDL_Texture *texture_;

  public:
    Element() = delete;

    Element(Texture &texture, SDL_Rect dest) :
      src_(ProduceRect(0, 0, texture.GetWidth(), texture.GetHeight())),
      dest_(dest), priority_(0), texture_(texture.Get()) 
    {}

    Element(Texture &texture, SDL_Rect src, SDL_Rect dest) :
      src_(src), dest_(dest), priority_(0), texture_(texture.Get())
    {}

    SDL_Texture *GetTexture() { 
      return texture_; 
    }

    SDL_Rect &GetSrcInfo() {
      return src_;
    }

    SDL_Rect &GetDestInfo() {
      return dest_;
    }

    bool SetPriority(int priority) { 
      if (priority < 0)return false;
      priority_ = priority; 
      return true;
    }

    int GetPriority() const { return priority_; }
  };

  using ElementLayer = map<string, Element>;
  using ElementMap = map<int, ElementLayer>;
  using NamedElement = pair<const string, Element>;

  class PlainWindow : public BasicWindow {
  protected:
    ElementMap elements_;

    bool DrawSingleElement(Element &element);

    Element *FindElement(string id);

    bool IsElementExist(string id) { 
      return FindElement(id) != nullptr; 
    }
  public:
    PlainWindow() : BasicWindow() {}

    PlainWindow(WindowOption option) : BasicWindow(option) {}

    bool DrawElements();
    bool SetElementPosition(string id, SDL_Point point);
    bool SetElementSize(string id, int width, int height);
    bool SetElementCropper(string id, SDL_Rect cropper);
    bool ElementInRange(string id, SDL_Point point);
    bool AddElement(string id, Element &element);
    NamedElement *FindElementByPoint(SDL_Point point);

    bool AddElement(string id, Element &&element) { 
      return AddElement(id, element); 
    }

    Element *GetElementById(string id) { 
      return FindElement(id); 
    }
  };

  using ManagedPlainWindow = shared_ptr<PlainWindow>;
}