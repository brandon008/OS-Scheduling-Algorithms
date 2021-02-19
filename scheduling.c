#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


int main(int argc,char *argv[]){
  //INPUT
  char s[500];
  FILE *myfile;
  sprintf(s,"%s",argv[1]);
  myfile = fopen(s,"r"); //TODO
  char ret[50] = "";
  char line1[50];
  fgets(line1,50,myfile);
  char nc = *strtok(line1," ");
  int n = atoi(&nc);
  char line[50];
  char outFile[1000];

  int process[3][5];
  for(int i=0; i<n; i++){
    fgets(line,50,myfile);
    char vc = *strtok(line," ");
    int v = atoi(&vc);
    process[i][0] = v;

    printf("%d ",v);
    for(int j=1; j<4; j++){
      vc = *strtok(NULL," ");
      int v = atoi(&vc);
      process[i][j] = v;
      printf("%d ",v);
    }

    process[i][4] = ceil(process[i][1]/2);
    process[i][1] = ceil(process[i][1]/2); //allocates runtime to half of block time
    printf("\n");
  }
  fclose(myfile);

    if(strcmp(argv[2],"0")==0){
      int util = 0; //num of cycles uized
      int queue[3] = {-1,-1,-1};
      int turnAround[3] = {-1,-1,-1};
      bool done = true;
      int timer = 0; //total num of cycles (+1)
      int status[3];

      while(done){
        sprintf(s,"%d ",timer);
        strcat(outFile,s);

        for(int i=0; i<n; i++){
          if(timer == process[i][3]){ //arrival
            int j = 0;
            while(queue[j] != -1){
              j++;
            }
            queue[j] = i;
          }
          else if(process[i][1] == 0 && process[i][2] == 0 && process[i][4]>0){
            bool z = false;
            for(int k = 0; k<n; k++){
              if(i == queue[k]){
                z = true;
              }
            }
            if(!z){
              int j = 0;
              while(queue[j] != -1){
                j++;
              }
              queue[j] = i;
            }
          }
        }
        bool cpuUsed = false;
        for(int i=0; i<n; i++){
          int j = queue[i];
          // printf("%d",j);
          if(j == -1){
            break;
          }
          if(process[j][1]>0){ //1st half CPU time
            if(!cpuUsed){
              status[j]=3;

              process[j][1]= process[j][1]-1;
              cpuUsed = true;
            } else{
              status[j]=1;

            }
          }
          else if(process[j][2]>0){ // i/o time
            queue[i] = -1;
        } else if(process[j][4]>0){ // 2nd half of CPU time
          if(!cpuUsed){
            status[j]=3;

            process[j][4]= process[j][4]-1;
            cpuUsed = true;
            if(process[j][4]==0){
              status[j]=5;
              queue[i] = -1;
              turnAround[j] = timer-process[j][3]+1;
            }
          } else{
            status[j]=1;

          }
        } else{
            status[j]=4;
            queue[i] = -1; //process is done
          }
        }
        for(int j = 0; j<n;j++){
          if(process[j][2]>0 && process[j][3]<=timer){
            bool z = false;
            for(int k = 0; k<n; k++){

              if(j==queue[k]){
                z = true;
              }
            }
            if(!z){
              status[j]=2;
              // printf("%d:blocked ", j);
              process[j][2]= process[j][2]-1;
            }
          }
        }
        if(cpuUsed){
          util+=1;

        }
        for(int i = 0; i<n; i++){
          switch (status[i]){
            case 1:
              sprintf(s," %d:ready",i);
              strcat(outFile,s);
            break;

            case 2:
              sprintf(s," %d:blocked",i);
              strcat(outFile,s);
            break;

            case 3:
              sprintf(s," %d:running",i);
              strcat(outFile,s);
            break;

            case 5:
              sprintf(s," %d:running",i);
              strcat(outFile,s);
              status[i]=4;
            break;

            default:
            break;
          }
        }
        sprintf(s,"\n");
        strcat(outFile,s);
        if(queue[1]==-1 && queue[2]>-1){
          queue[1]=queue[2];
          queue[2]= -1;
        }
        if(queue[0]==-1){
          if(queue[1]>-1){
            queue[0]=queue[1];
            queue[1]=queue[2];
            queue[2]= -1;
          } else{
            bool allDone = true;
            for(int i=0; i<n; i++){
              if(turnAround[i]==-1){
                allDone = false;
              }
            }
            if(allDone){
              // print and exit
              sprintf(s,"\nFinishing Time: %d\n",timer);
              strcat(outFile,s);
              float m = (float)util/((float)timer+1.0);
              sprintf(s,"CPU utilization:%f\n",m);
              strcat(outFile,s);

              for(int i=0; i<n; i++){
                sprintf(s,"Turnaround process %d: %d\n",i,turnAround[i]);
                strcat(outFile,s);

              }
              printf("%s",outFile);
              FILE *yourfile;
              char t[100];
              strncpy(t,argv[1],strlen(argv[1])-4);
              sprintf(s,"%s-%s.txt",t,argv[2]);
              yourfile = fopen(s,"w"); //TODO
              fputs(outFile,yourfile); //TODO
              fclose(yourfile);
              exit(0);
            }
          }
        }

        //if then condition so if !cpuUSed and robinCOunt is high,
        //don't increment the timer
        timer+=1;
      }
    } // scheduling algo 1 (nonpreemptive)
  else if(strcmp(argv[2],"1")==0){ //ROUND ROBIN WITH QUANTUM 2
    int robinCount = 0;
    int robinID = 0;
    int util = 0; //num of cycles uized
    int queue[3] = {-1,-1,-1};
    int turnAround[3] = {-1,-1,-1};
    bool done = true;
    int timer = 0; //total num of cycles (+1)
    bool readyQueue[3] = {false,false,false};
    int status[3];

    while(done){
      sprintf(s,"%d ",timer);
      strcat(outFile,s);

      for(int i=0; i<n; i++){
        if(timer == process[i][3]){ //arrival
          int j = 0;
          while(queue[j] != -1){
            j++;
          }
          queue[j] = i;
        }
        else if((process[i][1] == 0 && process[i][2] == 0 && process[i][4]>0)){
          bool z = false;
          for(int k = 0; k<n; k++){
            if(i == queue[k]){
              z = true;
            }
          }
          if(!z){
            int j = 0;
            while(queue[j] != -1){
              j++;
            }
            queue[j] = i;
          }
        }
        else if(robinID == i && robinCount>1){
          for(int k = 0; k<n; k++){ //check where we are in queue
            if(i == queue[k]){
              switch (k){
                case 0:
                  queue[0] = queue[1];
                case 1:
                  queue[1] = queue[2];
                case 2:
                  queue[2] = i; //flipping queue
                default:
                  if(queue[1]==-1 && queue[2]>-1){
                    queue[1]=queue[2];
                    queue[2]= -1;
                  }
                  if(queue[0]==-1){
                    if(queue[1]>-1){
                      queue[0]=queue[1];
                      queue[1]=queue[2];
                      queue[2]= -1;
                    }
                  }
              }
            }
          }
        }
      }
      bool cpuUsed = false;
      for(int i=0; i<n; i++){
        int j = queue[i];

        if(j == -1){
          break;
        }
        if(process[j][1]>0){ //1st half CPU time
          if(!cpuUsed && (robinID != j || robinCount<2)){
            status[j] = 3;

            process[j][1]= process[j][1]-1;
            cpuUsed = true;
            if(robinID!=j){ //
              robinID = j;
              robinCount = 0;
            }
            robinCount+=1;

          } else{
              status[j]=1;

            if(robinID == j && robinCount ==2){
              queue[i] = -1; // drops from queue
            }
          }
        }
        else if(process[j][2]>0){ // i/o time
          queue[i] = -1;
      } else if(process[j][4]>0){ // 2nd half of CPU time
        if(!cpuUsed){
          status[j]=3;

          process[j][4]= process[j][4]-1;
          cpuUsed = true;
          if(robinID!=j){ //
            robinID = j;
            robinCount = 0;
          }
          robinCount+=1;
          if(process[j][4]==0){
            queue[i] = -1;
            status[j] = 5;
            turnAround[j] = timer-process[j][3]+1;
          }
        } else{
          status[j]=1;

        }
      } else{
          queue[i] = -1; //process is done
          status[j] = 4;
        }
      }
      for(int j = 0; j<n;j++){
        if(process[j][2]>0 && process[j][3]<=timer && process[j][1]==0){
          bool z = false;
          for(int k = 0; k<n; k++){

            if(j==queue[k]){
              z = true;
            }
          }
          if(!z){
            status[j]=2;
            // printf("%d:blocked ", j);
            process[j][2]= process[j][2]-1;
          }
        }
      }
      if(cpuUsed){
        util+=1;

      }
      //HERE
      for(int i = 0; i<n; i++){
        switch (status[i]){
          case 1:
            sprintf(s," %d:ready",i);
            strcat(outFile,s);
          break;

          case 2:
            sprintf(s," %d:blocked",i);
            strcat(outFile,s);
          break;

          case 3:
            sprintf(s," %d:running",i);
            strcat(outFile,s);
          break;

          case 5:
            sprintf(s," %d:running",i);
            strcat(outFile,s);
            status[i]=4;
          break;

          default:
          break;
        }
      }
      sprintf(s,"\n");
      strcat(outFile,s);
      if(queue[1]==-1 && queue[2]>-1){
        queue[1]=queue[2];
        queue[2]= -1;
      }
      if(queue[0]==-1){
        if(queue[1]>-1){
          queue[0]=queue[1];
          queue[1]=queue[2];
          queue[2]= -1;
        } else{
          bool allDone = true;
          for(int i=0; i<n; i++){
            if(turnAround[i]==-1){
              allDone = false;
            }
          }
          if(allDone){
            // print and exit

            sprintf(s,"\nFinishing Time: %d\n",timer);
            strcat(outFile,s);

            float m = ((float)round(((float)util/((float)timer+1.0))*100)/(float)100);
            sprintf(s,"CPU utilization:%f\n",m);
            strcat(outFile,s);
            for(int i=0; i<n; i++){
              sprintf(s,"Turnaround process %d: %d\n",i,turnAround[i]);
              strcat(outFile,s);
            }
            printf("%s",outFile);
            FILE *yourfile;
            char t[100];
            strncpy(t,argv[1],strlen(argv[1])-4);
            sprintf(s,"%s-%s.txt",t,argv[2]);
            yourfile = fopen(s,"w"); //TODO
            fputs(outFile,yourfile); //TODO
            fclose(yourfile);
            exit(0);
          }
        }
      }
      timer+=1;
    }
  }
  else if(strcmp(argv[2],"2")==0){ //Shortest Remaining Job First
    int util = 0; //num of cycles uized
    int turnAround[3] = {-1,-1,-1};
    bool done = true;
    int timer = 0; //total num of cycles (+1)
    int status[3];

    while(done){
      bool cpuUsed = false;

      for(int j=0; j<n; j++){
        if(process[j][3]>timer){ //1st half CPU time
          status[j]=0; //NOTready
        }
        else if(process[j][1]>0){ //1st half CPU time
          status[j]=1; //ready
        }
        else if(process[j][2]>0){ // i/o time
          status[j]=2; //blocked
          process[j][2] = process[j][2]-1;
      } else if(process[j][4]>0){ // 2nd half of CPU time
        status[j]=1; //ready
      } else{
        status[j]=4; // done
        if(turnAround[j]==-1){
          turnAround[j] = timer-process[j][3]+1;
        }
      }
    }
    int minTime = -1; //least runtime remaining for any process
    int runner = -1; //ID of process that runs *this cycle*
    int totalCycles = -1; //total CPU cycles left for given process

    for(int i = 0; i<n; i++){
      if(status[i] == 1){
        totalCycles = process[i][1] + process[i][4];
        if(minTime == -1 || totalCycles<minTime){
          minTime = totalCycles;
          runner = i;
        }
      }
    }
    if(minTime>0){
      int r = runner;
      status[r]=3;//running
      if(process[r][1]>0){
        process[r][1]=process[r][1]-1;
      }else{
        process[r][4]=process[r][4]-1;
      }
      util+=1;
    }

      bool allDone = true;
      for(int i=0; i<n; i++){
        if(status[i]!=4){ //not done
          allDone = false;
        }
      }

          if(allDone){
            // print and exit
            timer -= 1;
            sprintf(s,"\nFinishing Time: %d\n",timer);
            strcat(outFile,s);
            float m = (float)util/((float)timer+1.0);
            sprintf(s,"CPU utilization:%f\n",m);
            strcat(outFile,s);

            for(int i=0; i<n; i++){
              sprintf(s,"Turnaround process %d: %d\n",i,turnAround[i]-1);
              strcat(outFile,s);
            }
            printf("%s",outFile);
            FILE *yourfile;
            char t[100];
            strncpy(t,argv[1],strlen(argv[1])-4);
            sprintf(s,"%s-%s.txt",t,argv[2]);
            yourfile = fopen(s,"w");
            fputs(outFile,yourfile);
            fclose(yourfile);
            exit(0);
          }
          sprintf(s,"%d ",timer);
          strcat(outFile,s);

          for(int i = 0; i<n; i++){
            switch (status[i]){
              case 1:
                sprintf(s," %d:ready",i);
                strcat(outFile,s);
              break;

              case 2:
                sprintf(s," %d:blocked",i);
                strcat(outFile,s);
              break;

              case 3:
                sprintf(s," %d:running",i);
                strcat(outFile,s);
              break;

              default:
              break;
            }

          }

      sprintf(s,"\n");
      strcat(outFile,s);

      timer+=1;
    }
  }

  //OUTPUT


  FILE *yourfile;

  sprintf(s,"%s-%s",argv[1],argv[2]);
  yourfile = fopen(s,"w");

  fputs(outFile,yourfile);
  fclose(yourfile);
}
