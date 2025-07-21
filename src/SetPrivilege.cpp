#include <SetPrivilege.h>
bool SetPrivilege(LPCTSTR lpszPrivilege, bool bEnablePrivilege) {
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(),
						  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
						  &hToken)) {

		return FALSE;
	}

	// lookup privilege on local system 获取指定权限（如调试权限）在本地系统上的唯一标识符（LUID）
	if (!LookupPrivilegeValue(NULL,
							  lpszPrivilege, // privilege to lookup
							  &luid))		 // receives LUID of privilege
	{

		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges 调整权限
	if (!AdjustTokenPrivileges(hToken,
							   FALSE,
							   &tp,
							   sizeof(TOKEN_PRIVILEGES),
							   (PTOKEN_PRIVILEGES)NULL,
							   (PDWORD)NULL)) {

		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {

		return FALSE;
	}

	return true;
}