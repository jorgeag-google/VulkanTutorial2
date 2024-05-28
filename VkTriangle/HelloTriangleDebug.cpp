#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HelloTriangleApplication.h"


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional
}

void HelloTriangleApplication::setupDebugMessenger() {
	if (!m_debug) {
		return;
	}
	
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);
	
	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
    	throw std::runtime_error("failed to set up debug messenger!");
	}
	
	if (m_debug) {
		std::cout << "Debug messenger created!" << std::endl;
	}
}

bool HelloTriangleApplication::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    if (m_debug) {
    	std::cout << "Required layers:\n";
		for (const auto& req_layer : m_validationLayers) {
			std::cout << '\t' << req_layer << '\n';
		}
		std::cout << "Available layers:\n";
		for (const auto& layer : availableLayers) {
			std::cout << '\t' << layer.layerName << '\n';
		}
	}
    
    bool allThere = true;
	for (size_t i = 0; i < m_validationLayers.size(); ++i) {
		std::string  req_layer = { m_validationLayers[i] };
		// Check that this layers is on the available ones
		bool found = false;
		for (const auto& layerProperties : availableLayers) {
			std::string layer_name = { layerProperties.layerName };
			if (layer_name == req_layer) {
				found = true;
				break;
			}
		}
		allThere &= found;
	}
	
	if (m_debug && allThere) {
		std::cout << "Layers dependency meet!" << std::endl;
	}

    return allThere;
}


// Format the error of the debug layer into an string that contains the info we desire to print
std::string debugMgsg2str(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {

	std::stringstream ss;

	ss << "  SEVERITY: ";
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		ss << "diagnostic";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		ss << "informational";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		ss << "warning";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		ss << "error";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		// Not an error
		break;
	}
	ss << std::endl;

	ss << "  TYPE: ";
	switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		ss << "general";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		ss << "validation";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		ss << "performance";
		break;
	}
	ss << std::endl;
	// This is the actual error message
	ss << pCallbackData->pMessage;
	ss << std::endl;

	return ss.str();
}

std::string HelloTriangleApplication::getDeviceInfo(const VkPhysicalDevice& device) {

	std::stringstream ss;
	
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	
	ss << "Vendor: ";
	switch(deviceProperties.vendorID) {
		case 0x1002:
			ss << "AMD";
		break;
		case 0x1010:
			ss << "ImgTec";
		break;
		case 0x10DE:
			ss << "NVIDIA";
		break;
		case 0x13B5:
			ss << "ARM";
		break;
		case 0x5143:
			ss << "Qualcomm";
		break;
		case 0x8086:
			ss << "INTEL";
		break;
		default:
			ss << deviceProperties.vendorID;
	}
	ss << std::endl;
	
	ss << "Name: " << deviceProperties.deviceName << std::endl;
	
	ss << "API version: " << VK_API_VERSION_MAJOR(deviceProperties.apiVersion) << "." << VK_API_VERSION_MINOR(deviceProperties.apiVersion)
	   << "." << VK_API_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
	   
	ss << "Device id: " << deviceProperties.deviceID << std::endl;
	
	ss << "Driver version: " << deviceProperties.driverVersion << std::endl;
	
	ss << "Type: ";
	switch(deviceProperties.deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			ss << "other";
		break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			ss << "Integrated GPU";
		break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			ss << "Discrete GPU";
		break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			ss << "Virtual GPU";
		break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			ss << "CPU";
		break;
		default:
			ss << "Unknown type";
	}
	ss << std::endl;
	
	return ss.str();
}


VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData, void* pUserData) {

	// If this is not a error ignore !!
	if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		return VK_FALSE;
	}

	std::cerr << "validation layer:" << std::endl << debugMgsg2str(messageSeverity, messageType, pCallbackData) << std::endl;

    return VK_FALSE;
}
