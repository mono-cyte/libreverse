#include <ProcMonitor.h>

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
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	do {
		if (stricmp(pe.szExeFile, szProcessName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &pe));

	CloseHandle(hSnapshot);
	printf("LastError: %lu", GetLastError());
	printf("Failed to find process %s", szProcessName);
	throw;
}

char* ProcMonitor::getPName(DWORD dwPID) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	char* PName = nullptr;

	do {
		if (pe.th32ProcessID == dwPID) {
			CloseHandle(hSnapshot);
			PName = strdup(pe.szExeFile);
			break;
		}
	} while (Process32Next(hSnapshot, &pe));

	CloseHandle(hSnapshot);

	if (PName == NULL) {
		printf("LastError: %lu\n", GetLastError());
		printf("Failed to find process name %lu", dwPID);
		throw;
	}

	return PName;
}

HANDLE ProcMonitor::getProcess(DWORD dwPID) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

	if (hProcess == NULL) {
		printf("LastError: %lu\n", GetLastError());
		printf("Failed to open process %lu", dwPID);
		throw;
	}

	return hProcess;
}

std::unordered_map<std::string, LPVOID> ProcMonitor::getModules(HANDLE hProcess, MODULEENTRY32* me32) {

	std::unordered_map<std::string, LPVOID> modules;

	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));

	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot failed: %lu\n", GetLastError());
		return modules;
	}

	me32->dwSize = sizeof(MODULEENTRY32);

	// 遍历所有模块
	if (Module32First(hModuleSnap, me32)) {
		do {
			modules.insert({(std::string)me32->szModule, me32->modBaseAddr});
		} while (Module32Next(hModuleSnap, me32));
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
