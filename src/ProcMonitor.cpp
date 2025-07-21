#include <ProcMonitor.h>
#include <tlhelp32.h>

PROCESSENTRY32 ProcMonitor::findProcessEntry(bool (*matchFunc)(const PROCESSENTRY32&, const void*), const void* param) {
	PROCESSENTRY32 pe32 = {};
	pe32.dwSize = sizeof(PROCESSENTRY32); // 必须初始化结构体大小

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot != INVALID_HANDLE_VALUE) {
		if (Process32First(hSnapshot, &pe32)) {
			do {
				if (matchFunc(pe32, param)) {
					CloseHandle(hSnapshot);
					return pe32;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	return pe32; // 返回空结构体表示未找到
}

LPCTSTR ProcMonitor::getFileName(LPCTSTR szDllPath) {
	LPCTSTR lastSlash = strrchr(szDllPath, '\\');
	if (lastSlash == NULL) {
		lastSlash = strrchr(szDllPath, '/');
	}
	if (lastSlash == NULL) {
		return szDllPath;
	}

	// 返回最后一个分隔符后的部分
	return lastSlash + 1;
}

DWORD ProcMonitor::getPID(LPCTSTR szProcessName) {
	PROCESSENTRY32 pe32 = getProcessEntry(szProcessName);
	if (pe32.th32ProcessID == 0) {
		return 0; // 未找到进程
	}
	return pe32.th32ProcessID;
}

std::string ProcMonitor::getPName(DWORD dwPID) {
	PROCESSENTRY32 pe32 = getProcessEntry(dwPID);
	if (pe32.th32ProcessID == 0) {
		return 0; // 未找到进程
	}
	return std::string(pe32.szExeFile);
}

PROCESSENTRY32 ProcMonitor::getProcessEntry(const char* processName) {
	// PROCESSENTRY32 pe32;
	// pe32.dwSize = sizeof(PROCESSENTRY32); // 必须初始化结构体大小

	// // 创建进程快照
	// HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// if (hSnapshot == INVALID_HANDLE_VALUE) {
	// 	// 错误处理
	// 	return pe32; // 返回空结构体
	// }

	// // 枚举进程
	// if (Process32First(hSnapshot, &pe32)) {
	// 	do {
	// 		if (strcmp(pe32.szExeFile, processName) == 0) {
	// 			CloseHandle(hSnapshot);
	// 			return pe32; // 找到目标进程
	// 		}
	// 	} while (Process32Next(hSnapshot, &pe32));
	// }

	// CloseHandle(hSnapshot);
	// return pe32; // 未找到则返回空结构体
	bool (*matcher)(const PROCESSENTRY32&, const void*) = [](const PROCESSENTRY32& pe32, const void* param) -> bool {
		// return strcmp(pe32.szExeFile, (LPCTSTR)param) == 0;
		return _stricmp(pe32.szExeFile, (char*)param) == 0;
	};

	return findProcessEntry(matcher, processName);
}

PROCESSENTRY32 ProcMonitor::getProcessEntry(DWORD dwPID) {
	// PROCESSENTRY32 pe32;
	// pe32.dwSize = sizeof(PROCESSENTRY32); // 必须初始化结构体大小

	// // 创建进程快照
	// HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// if (hSnapshot == INVALID_HANDLE_VALUE) {
	// 	// 错误处理
	// 	return pe32; // 返回空结构体
	// }

	// // 枚举进程
	// if (Process32First(hSnapshot, &pe32)) {
	// 	do {
	// 		if (pe32.th32ProcessID == dwPID) {
	// 			CloseHandle(hSnapshot);
	// 			return pe32; // 找到目标进程
	// 		}
	// 	} while (Process32Next(hSnapshot, &pe32));
	// }

	// CloseHandle(hSnapshot);
	// return pe32; // 未找到则返回空结构体

	// 2. 声明函数指针变量
	bool (*matcher)(const PROCESSENTRY32&, const void*) = [](const PROCESSENTRY32& pe32, const void* param) -> bool {
		return pe32.th32ProcessID == *(const DWORD*)param;
	};

	return findProcessEntry(matcher, &dwPID);
}

HANDLE ProcMonitor::openProcess(DWORD dwPID) {
	if (dwPID == 0) {
		throw "Invalid dwPID for openProcess";
	}
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

	if (hProcess == NULL) {
		printf("LastError: %lu\n", GetLastError());
		printf("Failed to open process %lu", dwPID);
		throw;
	}

	return hProcess;
}

HANDLE ProcMonitor::openProcess(PROCESSENTRY32 pe32) {
	if (pe32.th32ProcessID == 0) {
		throw "Invalid PROCESSENTRY32 for openProcess";
	}
	return openProcess(pe32.th32ProcessID);
}

int ProcMonitor::closeProcess(HANDLE hProcess) {
	return CloseHandle(hProcess);
}

std::unordered_map<std::string, LPVOID> ProcMonitor::getModules(HANDLE hProcess) {

	std::unordered_map<std::string, LPVOID> modules;

	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));

	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot failed: %lu\n", GetLastError());
		return modules;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);

	// 遍历所有模块
	if (Module32First(hModuleSnap, &me32)) {
		do {
			modules.insert({(std::string)me32.szModule, me32.modBaseAddr});
		} while (Module32Next(hModuleSnap, &me32));
	} else {
		printf("Module32First failed: %lu\n", GetLastError());
	}

	CloseHandle(hModuleSnap);
	return modules;
}

LPVOID ProcMonitor::searchModule(std::unordered_map<std::string, LPVOID> modules, std::string dllName) {
	{
		if (modules.find(dllName) != modules.end()) {
			return modules[dllName];
		}
		return nullptr;
	}
}
