#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<ctype.h>

char error_message[30] = "An error has occurred\n"; //Error Message
int closed = 0, emptyPath = 0, numOfMultiPath = 0, batch = 0, changedPath = 0; //Integer variables
char MULTI_PATH[512][512]; //Multiple path
int BUFFER_SIZE= 512;
char *path;

void welcome() {
    printf("\n");
    printf ("     .oooooo..o oooo                  oooo  oooo  \n");
    printf("    d8P'    `Y8 `888                  `888  `888  \n");
    printf("    Y88bo.       888 .oo.    .ooooo.   888   888  \n");
    printf("     `\"Y8888o.   888P\"Y88b  d88' `88b  888   888  \n");
    printf("         `\"Y88b  888   888  888ooo888  888   888  \n");
    printf("    oo     .d8P  888   888  888    .o  888   888  \n");
    printf("      8""88888P'  o888o o888o `Y8bod8P' o888o o888o \n");
    printf("\n");
    printf("            Welcome to our TEDShell \n\n");
    printf("                    Furkan OZELGE\n\n");

}

void myPrompt(){ //The method is used to write prompt.
        write(STDOUT_FILENO, "TEDShell> ", strlen("TEDShell> ")); //It writes our shell name which is called TEDShell as Standart Output.
}

void errorMsg(){ //The method is used to write error message.
        write(STDERR_FILENO, error_message, strlen(error_message)); //It writes the error message which is defined.
        exit(1);

}

int spaceController(char* buffer){ //The method is used to control empty (white) spaces.
        int flag=0;
        int i=0;
        while(i<strlen(buffer)){
                if(isspace(buffer[i])==0){ //It checks is character is white space or not.
                    flag=1;
                    break;
                }
            i=i+1; //Check every character until the end of the length of buffer.
        }
        return flag; //If there is no white space, then it returns 0.
}

int createProcess(char *myargs[]) { //The method is used to create process which are called child process.
        int cpid=fork(); //fork() is used to create a new process, which becomes the child process of the caller.
        if(cpid<0){ //It gives an error.
                errorMsg(); //Error message is printed.
                exit(1);
        }
        else if(emptyPath!=1 && cpid==0){ //Child process
                if(changedPath==0){
                        path=strdup("/bin/"); // It is used to duplicate a string which is "/bin/"
                        path=strcat(path,myargs[0]); // myargs is copied to path.
                        if(access(path,X_OK)!=0 && changedPath==0){ //It uses to test for access to path. It checks if path can be executed.
                                path=strdup("/usr/bin/"); // It is used to duplicate a string which is "/usr/bin/"
                                path=strcat(path,myargs[0]); // myargs is copied to path.
                                if(access(path,X_OK)!=0){ //It uses to test for access to path. It checks if path can be executed.
                                        write(STDERR_FILENO, error_message, strlen(error_message)); //If not, it will give error message.
                                        exit(0);
                                }
                        }
                  }
                  else if(numOfMultiPath==0 && changedPath==1){ //If there is no multiple path and one changed path
                        path=strcat(path,myargs[0]); // myargs is copied to path.
                        if(access(path,X_OK)!=0){ //It uses to test for access to path. It checks if path can be executed.
                                        write(STDERR_FILENO, error_message, strlen(error_message)); //If not, it will give error message.
                                        exit(0);
                                }
                  }
                  else{ // If there are multiple paths
                        for(int x=0;x<numOfMultiPath;x=x+1){ // x is incremented until to reach number of multiple paths
                                strcat(MULTI_PATH[x],myargs[0]); // myargs is copied to multipath
                                 if(access(MULTI_PATH[x],X_OK)==0){ //It used to check if multiple path can be accessible and executable.
                                        strcpy(path,MULTI_PATH[x]); //multipath is copied to path.
                                        break;
                                }
                        }

                  }
                  if(execv(path,myargs)==-1){ //exec() is used to replace current process.
                                errorMsg(); //If it equals -1 then it will give error and terminate process.
                                exit(0);
                  }

        }
        else {
                int status=0;
        }
        return cpid; //Return child process


}

