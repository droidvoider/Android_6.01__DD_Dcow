#ifndef __SHARED_H__
#define __SHARED_H__

//=>] run once logic [<=
//likely needed with till, no sources avail. this was taken from the pull sections it grabs us root
const char *RSF_DISK_TO_CACHE_WORKING; /* toolbox.c */
const char *RSF_CACHE_TO_DATA_WORKING; /* bridge.c only */

//bridge.c will search for of= as the first value, if it is then we are pulling so toolbox.c goes first.
const char *RSF_IS_PULL;

//toolbox.c shouldn't read while bridge is writing
const char *RSF_SENT_FILE_LIST;

//=>] Shared values [<=
//toolbox readystate
const char *RSF_TOOLBOX_READY;
//bridge readystate
const char *RSF_BRIDGE_READY;

//all done stop signal and panic quit signal
extern const char *RSF_ALL_DONE;
extern const char *RSF_PANIC;

// returns 1 if set, or 0 if it exists.
int rsf_set(const char *file);

// returns 1 if exists, or 0 if it does not exist
int rsf_check(const char *file);

// returns 0 on success, other on failure
int rsf_unset(const char *file);

// log info
#define LOGV(...) { __android_log_print(ANDROID_LOG_INFO, "droid_dd_dcow", __VA_ARGS__); }

// log notice
#define LOGN(x) { __android_log_print(ANDROID_LOG_INFO, "droid_dd_dcow", "NOTICE: %s", (x)); }
#endif
