#pragma once
#include "dawn.window.h"

namespace dawn {
  /* Experimental implementation. BE CAREFUL */
  enum class ElementVariant{
    Button,
    ImageView,
    ScrollBar_Vertical,
    ScrollBar_Horizontal
  };

  struct ElementSpecification {
    ElementVariant variant;
    SDL_Texture *hover_image;
    SDL_Texture *mousedown_image;
    short view_percentage;
  };

  class Element {
  protected:
    SDL_Rect src_;
    SDL_Rect dest_;
    ElementSpecification spec_;
    int priority_;
    SDL_Texture *texture_;
    bool self_held;

  public:
    int64_t order;

  public:
    ~Element() { if (self_held) SDL_DestroyTexture(texture_); }

    Element() = delete;

    Element(Texture &texture, SDL_Rect dest) :
      src_{ 0, 0, texture.GetWidth(), texture.GetHeight() },
      dest_(dest), priority_(0), texture_(texture.Get()), order(0),
      self_held(false)
    {}

    Element(Texture &texture, SDL_Rect src, SDL_Rect dest) :
      src_(src), dest_(dest), priority_(0), texture_(texture.Get()),
      order(0), self_held(false)
    {}

    bool Draw(SDL_Renderer *renderer);

    SDL_Texture *GetTexture() { 
      return texture_; 
    }

    void SetTexture(SDL_Texture *texture) {
      texture_ = texture;
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
  using ManagedElement = shared_ptr<Element>;

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

    Element *FindElement(string id);

    bool IsElementExist(string id) { 
      return FindElement(id) != nullptr; 
    }
  public:
    virtual ~PlainWindow() {
      auto id = SDL_GetWindowID(window_);
      DisposeWindow(id);
    }

    PlainWindow() = delete;

    PlainWindow(WindowOption option) : BasicWindow(option), real_time_(false) {
      RegisterWindow(this, SDL_GetWindowID(window_));
    }

    virtual void operator=(PlainWindow &rhs) {
      std::swap(window_, rhs.window_);
      std::swap(renderer_, rhs.renderer_);
      std::swap(elements_, rhs.elements_);
      std::swap(real_time_, rhs.real_time_);
    }

    virtual void operator=(PlainWindow &&rhs) {
      this->operator=(rhs);
    }

    void ClearElements();
    bool DrawElements();
    //TODO:Update by events
    //void UpdateElementByEventType
    bool SetElementPosition(string id, SDL_Point point);
    bool SetElementSize(string id, int width, int height);
    bool SetElementDestination(string id, SDL_Rect dest);
    bool SetElementCropper(string id, SDL_Rect cropper);
    SDL_Point GetElementPosition(string id);
    SDL_Point GetElementSize(string id);
    SDL_Rect GetElementDestination(string id);
    SDL_Rect GetElementCropper(string id);
    bool ElementInRange(string id, SDL_Point point);
    bool AddElement(string id, Element &element);
    NamedElement *FindElementByPoint(SDL_Point point);
    bool DisposeElement(string id);
    bool SetElementOnTop(string id);
    bool SetElementOnBottom(string id);
    bool SetElementTexture(string id, Texture &texture);
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

    bool GetRefreshingMode() const {
      return real_time_;
    }
  };

  using ManagedPlainWindow = shared_ptr<PlainWindow>;

  void ForceRefreshingAllWindow();
}