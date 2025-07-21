#ifndef MEMONITOR_H
#define MEMONITOR_H

#include <pch.h>
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
		if (!ptr) {
			return T{};
		}

		DWORD oldProtect;
		bool needRestore = false;

		// 检查当前保护属性（需额外调用 VirtualQuery）
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
			if (mbi.Protect & (PAGE_EXECUTE | PAGE_NOACCESS)) {
				// 需要临时改为可读
				if (VirtualProtect(ptr, sizeof(T), PAGE_READONLY, &oldProtect)) {
					needRestore = true;
				}
			}
		}

		T value = *ptr; // 实际读取

		if (needRestore) {
			VirtualProtect(ptr, sizeof(T), oldProtect, &oldProtect);
		}
		return value;
	}

	template <typename T>
	static T readMemory(void* base, size_t offset) {
		return *mePtr<T>(base, offset);
	}

	template <typename T>
	static bool writeMemory(T* ptr, const T& value) {
		if (!ptr)
			return false;

		DWORD oldProtect;
		if (!VirtualProtect(ptr, sizeof(T), PAGE_READWRITE, &oldProtect)) {
			return false;
		}

		*ptr = value;

		// 恢复保护并验证写入
		VirtualProtect(ptr, sizeof(T), oldProtect, &oldProtect);
		return (readMemory(ptr) == value); // 简化返回值逻辑
	}

	template <typename T>
	static bool writeMemory(void* base, size_t offset, const T& value) {
		return writeMemory(mePtr<T>(base, offset), value);
	}
};

#endif // MEMONITOR_H