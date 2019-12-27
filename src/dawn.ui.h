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
    int64_t order;

  public:
    Element() = delete;

    Element(Texture &texture, SDL_Rect dest) :
      src_(ProduceRect(0, 0, texture.GetWidth(), texture.GetHeight())),
      dest_(dest), priority_(0), texture_(texture.Get()), order(0)
    {}

    Element(Texture &texture, SDL_Rect src, SDL_Rect dest) :
      src_(src), dest_(dest), priority_(0), texture_(texture.Get()),
      order(0)
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

  //using ElementLayer = map<string, Element>;
  using NamedElement = pair<const string, Element>;

  class ElementLayer : public map<string, Element> {
  public:
    deque<Element *> drawing_vec;

    NamedElement *FindNamedElementByOrder(int64_t order);
    void ResortVector();
  };

  using ElementMap = map<int, ElementLayer>;

  class PlainWindow : public BasicWindow {
  protected:
    ElementMap elements_;
    bool real_time_;

    bool DrawSingleElement(Element &element);
    Element *FindElement(string id);

    bool IsElementExist(string id) { 
      return FindElement(id) != nullptr; 
    }
  public:
    PlainWindow() = delete;

    PlainWindow(WindowOption option) : BasicWindow(option), real_time_(false) {}

    bool DrawElements();
    bool SetElementPosition(string id, SDL_Point point);
    bool SetElementSize(string id, int width, int height);
    bool SetElementCropper(string id, SDL_Rect cropper);
    SDL_Point GetElementPosition(string id);
    bool ElementInRange(string id, SDL_Point point);
    bool AddElement(string id, Element &element);
    NamedElement *FindElementByPoint(SDL_Point point);
    bool DisposeElement(string id);
    bool SetElementOnTop(string id);
    bool SetElementOnBottom(string id);
    void SetWindowTitle(string title);

    bool AddElement(string id, Element &&element) { 
      return AddElement(id, element); 
    }

    Element *GetElementById(string id) { 
      return FindElement(id); 
    }

    void RealTimeRefreshingMode(bool value) {
      real_time_ = value;
    }
  };

  using ManagedPlainWindow = shared_ptr<PlainWindow>;
}