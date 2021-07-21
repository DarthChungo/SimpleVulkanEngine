#ifndef SVKE_APPLICATION_HPP
#define SVKE_APPLICATION_HPP

#include "defines.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "pch.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

namespace svke {
struct TestPushConstantData {
  glm::mat2 transform{1.0f};
  glm::vec2 offset;
};

class Application {
public:
  Application(uint32_t width, uint32_t height, const std::string &window_name);
  ~Application();

  Application(const Application &other) = delete;
  Application &operator=(const Application &other) = delete;

public:
  void Run();

private:
  void pLoadGameObjects();
  void pCreatePipelineLayout();
  void pCreatePipeline();
  void pCreateCommandBuffers();
  void pFreeCommandBuffers();
  void pDrawFrame();
  void pRenderGameObjects(VkCommandBuffer command_buffer);

private:
  void pRecreateSwapChain();
  void pRecordCommandBuffer(uint32_t image_index);

private:
  uint32_t pWidth;
  uint32_t pHeight;
  std::string pWindowName;

private:
  Window pWindow{pWidth, pHeight, pWindowName};
  Device pDevice{pWindow};
  std::unique_ptr<SwapChain> pSwapChain;
  std::unique_ptr<Pipeline> pPipeline;
  std::vector<GameObject> pGameObjects;
  VkPipelineLayout pPipelineLayout;
  std::vector<VkCommandBuffer> pCommandBuffer;
};
} // namespace svke

#endif
