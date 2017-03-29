#include <err.h>
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <android/log.h>
#include <linux/fs.h>

#include "shared.h"

int search(char *, char *);
// returns 0 on success, other on failure 
int copy(char *dst, char *src);

int main(int argc, const char *argv[]) {
	int fd; 

	if( rsf_set(RSF_CACHE_TO_DATA_WORKING) ) {
		LOGV("[*] bridge started");
		    rsf_unset(RSF_PANIC);



//FOLLOWING CODE: meant to run once it will loop at the bottom

  /* Allocate memory for the copy lines */
    int max_num_of_copies = 31;
    int copy_path_length = 1023;
    int max_line_length = 2047;
    
    //reserve memory and test if we ran out
    char **bridgeSrc = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (bridgeSrc==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        LOGV("Ran out of memory allocating bridgeSrc");        
    	exit(EXIT_FAILURE);
        }

    char **bridgeDst = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (bridgeDst==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        LOGV("Ran out of memory allocating bridgeDst");        
        exit(EXIT_FAILURE);
        }
    char **ddFileLine = (char **)malloc(sizeof(char*)*max_num_of_copies);
    if (ddFileLine==NULL)
        {
        fprintf(stderr,"Out of memory (1).\n");
        LOGV("Ran out of memory allocating ddFileLine");
        exit(EXIT_FAILURE);
        }        

	//when pull session target.. should match your /dev/block/ name
	char *	  target="if=/dev"; //if we find this in the left most position it is a pull session

	char *	  tmp_file_path="/data/local/tmp/files.txt";
	char *	  cache_file_path="/cache/recovery/files.txt";

    pid_t cpid;
    pid_t child_pid;
    
    //used to determine push/pull
    int loc;
	
	//while loops    
    int iLoop=0;
    int iCntr=0; 
    
   //fgets(line
   char line[max_line_length];

	//open the files list.
   FILE *fpBlock;
   fpBlock = fopen(tmp_file_path,"r");
   if (fpBlock == NULL) {
   LOGV("Couldn't open /data/local/tmp/files.txt");
    rsf_set(RSF_PANIC);
   	exit(EXIT_FAILURE);
   }





//===>[-] Parse text file for bridge specific duties [-]<===
   while (fgets(line, sizeof(line), fpBlock)) {
        /* Allocate space for the next line */
        bridgeSrc[iCntr] = malloc(copy_path_length);
        bridgeDst[iCntr] = malloc(copy_path_length);
        ddFileLine[iCntr] = malloc(max_line_length);        

	//return entire line up to | to test if copy/pull, then discard this value it's for toolbox.c
		strcpy(ddFileLine[iCntr],strtok(line, "|"));
		//this part is used for bridge
		strcpy(bridgeSrc[iCntr], strtok(NULL, "|"));
		strcpy(bridgeDst[iCntr], strtok(NULL, "\n"));
	//if there are no more lines of text, break while	
	if (bridgeSrc[iCntr] == NULL || bridgeDst[iCntr] == NULL) {
		break;
	}
		//copy or pull?
		if(iCntr == 0) {
  		loc = search(ddFileLine[iCntr], target);
  		  		if (loc >= 0) {
					rsf_set(RSF_IS_PULL);
					LOGV("Bridge: Pull session detected.");
				}
		}
  
	iCntr++;
}  
//we're done with the files list text file ok to close now
if(fpBlock != NULL){				        
fclose(fpBlock);	
}
//bridge copies files.txt to cache so that toolbox.c is able to dynamically read it
if (rsf_set(RSF_SENT_FILE_LIST)) {
			if( copy(cache_file_path, tmp_file_path) ) {
			LOGV("ERROR: could not copy %s to %s", cache_file_path, tmp_file_path);
			rsf_set(RSF_PANIC);
		}else{LOGV("Successfully copied %s %s",cache_file_path, tmp_file_path);rsf_set(RSF_SENT_FILE_LIST);}  //files list sent, rsf_set
}


if (rsf_check(RSF_IS_PULL) && !rsf_check(RSF_PANIC)) {
//////////////////////////=====PULL=====
	while (iLoop < iCntr) {
		//first loop logic
		if (iLoop == 0) {
			//toolbox goes first, set this and wait for toolbox to unset it, then pull to tmp
				rsf_set(RSF_BRIDGE_READY);
				LOGV("Bridge: DON'T push Allow MTP connection during this process!!!");
		}	
			//wait for toolbox to link this const char, which is our stop/go
			while (!rsf_check(RSF_TOOLBOX_READY)) {sleep(1);}

	if (rsf_check(RSF_PANIC)){
		LOGV("Bridge detected panic!! breaking loop.");
		break;
	}
			
  
    //attempts to fork the current process returning the results as pid_t, which is basically int
    cpid = fork();
    //let's grab our child process
    switch (cpid) {
        case -1: LOGV("Couldn't fork a process which is weird because we should be able to");
                 break;
	//means we successfully forked, let's retrieve the process id
        case 0: child_pid = getpid();

//<><><> EXECUTE COMMANDS <><><> <TODO> add error handling of some kind especially when pushing.
		LOGV("[*] bridge copying image");
		//copy destination source
		if( copy(bridgeDst[iLoop],bridgeSrc[iLoop]) ) {
			LOGV("PANIC: could not copy %s to %s", bridgeSrc[iLoop], bridgeDst[iLoop]);
			rsf_set(RSF_PANIC);
			break;
		}else{
		
		return (EXIT_SUCCESS); /* let's close out the child process to keep logic similiar to toolbox.c */
		}


        default: //LOGV("dumpstate--bridge pid: %d is waiting for forked pid: %d",getppid(), cpid);
                 waitpid(cpid, NULL, 0);
	}

			if (rsf_check(RSF_TOOLBOX_READY)){
				rsf_unset(RSF_TOOLBOX_READY);
			}
		rsf_set(RSF_BRIDGE_READY);
		iLoop++;
	}		

}else{
//PUSH
	while (iLoop < iCntr) {

if (iLoop > 0) {
			//wait for toolbox to link this const char, which is our stop/go
			while (!rsf_check(RSF_TOOLBOX_READY)) {sleep(1);}
}			

	if (rsf_check(RSF_PANIC)){
		LOGV("Bridge detected panic!! breaking loop.");
		break;
	}
			
  
    //attempts to fork the current process returning the results as pid_t, which is basically int
    cpid = fork();
    //let's grab our child process
    switch (cpid) {
        case -1: LOGV("PANIC: Couldn't fork a process which is weird because we should be able to");
        		 rsf_set(RSF_PANIC);
                 break;
	//means we successfully forked, let's retrieve the process id
        case 0: child_pid = getpid();

//<><><> EXECUTE COMMANDS <><><> <TODO> add error handling of some kind especially when pushing.
		LOGV("[*] bridge copying image");
		//copy destination source
		if( copy(bridgeDst[iLoop],bridgeSrc[iLoop]) ) {
			LOGV("PANIC: could not copy %s to %s", bridgeSrc[iLoop], bridgeDst[iLoop]);
			rsf_set(RSF_PANIC);
			break; //let's attempt to stop toolbox by holding panic signal open
		}else{
		
		return (EXIT_SUCCESS); /* let's tag out the child process to keep logic similiar to toolbox.c */
		}


        default: LOGV("dumpstate--bridge pid: %d is waiting for forked pid: %d",getppid(), cpid);
                 waitpid(cpid, NULL, 0);
	}
		//bridge has copied the file out let's delete it and then wait for toolbox again.
						//<TODO> delete images after pull, or rewrite them to 1 byte file size whichever is easier.
			if (rsf_check(RSF_TOOLBOX_READY)){
				rsf_unset(RSF_TOOLBOX_READY);
			}
		rsf_set(RSF_BRIDGE_READY);
		LOGV("*** bridge is waiting for toolbox to copy the first image to cache ***");
		iLoop++;
	}




}
	//END PUSH/PULL LOGIC

						
//		rsf_set(RSF_CACHE_TO_DATA_DONE);
		rsf_set(RSF_ALL_DONE);
	}
if (rsf_check(RSF_PANIC)){
			LOGV("PANIC: attempting to close");
			int iiCntd;
			while (iiCntd < 9){
			sleep(1);
			}
}
	return 0;
}