int processManager(char *buffer){
        int cpyStdOut=0;
        int cpid;
        if(strstr(buffer,">")!=NULL){ //Shell provides ">" to redirect.
                        char* redirectOfMulti[sizeof(char)*512];
                        int index=0;
                        redirectOfMulti[0]= strtok(strdup(buffer)," \n\t>"); // It is used to duplicate a string which is " \n\t>"
                        while(redirectOfMulti[index]!=NULL){ //If the given index is not equals zero
                                index=index+1; // Then increment it.
                                redirectOfMulti[index]=strtok(NULL," \n\t>"); // It used to split a string into " \n\t>".
                        }
                        if(index==1){ //If there is no output file, then it will give error message.
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0); //Process is terminated.
                        }
                        char* myargs[sizeof(buffer)];
                        int i=0;
                        myargs[0]= strtok(buffer,"\n\t>"); // It used to split a string into " \n\t>".
                        while(myargs[i]!=NULL){ // If the myargs that is in given i value is not equals zero.
                                i=i+1; //Then increment it.
                                myargs[i]=strtok(NULL," \n\t>"); // It used to split a string into " \n\t>".
                        }
                        if(i>2){ //If there is no output file, then it will give error message.
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }
                        char* tokens[sizeof(myargs[1])];
                        int x=0;
                        tokens[0]= strtok(myargs[1]," \n\t"); // It used to split a string into " \n\t>".
                        while(tokens[x]!=NULL){ // If the tokens that is in given x value is not equals zero.
                                x=x+1; //Then increment it.
                                tokens[x]=strtok(NULL," \n\t"); // It used to split a string into " \n\t>".
                        }
                        char *outF=strdup(tokens[0]);
                        cpyStdOut=dup(1);
                        int file_desc=open(outF,O_WRONLY|O_CREAT|O_TRUNC,0666); //Connection between a file and a file descriptor.
                        int error=open(outF,O_WRONLY|O_CREAT|O_TRUNC,0666);
                        fflush(stdout); //It writes any unwritten data in stream's buffer.
                        dup2(file_desc,STDOUT_FILENO);  //It duplicates an open file descriptor
                        dup2(file_desc,STDERR_FILENO);
                        close(file_desc);
                        closed=1;
                        if(error==-1 || x>1 || i>2 || file_desc==-1){ //These are gives error then error message is printed.
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                        }
                        tokens[x+1]=NULL;
                        myargs[i+1]=NULL;

                        strcpy(buffer,myargs[0]); //myargs is copied to buffer.
        }

        if(buffer[0] != '\n'&& buffer[0] != '\0') { // If the first character of buffer is not equals to "\n" and "\0"
                char *shellCommands[sizeof(buffer)];
                shellCommands[0] = strtok(buffer, " \t\n"); // It used to split a string into " \t\n".
                int tmp=0;
                while(shellCommands[tmp]!=NULL){ // If the shellcommands that is in given tmp value is not equals zero.
                        tmp=tmp+1; // Then increment it.
                        shellCommands[tmp]=strtok(NULL, " \n\t"); // Split a string into " \n\t".

                }
                shellCommands[tmp+1]=NULL; //The shellcommands that is in given tmp + 1 value is equaled to null.
		if(strcmp(shellCommands[0],"cd") == 0){ //"cd" command is used to change the current working directory
                        if(tmp==2){
                                if(chdir(shellCommands[1])!=0){ //"cd" is also known as chdir which is used to change directory
                                        write(STDERR_FILENO, error_message, strlen(error_message)); //If it fails then it will give error

                                }
                                }
                        else{ //If there are zero or more than 2 arguments, it will give error.
                                write(STDERR_FILENO, error_message, strlen(error_message));

                        }

                }

                else if(strcmp(shellCommands[0],"pwd") == 0){ //**Extra**
                        // It is used to write the full pathname of the current working directory to the standard output.
                                if(tmp==1){
                                        printf("\tWorkdir: %s\n",getenv("PWD"));
                                  //Getenv() It used to search the environment list to find the environment variable name,
                                        printf("\tFilepath: %s/%s\n",getenv("PWD"),__FILE__);
                                        }

                                 else{ //0 Arguments or more than 1 arguments?
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                 }

                        }

                else if(strcmp(shellCommands[0],"path") == 0){ //Path is used to access filenames and directory names.
                                changedPath=1;
                                if(tmp==2){
                                        emptyPath=0; //There is no any empty path
                                        path=strdup(shellCommands[1]); //It is used to duplicate a string
                                         if(path[strlen(path)-1]!='/'){ //If last character is not equals to "/".
                                                strcat(path,"/"); //It joins path and "/".
                                        }
                                }
                                else if(tmp==1){ //If tmp variable is equals 1, it means that there is empty path.
                                        emptyPath=1;
                                }
                                else{ //If there are more than one path
                                        emptyPath=0; //There is no empty path.
                                        int i=0;
                                        while(i<tmp){
                                                char *temp=strdup(shellCommands[i]); //It is used to duplicate a string
                                                if(temp[strlen(temp)-1]!='/'){ //If last character is not equals to "/".
                                                        strcat(temp,"/"); //It joins path and "/".
                                                }
                                                strcpy(MULTI_PATH[i-1],temp); //Multipath is copied to temp.
                                                numOfMultiPath++; // nunber of mutliple paths are increased
                                                i=i+1; //it increases i value until to reach tmp.
                                        }

                                        //printf("%d\n",numOfMultiPath);
                                        //for(int i=0;i<numOfMultiPath;i++)
                                                //printf("%s\n",MULTI_PATH[i]);
                                }

			}
                else if(strcmp(shellCommands[0],"exit") == 0) { //It is used to terminate process
                                if(tmp==1){ //If tmp value is equals to 1, then process is terminated.
                                        exit(0);
                                    }
                                else{ //If there are zero or more than two arguments, it will give error.
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                    }
                            }
                                else{
                                    if(emptyPath==1){ //If there is aa empty path, it will give error.
                                            write(STDERR_FILENO, error_message, strlen(error_message));
                                    }
                                    else{ //Otherwise it creates process with using fork().
                                            cpid=createProcess(shellCommands);
                                    }
                                }
                    }
                    if(closed==1){
                            dup2(cpyStdOut,1);
                            close(cpyStdOut);
                    }
                return cpid; //It returns child process.
    }

