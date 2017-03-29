#include "shared.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

//toolbox pull
const char *RSF_DISK_TO_CACHE_WORKING = "/cache/recovery/.disk_to_cache_working";

//toolbox push
//const char *RSF_CACHE_TO_DISK_WORKING = "/cache/recovery/.cache_to_disk_working";

//bridge pull
const char *RSF_CACHE_TO_DATA_WORKING = "/cache/recovery/.cache_to_data_working";

//bridge push
//const char *RSF_DATA_TO_CACHE_WORKING = "/cache/recovery/.data_to_cache_working";


//toolbox.c shouldn't read while bridge is writing
const char *RSF_SENT_FILE_LIST						= "/cache/recovery/.sent_file_list";

const char *RSF_IS_PULL								= "/cache/recovery/.is_pull_session";


const char *RSF_BRIDGE_RUN_ONCE 					= "/cache/recovery/.cache_to_data_working";
const char *RSF_TOOLBOX_READY						= "/cache/recovery/.toolbox_is_ready";
const char *RSF_BRIDGE_READY						= "/cache/recovery/.bridge_is_ready";
const char *RSF_ALL_DONE              				= "/data/local/tmp/.farm_done";
const char *RSF_PANIC              					= "/data/local/tmp/.panic";

int rsf_check(const char *file) {
	int fd;
	if( (fd = open(file, O_RDONLY)) == -1 ) {
		return 0;
	}
	close(fd);
	return 1;
}

int rsf_set(const char *file) {
	int fd;

	if(rsf_check(file)) 
		return 0;

	if( (fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0777)) == -1 )
		return 0;
	close(fd);

	chmod(file, 0777);
	return 1;
}

int rsf_unset(const char *file) {
	return unlink(file);
}
