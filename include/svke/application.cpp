#include "application.hpp"

void sierpinski(std::vector<svke::Model::Vertex>& vertices,
                uint8_t                           depth,
                svke::Model::Vertex               top,
                svke::Model::Vertex               left,
                svke::Model::Vertex               right) {
  if (depth <= 0) {
    vertices.push_back(top);
    vertices.push_back(right);
    vertices.push_back(left);

  } else {
    svke::Model::Vertex leftTop   = {0.5f * (left.position + top.position), 0.5f * (left.color + top.color)};
    svke::Model::Vertex rightTop  = {0.5f * (right.position + top.position), 0.5f * (right.color + top.color)};
    svke::Model::Vertex leftRight = {0.5f * (left.position + right.position), 0.5f * (left.color + right.color)};

    sierpinski(vertices, depth - 1, left, leftRight, leftTop);
    sierpinski(vertices, depth - 1, leftRight, right, rightTop);
    sierpinski(vertices, depth - 1, leftTop, rightTop, top);
  }
}

namespace svke {
  Application::Application(uint32_t width, uint32_t height, const std::string& window_name)
      : pWidth {width}, pHeight {height}, pWindowName {window_name} {
    pLoadModels();
    pCreatePipelineLayout();
    pCreatePipeline();
    pCreateCommandBuffers();
  }

  Application::~Application() { vkDestroyPipelineLayout(pDevice.getDevice(), pPipelineLayout, nullptr); }

  void Application::Run() {
    while (!pWindow.ShouldClose()) {
      glfwPollEvents();
      pDrawFrame();
    }

    vkDeviceWaitIdle(pDevice.getDevice());
  }

  void Application::pLoadModels() {
    std::vector<Model::Vertex> vertices {};

    sierpinski(vertices,
               3,
               {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
               {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
               {{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}});

    pModel = std::make_unique<Model>(pDevice, vertices);
  }

  void Application::pCreatePipelineLayout() {
    VkPipelineLayoutCreateInfo pipeline_layout_info {};

    pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount         = 0;
    pipeline_layout_info.pSetLayouts            = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges    = nullptr;

    if (vkCreatePipelineLayout(pDevice.getDevice(), &pipeline_layout_info, nullptr, &pPipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create pipeline layout");
    }
  }

  void Application::pCreatePipeline() {
    PipelineConfig pipeline_config {};
    Pipeline::DefaultPipelineConfig(pipeline_config, pSwapChain.getWidth(), pSwapChain.getHeight());

    pipeline_config.render_pass     = pSwapChain.getRenderPass();
    pipeline_config.pipeline_layout = pPipelineLayout;

    pPipeline =
        std::make_unique<Pipeline>(pDevice, "shaders/simple.vert.spv", "shaders/simple.frag.spv", pipeline_config);
  }

  void Application::pCreateCommandBuffers() {
    pCommandBuffer.resize(pSwapChain.getImageCount());

    VkCommandBufferAllocateInfo alloc_info {};

    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool        = pDevice.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(pCommandBuffer.size());

    if (vkAllocateCommandBuffers(pDevice.getDevice(), &alloc_info, pCommandBuffer.data()) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate command buffers");
    }

    for (uint64_t i = 0; i < pCommandBuffer.size(); i++) {
      VkCommandBufferBeginInfo begin_info {};

      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      if (vkBeginCommandBuffer(pCommandBuffer[i], &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
      }

      VkRenderPassBeginInfo render_pass_info {};

      render_pass_info.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_info.renderPass  = pSwapChain.getRenderPass();
      render_pass_info.framebuffer = pSwapChain.getFrameBuffer(i);

      render_pass_info.renderArea.offset = {0, 0};
      render_pass_info.renderArea.extent = pSwapChain.getSwapChainExtent();

      std::array<VkClearValue, 2> clear_values {};

      clear_values[0].color            = {0.1f, 0.1f, 0.1f, 1.0f};
      clear_values[1].depthStencil     = {1.0f, 0};
      render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
      render_pass_info.pClearValues    = clear_values.data();

      vkCmdBeginRenderPass(pCommandBuffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

      pPipeline->Bind(pCommandBuffer[i]);
      pModel->Bind(pCommandBuffer[i]);
      pModel->Draw(pCommandBuffer[i]);

      vkCmdEndRenderPass(pCommandBuffer[i]);

      if (vkEndCommandBuffer(pCommandBuffer[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
      }
    }
  }

  void Application::pDrawFrame() {
    uint32_t image_index;
    VkResult result = pSwapChain.AcquireNextImage(&image_index);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("Failed to acquire swap chain image");
    }

    result = pSwapChain.SubmitCommandBuffers(&pCommandBuffer[image_index], &image_index);

    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to present swap chain image");
    }
  }
}