#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) {
    int process;
    int currentPage;
    int page;

    //Tracking Variables
    static int initialized=0;
    static int tick=1;
    static int LastAccessed[MAXPROCESSES];
    //Stochstic markov matrix 
    static int markov[MAXPROCESSES][20][20];
    
    if(!initialized){
        for(process=0;process<MAXPROCESSES;process++){
            LastAccessed[process] = -1;
            //default values for markov matrix for each page
            for(int i=0;i<20;i++){
                for (int j=0;j<20;j++){
                    markov[process][i][j]=0;
                }
            }
        }
        //Ensuring static variables are only initialized once
        initialized=1;
    }
    //Individual process handling skipping for inactive processes
    for( process= 0;process<MAXPROCESSES;process++){
        if(!q[process].active){
            continue;
        }
        currentPage=q[process].pc / 128;
        //Update matrix if last page exists and is different from the current one
        if(LastAccessed[process]!=-1 && LastAccessed[process]!=currentPage){
            markov[process][LastAccessed[process]][currentPage]++;
        }
        LastAccessed[process]=currentPage;

        //Paging in current page to memory, if it isn't already
        if(!q[process].pages[currentPage])
            pagein(process, currentPage);

        //Marked array of pages that need to stay in memory
        int keep[20]={0};
        keep[currentPage]=1;

        //Markov chains for most likely page to be accessed
        int maxCount=0;
        int predictedPage=-1;
        int total=0;
        for(page=0; page<q[process].npages; page++){ //Iterate over all pages summing transitions to keep the highest count recorded
            total+=markov[process][currentPage][page];

            if(markov[process][currentPage][page]>maxCount){
                maxCount = markov[process][currentPage][page];
                predictedPage = page;
            }
        }
        if (total>0){
            //page transitioned, so confidence can be calculated
            double confidence= (double)maxCount/total;
            if (confidence>= 0.5 && predictedPage!=-1){
                //case where confidence is high enough so we can reasonably assume the predicted page needs to be kept in memory
                keep[predictedPage]=1;
            }
        }
        //Look-Ahead prediction using probability of transitioning from current page to new page
        for(page=0;page<q[process].npages; page++){
            if(markov[process][currentPage][page]>0){
                keep[page] = 1;
                double prob=(double)markov[process][currentPage][page]/total;
                //if probability is high, mark for next level
                if(prob>0.8){
                    int total2=0;
                    for(int j=0;j<q[process].npages;j++){
                        total2+=markov[process][page][j];
                    }
                    for(int j=0;j<q[process].npages;j++){
                        if(total2>0 && markov[process][page][j]>0){
                            keep[j]=1;
                        }
                    }
                }
            }
        }
        //mark next sequential page for prefetching
        int prefetchPage=currentPage+1;
        if(prefetchPage<q[process].npages){
            keep[prefetchPage]=1;
        }
        //Pagein marked processes that aren't in yet
        for(page=0;page<q[process].npages;page++){
            if (keep[page] && !q[process].pages[page]){
                pagein(process,page);
            }
        }
        //Pageout unmarked processes
        for(page=0;page<q[process].npages;page++){
            if(!keep[page] && q[process].pages[page]){
                pageout(process,page);
            }
        }
    }
}
