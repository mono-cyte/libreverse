#include <DLLMonitor.h>
#include <ProcMonitor.h>
#include <libloaderapi.h>
#include <winnt.h>

DWORD DLLMonitor::inject(HANDLE hProcess, HMODULE hKERNEL32, LPCTSTR szDllPath) {

	if (hProcess == NULL || hKERNEL32 == NULL || szDllPath == NULL) {
		return ERROR_INVALID_PARAMETER;
	}

	DWORD dwSize = strlen(szDllPath) + 1;

	// 为dll路径字符串的大小申请虚拟内存 (flProtect: 内存保护常量)
	LPVOID pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

	// 将dll路径字符串写入虚拟内存
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwSize, NULL);

	// LoadLibraryA 成功返回handle, 失败返回NULL
	LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKERNEL32, "LoadLibraryA");

	// 使用进程的KERNEL32.DLL模块的LoadLibraryA函数, 以存储dll路径字符串的虚拟内存内容作为参数, 创建一个远程线程
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, pRemoteBuf, 0, NULL);

	DWORD check = WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	VirtualFree(pRemoteBuf, dwSize, MEM_RELEASE);

	return check;
}

DWORD DLLMonitor::inject(PROCESSENTRY32 pe32, LPCTSTR szDllPath) {
	HANDLE hProcess = ProcMonitor::openProcess(pe32.th32ProcessID);
	HMODULE hKERNEL32 = GetModuleHandleA("KERNEL32.DLL");
	DWORD ret = inject(hProcess, hKERNEL32, szDllPath);
	ProcMonitor::closeProcess(hProcess);
	return ret;
}
DWORD DLLMonitor::release(HANDLE hProcess, HMODULE hKERNEL32, LPVOID injectedModule) {

	DWORD dwSize = sizeof(DWORD);

	LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKERNEL32, "FreeLibrary");

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, injectedModule, 0, NULL);

	DWORD check = WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	return check;
}

DWORD DLLMonitor::release(HANDLE hProcess, LPVOID injectedModule) {
	HMODULE hKERNEL32 = GetModuleHandleA("KERNEL32.DLL"); // 内核模块位置固定
	DWORD ret = release(hProcess, hKERNEL32, injectedModule);
	return ret;
}

DWORD DLLMonitor::release(HANDLE hProcess, LPCTSTR szDllPath) {
	LPCTSTR dllModuleName = ProcMonitor::getFileName(szDllPath);

	LPVOID injectedModule = ProcMonitor::searchModule(ProcMonitor::getModules(hProcess), dllModuleName);

	if (injectedModule == nullptr) {
		return -1;
	}

	DWORD ret = release(hProcess, injectedModule);
	return ret;
}

DWORD DLLMonitor::release(PROCESSENTRY32 pe32, LPCTSTR szDllPath) {
	HANDLE hProcess = ProcMonitor::openProcess(pe32.th32ProcessID);

	DWORD ret = release(hProcess, szDllPath);
	return ret;
}