//searches a string and returns it's position -- this is used to determine push or pull
//basically: I split the lines left/right if= / of= ... whenever if=/dev it's a 
int search(char *src, char *string_to_search) {
   int i, j, firstOccurence;
   i = 0, j = 0;
 
   while (src[i] != '\0') {
 
      while (src[i] != string_to_search[0] && src[i] != '\0')
         i++;
 
      if (src[i] == '\0')
         return (-1);
 
      firstOccurence = i;
 
      while (src[i] == string_to_search[j] && src[i] != '\0' && string_to_search[j] != '\0') {
         i++;
         j++;
      }
 
      if (string_to_search[j] == '\0')
         return (firstOccurence);
      if (src[i] == '\0')
         return (-1);
 
      i = firstOccurence + 1;
      j = 0;
   }
return -1;   
}
//bridge copy function
int copy(char *dst, char *src) {
	int dst_fd, src_fd; 
	size_t src_len;
	ssize_t total;

	// open files
	dst_fd = open(dst, O_WRONLY | O_TRUNC | O_CREAT, 0777);
	src_fd = open(src, O_RDONLY);

	// check for open errors
	if( dst_fd == -1 || src_fd == -1 ) {
		if( dst_fd != -1 )
			close(dst_fd);
		if( src_fd != -1 )
			close(src_fd);
		return -1;
	}

	// get src_len
	if( (src_len = (size_t) lseek(src_fd, 0, SEEK_END)) == (size_t)-1 ) {
		close(src_fd); close(dst_fd);
		return -1;
	}
	lseek(src_fd, 0, SEEK_SET);

	// lazy copy file
	total = sendfile(dst_fd, src_fd, NULL, src_len);
	close(dst_fd);
	close(src_fd);

	chmod(dst, 0701);
	chmod(dst, 0702);
	chmod(dst, 0703);
	chmod(dst, 0704);
	chmod(dst, 0705);
	chmod(dst, 0706);
	chmod(dst, 0707);

	// return 0 on success
	return total != (ssize_t)src_len;
}
