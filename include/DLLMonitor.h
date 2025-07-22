#ifndef DLLMONITOR_H
#define DLLMONITOR_H

#include <pch.h>

class DLLMonitor {
  public:
	DLLMonitor();
	~DLLMonitor();

	/**
	 * @brief 创建远程线程，以在目标进程中加载DLL
	 * @param dwPID 		dw = dword
	 * @param szDllPath 	sw = string zero, 以\0结尾的字符串
	 * @return DWORD 成功返回 0，失败返回错误码
	 */
	static DWORD inject(HANDLE hProcess, HMODULE hKERNEL32, LPCTSTR szDllPath);

	/**
	 * @brief inject升级版, 自动解析
	 * @param pe32 目标进程的PROCESSENTRY32结构体
	 * @param szDllPath DLL路径
	 * @return DWORD 成功返回 0，失败返回错误码
	 */
	static DWORD inject(const PROCESSENTRY32& pe32, LPCTSTR szDllPath);

	/**
	 * @brief 创建远程线程, 以在目标进程中卸载DLL, 需要手动寻找模块句柄
	 * @param dwPID 		dw = dword
	 * @param injectedModule 	已注入的模块句柄
	 * @return DWORD 成功返回 0，失败返回错误码
	 */
	static DWORD release(HANDLE hProcess, HMODULE hKERNEL32, LPVOID injectedModule);

	/**
	 * @brief release中介, 重载函数使用
	 * @param dwPID 		    dw = dword
	 * @param injectedModule    已注入的模块句柄
	 * @return DWORD 		    成功返回 0，失败返回错误码
	 */
	static DWORD release(HANDLE hProcess, LPVOID injectedModule);

	/**
	 * @brief release升级版, 自动解析
	 * @param dwPID 		    dw = dword
	 * @param injectedModule    已注入的模块句柄
	 * @return DWORD 		    成功返回 0，失败返回错误码
	 */
	static DWORD release(HANDLE hProcess, LPCTSTR szDllPath);

	static DWORD release(const PROCESSENTRY32& pe32, LPCTSTR szDllPath);
};

#endif // DLLMONITOR_H