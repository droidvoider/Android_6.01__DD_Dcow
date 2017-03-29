/*
Copyright (c) 2010, The Android Open Source Project.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the 
   distribution.
 * Neither the name of The Android Open Source Project nor the names
   of its contributors may be used to endorse or promote products
   derived from this software without specific prior written
   permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <selinux/selinux.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "shared.h"

// #define FARM_PULL 1

int main(int, char **);

static int toolbox_main(int argc, char **argv)
{
    // "toolbox foo ..." is equivalent to "foo ..."
    if (argc > 1) {
        return main(argc - 1, argv + 1);
    } else {
        printf("!Toolbox!\n");
        return 0;
    }
}

#define TOOL(name) int name##_main(int, char**);
#include "tools.h"
#undef TOOL

static struct 
{
    const char *name;
    int (*func)(int, char**);
} tools[] = {
    { "toolbox", toolbox_main },
#define TOOL(name) { #name, name##_main },
#include "tools.h"
#undef TOOL
    { 0, 0 },
};

static void SIGPIPE_handler(int signal) {
    // Those desktop Linux tools that catch SIGPIPE seem to agree that it's
    // a successful way to exit, not a failure. (Which makes sense --- we were
    // told to stop by a reader, rather than failing to continue ourselves.)
    _exit(0);
}

int main(int argc, char **argv)
{
    
    
    int i;
	char *currcon;
    char *name = argv[0];

	// contexts
	const char *SERVER_CONTEXT = "u:r:system_server:s0";
	const char *INSTALL_CONTEXT = "u:r:install_recovery:s0";

	// only run SERVER_CONTEXT once
	const char *RSF_TOOLBOX = "/cache/recovery/.toolbox";
	
	getcon(&currcon);
	LOGV("[+] toolbox running %s as uid: %d, gid: %d, context %s", argv[0], getuid(), getgid(), currcon);
	

	if( !strcmp(SERVER_CONTEXT, currcon) && rsf_set(RSF_TOOLBOX) ) {
		LOGV("[*] toolbox launching install context");
		property_set("ctl.start", "flash_recovery");
	//if install_recovery set disk to cache working, and go
	}else if( !strcmp(INSTALL_CONTEXT, currcon) && rsf_set(RSF_DISK_TO_CACHE_WORKING) ) {
	
	
	pid_t cpid;
    pid_t child_pid;
	
    int 		max_num_of_copies 		= 31;  /* max number of push/pull lines or partitions, 32 is safe */
    int 		copy_path_length 		= 1023; /* how many chars used to store the dd paths left/right */
    int 		max_line_length 		= 2047;  /* this is the entire line length */
    
    char *	  	exe_name				="/system/bin/dd";

   //fgets(line
   char line[max_line_length];


   int iCntr=0;
   int iLoop=0;

    //reserve memory and test if we ran out
    char **ddFileLineLeft = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (ddFileLineLeft==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
        }        
    //reserve memory and test if we ran out
    char **ddFileLineRight = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (ddFileLineRight==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
        }
    //reserve memory and test if we ran out
    char **dd_block_size = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (dd_block_size==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
        }  	



		// waits for communication from bridge that files.txt has been sent.
		while(!rsf_check(RSF_SENT_FILE_LIST)) sleep(1);
		LOGV("[*] toolbox pulling file list");

		
		//panic
		if (rsf_check(RSF_PANIC)){
			return 1;
		}

	FILE *fpBlock = fpBlock = fopen("/cache/recovery/files.txt","r");
   if (fpBlock == NULL) {
    printf("couldn't open /cache/recovery/files.txt was there a copy error reported in logcat by bridge?\n");
    printf("farm binary overwrites dumpstate with bridge then does fork setprop dumpstate\n");
    printf("bridge copies /data/local/tmp/files.txt to /cache/recovery/files.txt\n");    
   	return 1;
   }
//===>[-] Parse text file for toolbox specific duties [-]<===
   while (fgets(line, sizeof(line), fpBlock)) {
        /* Allocate space for the next line */
        dd_block_size[iCntr] = malloc(40);
        ddFileLineLeft[iCntr] = malloc(max_line_length);        
        ddFileLineRight[iCntr] = malloc(max_line_length);
        
	//return entire line up to " "
	strcpy(ddFileLineLeft[iCntr],strtok(line, " "));
	//return entire line up to the next " "
	strcpy(ddFileLineRight[iCntr],strtok(NULL, " "));
	//return entire line up to the first |	
	strcpy(dd_block_size[iCntr],strtok(NULL, "|"));		
	//if there are no more lines of text, break while	
	if (ddFileLineLeft[iCntr] == NULL || ddFileLineRight[iCntr] == NULL || dd_block_size[iCntr] == NULL) {
		break;
	}
	iCntr++;
}  
//we have the data step a is complete
if(fpBlock != NULL)				        
fclose(fpBlock);

//LOGV("toolbox is loaded and ready.. attempting communication with bridge...");		
		
		//panic
		if (rsf_check(RSF_PANIC)){
			return 1;
		}
		
//rsf_set(RSF_DISK_TO_CACHE_WORKING);
	while (iLoop < iCntr) {
			//panic
		if (rsf_check(RSF_PANIC)){
			break; 
		}

	//ready to send logic handled by bridge
	while (!rsf_check(RSF_BRIDGE_READY)) {sleep(1);}
	

    //attempts to fork the current process returning the results as pid_t, which is basically int
    cpid = fork();
    //let's grab our child process
    switch (cpid)
    	{
        	case -1: LOGV("Fork failed; cpid == -1");
                 	break;
			//means we successfully forked, let's retrieve the process id
        	case 0: child_pid = getpid();
					const char *args[] = {exe_name, ddFileLineLeft[iLoop], ddFileLineRight[iLoop], dd_block_size[iCntr]};
					execv(args[0], (char * const*)args);
        	default: //LOGV("toolbox pid: %d is waiting for forked pid: %d's dd command to complete",getppid(), cpid);
                 	waitpid(cpid, NULL, 0);
		}
		//let bridge know we are done working and unset bridge is ready since we know first.. 
			if (rsf_check(RSF_BRIDGE_READY)){
				rsf_unset(RSF_BRIDGE_READY);
			}	
		rsf_set(RSF_TOOLBOX_READY);
		iLoop++;
	}		
/////////END OF NEW LOGIC IS THE BRACKET ABOVE THIS COMMENT
///////////////////////////////////////////////////////////////////////////////////////////////		
if(ddFileLineLeft != NULL)				        
free(ddFileLineLeft);
if(ddFileLineRight != NULL)				        
free(ddFileLineRight);
if(currcon != NULL)
	free(currcon);				

} //END OF ELSE IF --- PRIV ELEVATION LOGIC

leave_hack:
if(currcon != NULL)
	free(currcon);				


    // Let's assume that none of this code handles broken pipes. At least ls,
    // ps, and top were broken (though I'd previously added this fix locally
    // to top). We exit rather than use SIG_IGN because tools like top will
    // just keep on writing to nowhere forever if we don't stop them.
    signal(SIGPIPE, SIGPIPE_handler);

    if((argc > 1) && (argv[1][0] == '@')) {
        name = argv[1] + 1;
        argc--;
        argv++;
    } else {
        char *cmd = strrchr(argv[0], '/');
        if (cmd)
            name = cmd + 1;
    }

    for(i = 0; tools[i].name; i++){
        if(!strcmp(tools[i].name, name)){
            return tools[i].func(argc, argv);
        }
    }

    printf("%s: no such tool\n", argv[0]);
    return -1;
}
