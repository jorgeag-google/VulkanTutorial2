#ifndef HELLO_TRIANGLE_H
#define HELLO_TRIANGLE_H

#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class HelloTriangleApplication {
public:
    void run();

private:
	// VK ralted
    void initVulkan();
    void createInstance();
    bool checkValidationLayerSupport();
    void pickPhysicalDevice();
    void createLogicalDevice();
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    std::string getDeviceInfo(const VkPhysicalDevice& device);
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    
    // VK debug related
    VkDebugUtilsMessengerEXT m_debugMessenger;
    const std::vector<const char*> m_validationLayers = {
    	"VK_LAYER_KHRONOS_validation"
	};
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData, void* pUserData);

	// Program lifecylce
    void mainLoop();
	bool checkExtensionSupport(const std::vector<const char*>& required_extensions);
	std::vector<const char*> getRequiredExtensions();
    void cleanup();
    
    // GLFW related
    void initWindow();
 
    GLFWwindow* m_window;
    glm::ivec2  m_window_size;
    
    // App related
    const bool m_debug = true;
};

#endif //HELLO_TRIANGLE_H

