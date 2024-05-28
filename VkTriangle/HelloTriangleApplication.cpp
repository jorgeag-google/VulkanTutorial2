#include <cstring>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HelloTriangleApplication.h"

void HelloTriangleApplication::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApplication::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window_size = glm::ivec2(800, 600);
	m_window = glfwCreateWindow(m_window_size.x, m_window_size.y, "Vulkan", nullptr, nullptr);
	
	if (m_debug) {
		if (m_window != nullptr) {
			std::cout << "GLFW window created!" << std::endl;
		}
	}
}

void HelloTriangleApplication::initVulkan() {
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
}

void HelloTriangleApplication::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	
	// Empthy for now, but we will need to have it
	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	if (m_debug) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	
	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
    	throw std::runtime_error("failed to create logical device!");
	}

	if (m_debug) {
		std::cout << "Device created!" << std::endl;
	}
	
	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	if (m_debug) {
		std::cout << "Graphics queue created!" << std::endl;
	}
}

void HelloTriangleApplication::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
    	throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	if (m_debug) {
		std::cout << "There are " << deviceCount << " devices" << std::endl;
	}

	std::optional<size_t> deviceIndex;
	int i = 0;
	for (const auto& device : devices) {
		if (m_debug) {
			std::cout << i << ":" << std::endl;
		}
		
    	if (isDeviceSuitable(device) && !deviceIndex.has_value()) {
			deviceIndex = i;
		}
		++i;
    }
    
    if (deviceIndex.has_value()) {
    	m_physicalDevice = devices[*deviceIndex];
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
    	throw std::runtime_error("failed to find a suitable GPU!");
	}

	if (m_debug) {
		std::cout << "Suitable GPU found:" << std::endl;
		std::cout << getDeviceInfo(m_physicalDevice) << std::endl;
	}
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(const VkPhysicalDevice& device) {
	QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
		    indices.graphicsFamily = i;
		}
		
		if (indices.isComplete()) {
			break;
	    }

		i++;
	}

    return indices;
}

bool HelloTriangleApplication::isDeviceSuitable(const VkPhysicalDevice& device) {
	
	if (m_debug) {
		std::cout << getDeviceInfo(device) << std::endl;
	}
	
	QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

void HelloTriangleApplication::createInstance() {

    if (m_debug && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not available!");
    }

	VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Check that all required extensions are available
	auto required_extensions = getRequiredExtensions();
	if (!checkExtensionSupport(required_extensions)) {
		throw std::runtime_error("Missing VK extensions!");
	} else {
		createInfo.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		createInfo.ppEnabledExtensionNames = required_extensions.data();
	}
	
	// If we want validation layers
	// Enable until we get the extension correctlly (if not there is a crash)
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (m_debug) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
		
		populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    	throw std::runtime_error("failed to create instance!");
	} else  if (m_debug) {
		std::cout << "VK instance created!" << std::endl;
	}
}

bool HelloTriangleApplication::checkExtensionSupport(const std::vector<const char*>& required_extensions) {
	// Query for available extensions
	// 1) How many are there
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// 2) which are they
	std::vector<VkExtensionProperties> avail_extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, avail_extensions.data());
	
	// print just for debug
	if (m_debug) {
		std::cout << "Required extensions:\n";
		for (const auto& extension_name : required_extensions) {
			std::cout << '\t' << extension_name << '\n';
		}
		std::cout << "Available extensions:\n";
		for (const auto& extension : avail_extensions) {
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}
	
	bool allThere = true;
	for (const auto& req_extension : required_extensions) {
		// Check that this required extension is on the available ones
		bool found = false;
		for (const auto& avail_extension : avail_extensions) {
			if (strcmp(avail_extension.extensionName, req_extension) == 0) {
				found = true;
				break;
			}
		}
		allThere &= found;
	}
	
	if (allThere) {
		std::cout << "Extensions dependency meet!" << std::endl;
	}
	
	return allThere;
}

std::vector<const char*> HelloTriangleApplication::getRequiredExtensions() {
	// Query the required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	std::vector<const char*> required_extensions{glfwExtensions, glfwExtensions + glfwExtensionCount};

	if (m_debug) {
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	
	return required_extensions;
}

void HelloTriangleApplication::mainLoop() {
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void HelloTriangleApplication::cleanup() {
	// VK debug
	if (m_debug) {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

	// VK
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);
	
	// GLFW
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