int main(int argc, char* argv[]){
        FILE *file = NULL;
        path=(char*) malloc(BUFFER_SIZE);
        char buffer[BUFFER_SIZE];
        welcome();

        if(argc==1){ //Interactive mode allows the user to type commands directly
                file=stdin; //Store standard input to the file.
                myPrompt(); //Call myPrompt method.
        }

        else if(argc==2){ //Batch mode reads input from a batch file and executes commands from therein.

                char *bFile= strdup(argv[1]);
                file = fopen(bFile, "r"); //Open text file to read.
                if (file == NULL) { //If there is no file, it will give error.
        	        errorMsg();
                }
                batch=1;
        }
        else{ //Otherwise it gives error message.
                errorMsg();
        }

        while(fgets(buffer, BUFFER_SIZE, file)){ //Writes from file to buffer
                closed=0;
                if(spaceController(buffer)==0){ //Checks if the buffer is only space.
                        continue;
                }
                if(strstr(buffer,"&")!=NULL){//Predefined function used for string handling also for concurrency
                        int q=0;
                        char *myargs[sizeof(buffer)];
                        myargs[0]= strtok(buffer,"\n\t&"); // It used to split a string into "\n\t&"
                        while(myargs[q]!=NULL){
                                q=q+1;
                                myargs[q]=strtok(NULL,"\n\t&"); //// It used to split a string into "\n\t&"

                        }
                        myargs[q+1]=NULL;
                        int myPid[q];
                    for(int i=0;i<q;i=i+1){
                        myPid[i]=processManager(myargs[i]);
                        for(int x=0;x<q;i=i+1){
                                int status=0;
                                waitpid(myPid[x],&status,0); //Waitpid() it used when there are more than one child for the process
                                //It is used to launch parallel commands.
                                if (status == 1){
                                    errorMsg();
                                }
                        }
                      }
                }
                else{
                    	processManager(buffer);
                }
                if(argc == 1) {
                        myPrompt();
                 }

        }
}
