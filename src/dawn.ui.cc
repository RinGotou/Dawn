#include "dawn.ui.h"

namespace dawn {
  bool Element::Draw(SDL_Renderer *renderer) {
    return SDL_RenderCopy(renderer, texture_, &src_, &dest_) == 0;
  }

  NamedElement *ElementLayer::FindNamedElementByOrder(int64_t order) {
    auto it = begin();
    for (; it != end(); it++) {
      if (it->second.order == order) break;
    }

    if (it != end()) return &(*it);

    return nullptr;
  }

  void ElementLayer::ResortVector() {
    size_t index = 0;
    for (auto it = begin(); it != end(); it++) {
      while (drawing_vec[index] != &it->second && index < drawing_vec.size()) {
        index += 1;
      }

      if (index != drawing_vec.size()) it->second.order = index;
      index = 0;
    }
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

  void PlainWindow::ClearElements() {
    elements_.clear();
    if (real_time_) Clear();
  }
  
  bool PlainWindow::DrawElements() {
    bool result = true;

    if (window_ == nullptr || renderer_ == nullptr) return false;

    Clear();

    // Draw all elements sorted by priority(from bottom to top)
    for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
      auto &drawing_vec = it->second.drawing_vec;
      for (auto it = drawing_vec.rbegin(); it != drawing_vec.rend(); it++) {
        (*it)->Draw(renderer_);
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

    if (real_time_) DrawElements();

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

    if (real_time_) DrawElements();

    return result;
  }

  bool PlainWindow::SetElementDestination(string id, SDL_Rect dest) {
    bool result = true;
    auto *element = FindElement(id);

    if (element != nullptr) {
      auto &elem_dest = element->GetDestInfo();
      elem_dest = dest;
    }
    else {
      result = false;
    }

    if (real_time_) DrawElements();

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

    if (real_time_) DrawElements();

    return result;
  }

  SDL_Point PlainWindow::GetElementPosition(string id) {
    auto *element = FindElement(id);
    SDL_Point result{ 0, 0 };

    if (element != nullptr) {
      auto &dest = element->GetDestInfo();
      result.x = dest.x;
      result.y = dest.y;
    }

    return result;
  }

  SDL_Point PlainWindow::GetElementSize(string id) {
    auto *element = FindElement(id);
    SDL_Point result{ 0, 0 };

    if (element != nullptr) {
      auto &dest = element->GetDestInfo();
      result.x = dest.w;
      result.y = dest.h;
    }

    return result;
  }

  SDL_Rect PlainWindow::GetElementDestination(string id) {
    auto *element = FindElement(id);
    SDL_Rect result{};

    if (element != nullptr) {
      result = element->GetDestInfo();
    }

    return result;
  }

  SDL_Rect PlainWindow::GetElementCropper(string id) {
    auto *element = FindElement(id);
    SDL_Rect result{};

    if (element != nullptr) {
      result = element->GetSrcInfo();
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
    auto inserted_element_it = it->second.find(id);
    it->second.drawing_vec.push_back(&(inserted_element_it->second));
    inserted_element_it->second.order = it->second.drawing_vec.size() - 1;

    if (real_time_) DrawElements();

    return true;
  }

  NamedElement *PlainWindow::FindElementByPoint(SDL_Point point) {
    NamedElement *result = nullptr;

    for (auto &layer : elements_) {
      auto &drawing_vec = layer.second.drawing_vec;
      for (size_t index = 0; index < drawing_vec.size(); index += 1) {
        if (InRange(drawing_vec[index]->GetDestInfo(), point)) {
          result = layer.second.FindNamedElementByOrder(index);
          break;
        }
      }

      if (result != nullptr) break;
    }

    return result;
  }

  bool PlainWindow::DisposeElement(string id) {
    bool result = false;
    ElementLayer::iterator it;

    for (auto &layer : elements_) {
      it = layer.second.find(id);

      if (it != layer.second.end()) {
        deque<Element *>::iterator target_it = layer.second.drawing_vec.begin();
        for (int64_t i = 0; i < it->second.order; i += 1) target_it++;
        layer.second.drawing_vec.erase(target_it);
        layer.second.erase(id);
        result = true;
        break;
      }
    }

    if (real_time_) DrawElements();

    return result;
  }

  bool PlainWindow::SetElementOnTop(string id) {
    bool result = false;
    ElementLayer::iterator it;

    for (auto &layer : elements_) {
      it = layer.second.find(id);

      if (it != layer.second.end()) {
        auto &drawing_vec = layer.second.drawing_vec;
        deque<Element *>::iterator target_it = drawing_vec.begin();
        for (int64_t i = 0; i < it->second.order; i += 1) target_it++;
        Element *selected = *target_it;
        drawing_vec.erase(target_it);
        drawing_vec.push_front(selected);
        it->second.order = 0;
        layer.second.ResortVector();
        result = true;
        break;
      }
    }

    if (real_time_) DrawElements();

    return result;
  }

  bool PlainWindow::SetElementOnBottom(string id) {
    bool result = false;
    ElementLayer::iterator it;

    for (auto &layer : elements_) {
      it = layer.second.find(id);

      if (it != layer.second.end()) {
        auto &drawing_vec = layer.second.drawing_vec;
        deque<Element *>::iterator target_it = drawing_vec.begin();
        for (int64_t i = 0; i < it->second.order; i += 1) target_it++;
        Element *selected = *target_it;
        drawing_vec.erase(target_it);
        drawing_vec.push_back(selected);
        it->second.order = drawing_vec.size() - 1;
        layer.second.ResortVector();
        result = true;
        break;
      }
    }

    if (real_time_) DrawElements();

    return result;
  }

  bool PlainWindow::SetElementTexture(string id, Texture &texture) {
    bool result = false;
    ElementLayer::iterator it;

    for (auto &layer : elements_) {
      it = layer.second.find(id);

      if (it != layer.second.end()) {
        it->second.SetTexture(texture.Get());
        result = true;
        break;
      }
    }

    if (real_time_) DrawElements();

    return result;
  }

  void PlainWindow::SetWindowTitle(string title) {
    SDL_SetWindowTitle(window_, title.data());
  }

  void ForceRefreshingAllWindow() {
    auto &storage = GetWindowMgmtStorage();
    for (auto &unit : storage) {
      unit.second->DrawElements();
    }
  }
}