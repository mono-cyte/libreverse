#ifndef MEMONITOR_H
#define MEMONITOR_H

#include <pch.h>
#include <vector>

/**
 * @brief MeMonitor: Memory Monitor 类用于内存读写
 */
class MeMonitor {

  public:
	MeMonitor();
	~MeMonitor();

	/**
	 * @brief getAddr: 获取内存数据的指针
	 * @param base: 基地址
	 * @param offsets: 偏移量列表(从内到外排序,指向具体内存数据)
	 */
	static uintptr_t getAddr(uintptr_t base, const std::vector<uintptr_t>& offsets);

	// 辅助函数：直接读取指针指向的值
};

#endif // MEMONITOR_H