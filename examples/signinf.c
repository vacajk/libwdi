/*
* wdi-simple.c: Console Driver Installer for a single USB device
* Copyright (c) 2010-2018 Pete Batard <pete@akeo.ie>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include "getopt/getopt.h"
#else
#include <getopt.h>
#endif
#include "libwdi.h"

#if defined(_PREFAST_)
/* Disable "Banned API Usage:" errors when using WDK's OACR/Prefast */
#pragma warning(disable:28719)
/* Disable "Consider using 'GetTickCount64' instead of 'GetTickCount'" when using WDK's OACR/Prefast */
#pragma warning(disable:28159)
#endif

#define oprintf(...) do {if (!opt_silent) printf(__VA_ARGS__);} while(0)

/*
 * Change these values according to your device if
 * you don't want to provide parameters
 */
#define DESC        "Microsoft XBox Controller Type S"
#define VID         0x045E
#define PID         0x0289
#define INF_NAME    "usb_device.inf"
#define DEFAULT_DIR "usb_driver"


void usage(void)
{
	printf("\n");
	printf("gencat <cat file name> <hardware id> [[[file name 1] file name 2]  ... ]\n");
	printf("   <cat file name>  <hardware id>\n");
	printf("\n");
	exit(-1);
}


#define ERR(fmt, ...)  fprintf(stderr, "Error: " fmt "\n", __VA_ARGS__)

BOOL CreateCat(LPCSTR szCatPath, LPCSTR szHWID, LPCSTR szSearchDir, LPCSTR* szFileList, DWORD cFileList);
BOOL SelfSignFile(LPCSTR szFileName, LPCSTR szCertSubject);

int __cdecl main(int argc, char** argv)
{
	int r = 0;
	if (argc < 4) usage();
	DWORD cFileList = argc - 3;
	LPSTR *szLocalFileList = (LPSTR *)malloc(cFileList * sizeof(LPSTR));
	for (DWORD i = 0; i < cFileList; i++) {
		szLocalFileList[i] = argv[3 + i];
	}

	char subject[MAX_PATH];
	sprintf(subject, "CN=%s (tiny usb tool)", argv[2]);
	if (CreateCat(argv[1], argv[2], NULL, szLocalFileList, cFileList)) {
		if (!SelfSignFile(argv[1], subject)) {
			ERR("Fail to sign cat file %s", argv[1]);
		}
	}
	else {
		ERR("Fail to create cat file %s", argv[1]);
	}


	free(szLocalFileList);



	return r;
}
