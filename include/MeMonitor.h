#ifndef MEMONITOR_H
#define MEMONITOR_H

#include <cstddef>

/**
 * @brief MeMonitor: Memory Monitor 类用于内存读写
 */
class MeMonitor {

  public:
	MeMonitor();
	~MeMonitor();

	template <typename T>
	static T* mePtr(void* base, size_t offset) {
		return (T*)((char*)base + offset);
	}

	// 辅助函数：直接读取指针指向的值
	template <typename T>
	static T readMemory(T* ptr) {
		return *ptr;
	}

	template <typename T>
	static T readMemory(void* base, size_t offset) {
		return *mePtr<T>(base, offset);
	}
};

#endif // MEMONITOR_H