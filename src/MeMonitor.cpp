#include "MeMonitor.h"
#include <cstdint>

uintptr_t MeMonitor::getAddr(uintptr_t base, const std::vector<uintptr_t>& offsets) {
	// 多级指针解引用
	auto it = offsets.begin();
	auto end = offsets.end();
	if (it != end) {
		for (; std::next(it) != end; ++it) {
			base = *(uintptr_t*)(base + *it);
		}
		base += *it;
	}
	return base;
}