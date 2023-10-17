// Excel.cpp - stubs for Excel functions
#include <windows.h>
#include "../bondxll/xll/xll/XLCALL.H"

#ifdef __cplusplus
extern "C" {
#endif

	int _cdecl Excel4(int xlfn, LPXLOPER operRes, int count, ...) { return 0; }
	/* followed by count LPXLOPERs */

	int pascal Excel4v(int xlfn, LPXLOPER operRes, int count, LPXLOPER opers[]) { return 0; }

	int pascal XLCallVer(void) { return 0; }

	//long pascal LPenHelper(int wCode, VOID* lpv);

	int _cdecl Excel12(int xlfn, LPXLOPER12 operRes, int count, ...) { return 0; }
	/* followed by count LPXLOPER12s */

	int pascal Excel12v(int xlfn, LPXLOPER12 operRes, int count, LPXLOPER12 opers[]) { return 0; }

#ifdef __cplusplus
}
#endif