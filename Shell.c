#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "Hamsof:- "

int execute(char* arglist[]);
char** tokenize(char* cmdline);
char* read_cmd(char*, FILE*);
bool builtin_cmds(char* arglist[]);


int main(){
   char *cmdline;
   char** arglist;
   char* prompt = PROMPT;
   int choice; 
   while((cmdline = read_cmd(prompt,stdin)) != NULL){
      if((arglist = tokenize(cmdline)) != NULL){
         if(!builtin_cmds(arglist)){
            execute(arglist);
         }
         
         
       //  need to free arglist
         for(int j=0; j < MAXARGS+1; j++)
	         free(arglist[j]);
         free(arglist);
         free(cmdline);
      }
  }//end of while loop
   printf("\n");
   return 0;
}


int execute(char* arglist[]){
   int status;
   int cpid = fork();
   switch(cpid){
      case -1:
         perror("fork failed");
	      exit(1);
      case 0:
      	
      	for(int i=0; arglist[i] != NULL; ++i){
      		//input redirection
		if (strcmp(arglist[i],"<")==0){
			int file = open(arglist[i + 1], O_RDONLY);
			
			if (file == -1 || arglist[i+1]  == NULL) {
				printf("Invalid Command!\n");
				exit(1);	
			}
			
			dup2(file, 0);
               	arglist[i] = NULL;
               	arglist[i + 1] = NULL;
               	break;
		}
		
		//output redirection
		else if (strcmp(arglist[i],">")==0){
			int file = open(arglist[i + 1], O_WRONLY | O_CREAT, 0644);
 			if (file == -1 || arglist[i+1]  == NULL) {
 				printf("Invalid Command!\n");
 				exit(1);
			}
			dup2(file, 1);
			arglist[i] = NULL;
 			arglist[i + 1] = NULL;
 			break;
		}
		
		
      	
      	}
      
      
      
	      execvp(arglist[0], arglist);
 	      perror("Command not found...");
	      exit(1);
      default:
	      waitpid(cpid, &status, 0);
         printf("child exited with status %d \n", status >> 8);
         return 0;
   }
}
char** tokenize(char* cmdline){
//allocate memory
   char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
   for(int j=0; j < MAXARGS+1; j++){
	   arglist[j] = (char*)malloc(sizeof(char)* ARGLEN);
      bzero(arglist[j],ARGLEN);
    }
   if(cmdline[0] == '\0')//if user has entered nothing and pressed enter key
      return NULL;
   int argnum = 0; //slots used
   char*cp = cmdline; // pos in string
   char*start;
   int len;
   while(*cp != '\0'){
      while(*cp == ' ' || *cp == '\t') //skip leading spaces
          cp++;
      start = cp; //start of the word
      len = 1;
      //find the end of the word
      while(*++cp != '\0' && !(*cp ==' ' || *cp == '\t'))
         len++;
      strncpy(arglist[argnum], start, len);
      arglist[argnum][len] = '\0';
      argnum++;
   }
   arglist[argnum] = NULL;
   return arglist;
}      

char* read_cmd(char* prompt, FILE* fp){
   printf("%s", prompt);
  int c; //input character
   int pos = 0; //position of character in cmdline
   char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
   while((c = getc(fp)) != EOF){
       if(c == '\n')
	  break;
       cmdline[pos++] = c;
   }
//these two lines are added, in case user press ctrl+d to exit the shell
   if(c == EOF && pos == 0) 
      return NULL;
   cmdline[pos] = '\0';
   return cmdline;
}

bool builtin_cmds(char* arglist[]){
   char* cmds_list[3];
   cmds_list[0] = "exit";
   cmds_list[1] = "cd";
   cmds_list[2] = "help";

   if(strcmp(arglist[0],cmds_list[0])==0){
      printf("Bye Bye from my shell\n");
      exit(0);
   }

   else if(strcmp(arglist[0],cmds_list[1])==0){
      chdir(arglist[1]);
      return true;
   }

   else if(strcmp(arglist[0],cmds_list[2])==0){
      printf("This is the help page\n");
      return true;
   }

   return false;

}