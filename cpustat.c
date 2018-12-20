/*
 * Displays linux /proc/{pid}/stat in human-readable format
 * Build: gcc -o cpustat cpustat.c
 * 
 *       tickspersec are taken from sysconf
 *

 */


/* Copyright (C) 2018 Shieryn Tjandra
*/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/times.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>       /* time_t, time (for timestamp in second) */
#include <sys/timeb.h>  /* ftime, timeb (for timestamp in millisecond) */
#include <sys/time.h>   /* gettimeofday, timeval (for timestamp in microsecond) */
#include <stdint.h> /* for uint64 definition */
#include<regex.h>
#define BILLION 1000000000L
typedef long long int num;
//declaration
int pid;int pid2;
char tcomm[25]; //procname
char newtcomm[25];
char state;
int ppid;
num pgid;
num sid;
num tty_nr;
num tty_pgrp;
num flags;
num min_flt;
num cmin_flt;
num maj_flt;
num cmaj_flt;
num utime;
num stimev;
num cutime;
num cstime;
num priority;
num nicev;
num num_threads;
num it_real_value;

unsigned long long start_time;

num vsize;
num rss;
num rsslim;
num start_code;
num end_code;
num start_stack;
num esp;
num eip;

num pending;
num blocked;
num sigign;
num sigcatch;
num wchan;
num zero1;
num zero2;
num exit_signal;
num cpu;
num rt_priority;
num policy;

long tickspersec;
char *temp;

char *proc;
char dir[50];
char cwd[PATH_MAX];
FILE *input;
FILE *inputproc;


void readone(num *x) { fscanf(input, "%25d ", x); }
void readunsigned(unsigned long long *x) { fscanf(input, "%llu ", x); }
void readstr(char *x) {  fscanf(input, "%s ", x);}
void readone2(num *x) { fscanf(inputproc, "%25d ", x); }
void readstr2(char *x) {  fscanf(inputproc, "%[^)]", x);}
void readchar(char *x) {  fscanf(input, "%c ", x);}

void printone(char *name, num x) {  printf("%s %d,", name, x);}
void printonex(char *name, num x) {  printf("%s: %016llx\n", name, x);}
void printunsigned(char *name, unsigned long long x) {  printf("%s: %llu\n", name, x);}
void printchar(char *name, char x) {  printf("%s:%c\n", name, x);}
void printstr(char *name, char *x) {  printf("%s %s),", name, x);}
void printtime(char *name, num x) {  printf("%s %f,", name, (((double)x) / tickspersec));}
//tcomm pattern

void cputime(num utime,num stime, num cutime,num cstime) 
{
  double utime_1,stime_1,cutime_1,cstime_1,cputime;
   utime_1  = ((double) utime)/ tickspersec;
   stime_1  = ((double) stime)/ tickspersec;
   cutime_1  = ((double) cutime)/ tickspersec;
   cstime_1  = ((double) cstime)/ tickspersec;

   cputime = utime_1+stime_1+cutime_1+cstime_1;

 printf("cpu %.6f\n",cputime);
}

//It scans /proc for all PID's and prints them. Instead of printing the PID you can put any code to
// get more info from the /proc/<PID> directory by changing the processdir() function.
void processdir(const struct dirent *dir)
{
  //char flag[] = "/";
  char proc[] = "/proc/";
    temp=&dir->d_name;
    strcat(proc,temp);
    chdir(proc);
    getcwd(cwd, sizeof(cwd));
  input=fopen("stat","r");
  inputproc=fopen("stat","r");
        readone(&pid);
	readone2(&pid2);
      	readstr(tcomm);
	readstr2(newtcomm);
        readchar(&state);
        readone(&ppid);
        readone(&pgid);
        readone(&sid);
        readone(&tty_nr);
        readone(&tty_pgrp);
        readone(&flags);
        readone(&min_flt);
        readone(&cmin_flt);
        readone(&maj_flt);
        readone(&cmaj_flt);
        readone(&utime);
        readone(&stimev);
        readone(&cutime);
        readone(&cstime);
        readone(&priority);
        readone(&nicev);
        readone(&num_threads);
        readone(&it_real_value);
        readunsigned(&start_time);
        readone(&vsize);
        readone(&rss);
        readone(&rsslim);
        readone(&start_code);
        readone(&end_code);
        readone(&start_stack);
        readone(&esp);
        readone(&eip);
        readone(&pending);
        readone(&blocked);
        readone(&sigign);
        readone(&sigcatch);
        readone(&wchan);
        readone(&zero1);
        readone(&zero2);
        readone(&exit_signal);
        readone(&cpu);
        readone(&rt_priority);
        readone(&policy);

}

int filter(const struct dirent *dir)
{
     return !fnmatch("[0-9]*", dir->d_name, 0);
}

int main() {
		//timestamp
		char buffer[30];
		const char* times="0.000000";
		double cpu;
		uint64_t diff; 
		struct timespec now;
		/* measure realtime time */ 
		clock_gettime(CLOCK_REALTIME,&now); 
		struct timeval tv;
		time_t curtime;
		gettimeofday(&tv, NULL); 
		curtime=tv.tv_sec;
		strftime(buffer,30,"%m-%d-%Y_%T.",localtime(&curtime));
		printf("Local Time : %s%u\n",buffer,(long long unsigned int) now.tv_nsec);
			//convert to UTC,EST 
		time_t mytime;

		struct tm * ptm;
		time ( &mytime ); // Get local time in time_t
		ptm = gmtime ( &mytime ); // Find out UTC time
		putenv("TZ=UTC");
		time_t utctime = mktime(ptm); // Get UTC time as time_t
		printf("\nUTC Time %s\n",  ctime(&utctime));
		putenv("TZ=EST5EDT");
		printf("\nEST Time %s\n",  ctime(&utctime));
		tickspersec = sysconf(_SC_CLK_TCK);
		input = NULL;
		struct dirent **namelist;
		int n;
		n = scandir("/proc", &namelist, filter, 0);
		if (n < 0)
		   perror("Not enough memory.");
		else {  
		    while(n--) {
		        processdir(namelist[n]);
		          	const char* s1=tcomm;
				
				            printone("pid", pid);
				            printstr("", newtcomm);
				            //printtime("u", utime);
				            //printtime("s", stimev);
				            //printtime("cu", cutime);
				            //printtime("cs", cstime);
				            cputime(utime,stimev,cutime,cstime);

		        free(namelist[n]);
		      }
		    free(namelist);
		   }

	return 0;
}
