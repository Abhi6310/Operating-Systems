#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFERSIZE 1024

//removing the newline char from fgets input
//Refered to https://www.geeksforgeeks.org/fgets-function-in-c/ for help with fgets() in cases where the newline character threw everything off
void removeNewline(char *str){
    str[strcspn(str,"\n")]='\0';
}

int main(int argc,char *argv[]){
    //Command line arguments error handling
    if (argc!=2){
        fprintf(stderr,"Error:%sfilename\n",argv[0]);
        exit(1);
    }

    //Opening file for reading/writing and erroring as needed
    int fd = open(argv[1],O_RDWR);
    if(fd<0){
        fprintf(stderr,"%s Error:invalid filename/corrupted file'%s':%s\n",argv[0],argv[1],strerror(errno));
        exit(255);
    }

    char input[BUFFERSIZE];
    //Infinite loop for processing read, write, seek and exiting
    while(1){
        printf("Option (r for read, w for write, s for seek, e for exit): ");
        //Referred to https://www.geeksforgeeks.org/use-fflushstdin-c/ to solve errors in buffer responsiveness
        fflush(stdout);
        //Ctrl+D exiting
        if(fgets(input,sizeof(input),stdin)==NULL){
            printf("\nEOF:Exiting\n");
            break;
        }
        //trimming out '/n'
        removeNewline(input);

        //Skip empty input
        if(strlen(input)==0)
            continue;
        if(strcmp(input,"r")==0){
            //reading
            printf("Enter the number of bytes you want to read: ");
            fflush(stdout);
            if(fgets(input,sizeof(input),stdin)==NULL){
                printf("\nEOF:Exiting\n");
                break;
            }
            removeNewline(input);

            //Converting input to long int to store in buffer
            long numBytes=strtol(input,NULL,10);
            if(numBytes<=0){
                printf("Invalid number of bytes\n");
                continue;
            }

            //Buffer allocation with 1 extra byte for null
            char *buffer=malloc(numBytes+1);
            if(buffer==NULL){
                perror("malloc");
                continue;
            }

            ssize_t readBytes=read(fd,buffer,numBytes);
            if(readBytes<0){
                perror("read");
                free(buffer);
                continue;
            }
            //null-termination for buffer
            buffer[readBytes]='\0';

            printf("Read:%s\n",buffer);
            free(buffer);
        } 
        else if(strcmp(input,"w")==0){
            //writing with similar error handling as read
            printf("Enter the data you want to write: ");
            fflush(stdout);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("\nEOF:Exiting\n");
                break;
            }
            removeNewline(input);
            size_t len=strlen(input);
            if(len==0){
                printf("No data\n");
                continue;
            }
            ssize_t writeBytes=write(fd,input,len);
            if(writeBytes<0){
                perror("write");
                continue;
            }
            if((size_t)writeBytes!=len){
                fprintf(stderr,"Error:only %zd out of %zu bytes got written\n",writeBytes,len);
            }else{
                printf("Successful write\n");
            }
        } 
        else if(strcmp(input,"s")==0){
            //seek
            printf("Enter an offset value: ");
            fflush(stdout);
            if(fgets(input,sizeof(input),stdin)==NULL){
                printf("\nEOF:Exiting\n");
                break;
            }
            removeNewline(input);
            //Referenced https://www.ibm.com/docs/en/i/7.3?topic=lf-strtol-strtoll-convert-character-string-long-long-long-integer
            //for diffrentiating between conversions for a long int as opposed to long long
            long long offset=strtoll(input,NULL,10);

            printf("Enter a value for whence (0:SEEK_SET,1:SEEK_CUR,2:SEEK_END): ");
            fflush(stdout);
            if(fgets(input,sizeof(input),stdin)==NULL){
                printf("\nEOF:Exiting\n");
                break;
            }
            removeNewline(input);
            int whence=atoi(input);
            if(whence<0||whence>2){
                printf("Invalid whence value,only 0,1,or 2\n");
                continue;
            }
            //offset updating and seeking
            off_t newOffset=lseek(fd,offset,whence);
            if(newOffset==(off_t)-1){
                perror("lseek");
            }else{
                printf("offset set to %ld.\n",(long)newOffset);
            }
        }
        //Added 'e' to make exiting easier rather than ctrl+d
        else if(strcmp(input,"e")==0){
            printf("Exiting.\n");
            break;
        }
        else{
            //deault for invalid option
            printf("Invalid option, try again\n");
        }
    }

    //Closing the file
    if(close(fd)<0){
        perror("close");
        exit(1);
    }
    return 0;
}
