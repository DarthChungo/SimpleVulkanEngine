#ifndef SVKE_WINDOW_HPP
#define SVKE_WINDOW_HPP

#include "pch.hpp"

namespace svke {
  class Window {
   public:
    Window(uint32_t width, uint32_t height, const std::string& win_name);
    ~Window();

    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;

    bool ShouldClose();
    void PollEvents();

    VkExtent2D getExtent() { return {pWidth, pHeight}; }

    friend class Device;

   private:
    void pCreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

   private:
    uint32_t pWidth;
    uint32_t pHeight;

    GLFWwindow* pWindow;
    std::string pWindowName;
  };
}

#endif