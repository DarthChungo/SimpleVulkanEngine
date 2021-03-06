#include "defines.hpp"
#include "model.hpp"
#include "pch.hpp"

namespace svke {
  Model::Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
      : pDevice {device} {
    pCreateVertexBuffer(vertices);
    pCreateIndexBuffer(indices);
  }

  Model::Model(Device& device, const std::vector<Vertex>& vertices) : pDevice {device} {
    pCreateVertexBuffer(vertices);
  }

  Model::~Model() {
    vkDestroyBuffer(pDevice.getDevice(), pVertexBuffer, nullptr);
    vkFreeMemory(pDevice.getDevice(), pVertexBufferMemory, nullptr);

    if (pUsingIndexBuffer) {
      vkDestroyBuffer(pDevice.getDevice(), pIndexBuffer, nullptr);
      vkFreeMemory(pDevice.getDevice(), pIndexBufferMemory, nullptr);
    }
  }

  void Model::Bind(VkCommandBuffer buffer) {
    VkBuffer     buffers[] = {pVertexBuffer};
    VkDeviceSize offets[]  = {0};

    vkCmdBindVertexBuffers(buffer, 0, 1, buffers, offets);

    if (pUsingIndexBuffer) {
      vkCmdBindIndexBuffer(buffer, pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void Model::Draw(VkCommandBuffer buffer) {
    if (pUsingIndexBuffer) {
      vkCmdDrawIndexed(buffer, pIndexCount, 1, 0, 0, 0);
    } else {
      vkCmdDraw(buffer, pVertexCount, 1, 0, 0);
    }
  }

  void Model::pCreateVertexBuffer(const std::vector<Vertex>& vertices) {
    pVertexCount = static_cast<uint32_t>(vertices.size());

    if (pVertexCount < 3) {
      throw std::runtime_error("The model cannot contain less than three vertices");
    }

    VkDeviceSize buffer_size = sizeof(vertices[0]) * pVertexCount;
    pDevice.CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         pVertexBuffer,
                         pVertexBufferMemory);

    void* data;

    vkMapMemory(pDevice.getDevice(), pVertexBufferMemory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(pDevice.getDevice(), pVertexBufferMemory);
  }

  void Model::pCreateIndexBuffer(const std::vector<uint32_t>& indices) {
    pIndexCount       = static_cast<uint32_t>(indices.size());
    pUsingIndexBuffer = true;

    VkDeviceSize buffer_size = sizeof(indices[0]) * pIndexCount;
    pDevice.CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         pIndexBuffer,
                         pIndexBufferMemory);

    void* data;

    vkMapMemory(pDevice.getDevice(), pIndexBufferMemory, 0, buffer_size, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(pDevice.getDevice(), pIndexBufferMemory);
  }

  std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindings() {
    std::vector<VkVertexInputBindingDescription> descriptions(1);

    descriptions[0].binding = 0;
    descriptions[0].stride  = sizeof(Vertex);
    descriptions[0].binding = VK_VERTEX_INPUT_RATE_VERTEX;

    return descriptions;
  }

  std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAtributes() {
    std::vector<VkVertexInputAttributeDescription> descriptions(2);

    descriptions[0].binding  = 0;
    descriptions[0].location = 0;
    descriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[0].offset   = offsetof(Vertex, position);

    descriptions[1].binding  = 0;
    descriptions[1].location = 1;
    descriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].offset   = offsetof(Vertex, color);

    return descriptions;
  }
}
