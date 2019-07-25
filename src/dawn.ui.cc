#include "dawn.ui.h"

namespace dawn {
  bool PlainWindow::DrawSingleElement(Element &element) {
    return SDL_RenderCopy(renderer_, element.GetTexture(),
      &element.GetSrcInfo(), &element.GetDestInfo()) == 0;
  }

  Element *PlainWindow::FindElement(string id) {
    Element *result = nullptr;
    ElementLayer::iterator it;

    for (auto &layer : elements_) {
      it = layer.second.find(id);

      if (it != layer.second.end()) {
        result = &(it->second);
        break;
      }
    }

    return result;
  }
  
  bool PlainWindow::DrawElements() {
    bool result = true;

    // Draw all elements sorted by priority(from bottom to top)
    for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
      for (auto &element : it->second) {
        DrawSingleElement(element.second);
      }
    }

    // Refresh window to show drawing result
    Present();

    return result;
  }

  bool PlainWindow::SetElementPosition(string id, SDL_Point point) {
    bool result = true;
    auto *element = FindElement(id);

    if (element != nullptr) {
      auto &dest = element->GetDestInfo();
      dest.x = point.x;
      dest.y = point.y;
    }
    else {
      result = false;
    }

    return result;
  }

  bool PlainWindow::SetElementSize(string id, int width, int height) {
    bool result = true;
    auto *element = FindElement(id);

    if (element != nullptr) {
      auto &dest = element->GetDestInfo();
      dest.w = width;
      dest.h = height;
    }
    else {
      result = false;
    }

    return result;
  }

  bool PlainWindow::SetElementCropper(string id, SDL_Rect cropper) {
    bool result = true;
    auto *element = FindElement(id);

    if (element != nullptr) {
      auto &src = element->GetSrcInfo();
      src = cropper;
    }
    else {
      result = false;
    }

    return result;
  }

  bool PlainWindow::ElementInRange(string id, SDL_Point point) {
    bool result = true;
    auto *element = FindElement(id);

    if (element != nullptr) {
      result = InRange(element->GetDestInfo(), point);
    }
    else {
      result = false;
    }

    return result;
  }

  bool PlainWindow::AddElement(string id, Element &element) {
    int priority = element.GetPriority();
    auto it = elements_.find(priority);

    if (it == elements_.end()) {
      elements_.insert(std::make_pair(priority, ElementLayer()));
      it = elements_.find(priority);
    }

    auto element_it = it->second.find(id);

    if (element_it != it->second.end()) return false;

    it->second.insert(std::make_pair(id, element));

    return true;
  }

  NamedElement *PlainWindow::FindElementByPoint(SDL_Point point) {
    NamedElement *result = nullptr;

    for (auto &layer : elements_) {
      for (auto &element : layer.second) {
        if (InRange(element.second.GetDestInfo(), point)) {
          result = &element;
        }
      }
    }

    return result;
  }
}