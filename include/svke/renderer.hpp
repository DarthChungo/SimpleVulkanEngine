#ifndef SVKE_RENDERER_HPP
#define SVKE_RENDERER_HPP

#include "defines.hpp"
#include "device.hpp"
#include "pch.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

namespace svke {
  class Renderer {
   public:
    Renderer(Window &window, Device &device);
    ~Renderer();

    Renderer(const Renderer &other) = delete;
    Renderer &operator=(const Renderer &other) = delete;

   public:
    bool         isFrameInProgress() const { return pIsFrameStarted; }
    float        getAspectRatio() const { return pSwapChain->getExtentAspectRatio(); }
    VkRenderPass getSwapChainRenderPass() const { return pSwapChain->getRenderPass(); }

   public:
    VkCommandBuffer BeginFrame();
    void            EndFrame();
    void            BeginSwapChainRenderPass(VkCommandBuffer command_buffer);
    void            EndSwapChainRenderPass(VkCommandBuffer command_buffer);

   private:
    void pCreateCommandBuffers();
    void pFreeCommandBuffers();
    void pRecreateSwapChain();

   private:
    Window &                     pWindow;
    Device &                     pDevice;
    std::unique_ptr<SwapChain>   pSwapChain;
    std::vector<VkCommandBuffer> pCommandBuffer;

   private:
    uint32_t pCurrentImageIndex {0};
    uint32_t pCurrentFrameIndex {0};
    bool     pIsFrameStarted {false};
  };
}  // namespace svke

#endif
