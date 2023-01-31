#ifndef VK_CHECK(x)
#define	VK_CHECK(x)\
do\
{\
	VkResult err = x;\
	if (err)\
	{\
		std::cout << "detected Vulkan error:	" << err << "\n";\
		abort();\
	}\
} while (0)
#endif //VK_CHECK(x)