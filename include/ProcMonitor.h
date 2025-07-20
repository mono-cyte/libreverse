#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <pch.h>

#include <unordered_map>
#include <string>

class ProcMonitor {
  private:
	// 通用进程查找模板
	static PROCESSENTRY32 findProcessEntry(bool (*matchFunc)(const PROCESSENTRY32&, const void*), const void* param);

  public:
	static LPCTSTR getFileName(LPCTSTR szDllPath);

	static DWORD getPID(LPCTSTR szProcessName);

	static std::string getPName(DWORD dwPID);

	static PROCESSENTRY32 getProcessEntry(const char* processName);

	static PROCESSENTRY32 getProcessEntry(DWORD dwPID);

	/**
	 * @brief 获取进程句柄
	 * @param DWORD dwPID 进程ID
	 * @return HANDLE 进程句柄
	 */
	static HANDLE getProcess(DWORD dwPID);

	/**
	 * @brief 通过 hashtable 获取所有模块名称&基址
	 * @param HANDLE hProcess 目标进程句柄
	 * @param MODULEENTRY32* me32 模块信息结构体
	 * @param LPCTSTR dllName 模块名称
	 * @return 模块名称和基址的映射表(提高查找效率)
	 */
	static std::unordered_map<std::string, LPVOID> getModules(HANDLE hProcess, MODULEENTRY32* me32);

	/**
	 * @brief
	 * @param std::unordered_map<std::string, LPVOID> modules
	 * @param dllName
	 * @return std::string 模块基址 LPVOID
	 */
	static LPVOID searchModule(std::unordered_map<std::string, LPVOID> modules, std::string dllName);
};

#endif