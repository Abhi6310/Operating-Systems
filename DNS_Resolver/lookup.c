//For compilation with for proper api
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MaxInputFiles 20
#define HostnameMaxLength 256

int main(int argc, char *argv[]){
    //Argument check at least 2 files needed
    if(argc<3){
        fprintf(stderr,"Input messed up: %s <logFile> <dataFile1> [dataFile2...up to %d]\n",argv[0],MaxInputFiles);
        return EXIT_FAILURE;
    }
    //input file # check
    if(argc-2>MaxInputFiles){
        fprintf(stderr,"Too many input files (max %d)\n",MaxInputFiles);
        return EXIT_FAILURE;
    }
    //opening the file on write mode
    FILE *logFile=fopen(argv[1],"w");
    if(logFile==NULL){
        fprintf(stderr,"Output File didn't open correctly'%s': %s\n",argv[1],strerror(errno));
        return EXIT_FAILURE;
    }
    //storing start time
    struct timeval startTime,endTime;
    if (gettimeofday(&startTime,NULL)!=0){
        fprintf(stderr,"Can't get start time %s\n",strerror(errno));
        fclose(logFile);
        return EXIT_FAILURE;
    }
    //Processing each data file by going line by line skipping over empty lines and empty files
    for (int fileIndex=2;fileIndex<argc;fileIndex++){
        const char *dataFilename=argv[fileIndex];
        FILE *dataFile=fopen(dataFilename,"r");
        if (dataFile==NULL) {
            fprintf(stderr,"invalid file %s\n",dataFilename);
            continue;
        }

        char hostname[HostnameMaxLength];
        //reading each line until error or EOF
        while (fgets(hostname,sizeof(hostname),dataFile)){
            hostname[strcspn(hostname,"\r\n")]='\0';
            if (hostname[0]=='\0'){
                continue;
            }
            //DNS lookup only with only IPv4 
            struct addrinfo hints;
            //needed to do this due to garbage values in struct
            memset(&hints,0,sizeof(hints));
            hints.ai_family=AF_INET;
            hints.ai_socktype=SOCK_STREAM;

            struct addrinfo *result=NULL;

            int lookupStatus=getaddrinfo(hostname,NULL,&hints,&result);

            if (lookupStatus!=0){
                fprintf(logFile, "%s, NOT_RESOLVED\n", hostname);
            } else {
                //pointer to IPv4 address
                struct sockaddr_in *addr4 = (struct sockaddr_in *)result->ai_addr;
                char ipString[INET_ADDRSTRLEN];
                //converting binary IP to string
                if (inet_ntop(AF_INET,&addr4->sin_addr,ipString,sizeof(ipString))==NULL) {
                    fprintf(logFile,"%s, NOT_RESOLVED\n",hostname);
                } else {
                    fprintf(logFile,"%s, %s\n",hostname,ipString);
                }
                //releasing memory after every success to avoid memory leaks
                freeaddrinfo(result);
            }
        }
        //error during reading
        if (ferror(dataFile)){
            fprintf(stderr,"Error reading from %s\n",dataFilename);
        }
        fclose(dataFile);
    }
    //this would only error if writing the buffered data got messed up
    if (fclose(logFile)!=0) {
        fprintf(stderr,"Error closing log file: %s\n",strerror(errno));
    }
    //finally calculate end time
    if (gettimeofday(&endTime, NULL) != 0) {
        fprintf(stderr,"Error getting end time: %s\n",strerror(errno));
        return EXIT_FAILURE;
    }
    //computing passed time and printing results
    double totalTime=(endTime.tv_sec-startTime.tv_sec)+(endTime.tv_usec-startTime.tv_usec)/1e6;
    printf("%s: total time is %.6f seconds\n",argv[0],totalTime);

    return EXIT_SUCCESS;
}
