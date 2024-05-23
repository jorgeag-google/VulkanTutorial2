#include <cstring>
#include <stdexcept>
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

	std::string str;

	str.append("  SEVERITY: ");
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		str.append("diagnostic");
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		str.append("informational");
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		str.append("warning");
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		str.append("error");
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		// Not an error
		break;
	}
	str.append("\n");

	str.append("  TYPE: ");
	switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		str.append("general");
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		str.append("validation");
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		str.append("performance");
		break;
	}
	str.append("\n");
	// This is the actual error message
	str.append(pCallbackData->pMessage);
	str.append("\n");

	return str;
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
