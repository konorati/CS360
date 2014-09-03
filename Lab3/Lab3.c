/* Kristin Onorati
 * 10874126
 * CPTS 360 Lab 3
 * Group 1 Seq 3
 */

 #include "Lab3.h"

 // Get input from user on command line
 char* getInput()
 {
    char buf[1024];
    char *temp = NULL;
    int len = -1;

    printf("User@mysh: ");
    fgets(buf, 1024, stdin);
    len = strlen(buf);
    temp = (char*)malloc(sizeof(char)*len);
    strcpy(temp, buf);
    if(temp[len-1] == '\n')
    {
        temp[len-1] = 0;
    }
    return temp;
 }

 // Parse user input string into argv[] array of char*'s
 char** parseInput(char* input)
 {
    int count = 0;
    char *str = NULL;
    char *tmpStr = NULL;
    // Allocate char* inputArr[20]
    char** inputArr = (char**)malloc(sizeof(char*)*20);
    str = strtok(input, " ");
    while(str)
    {
        tmpStr = (char *)malloc(sizeof(char)*strlen(str));
        strcpy(tmpStr, str);
        inputArr[count] = tmpStr;
        count++;
        str = strtok(NULL, " ");
    }
    inputArr[count] = NULL;
    return inputArr;
 }

 // Determine which command to run and run it
 void handleCommand(char** input, char* const envp[])
 {
    char *temp;
    int i = 0;
    // If command = cd run cd command
    if(strcmp(input[0], "cd") == 0)
    {
        cd(input[1]);
    }
    else if(strcmp(input[0], "exit") == 0)
    {
        exitProg();
    }
    else
    {
        forkChild(input, envp);
    }

    // After handling command free the memory in the input array
    while(input[i])
    {
        free(input[i++]);
    }
    free(input);
 }

 // Change directory command
 void cd(char* pathname)
 {
    if(pathname)
    {
        chdir(pathname);
    }
    else
    {
        chdir(homePath);
    }
 }

 // Exit command
 void exitProg()
 {
    exit(1);
 }

 // All other commands
 void otherCommand(char** input, char* const envp[])
 {
    char original[128];
    char new[1024];
    int i = 0;
    strcpy(original, input[0]);

   //Check each bin folder for command by concatenating pathname with input[0]
    while(commandPath[i++]) 
    {
        strcpy(new, commandPath[i]);
        strcat(new, "/");
        strcat(new, original);
        execve(new, input, envp);
    }
 }

// Get and set array of possible command paths
 void findPath()
 {
    char *token;
    int i = 0;
    char *name = "PATH";
    char *temp = getenv(name);
    token = strtok(temp, ":");
    while(token != NULL)
    {
        commandPath[i++] = token;
        token = strtok(NULL,":");
    }
    commandPath[i] = NULL;
    i = 0;
}

// Get and set home path
 void findHome()
 {
    int j = 0,i = 0;
    const char* name = "HOME";
    char *temp = getenv(name);
    homePath = (char*)malloc(sizeof(char)*strlen(temp));
    while(temp[i])
    {
       homePath[j++] = temp[i++];
     }
     homePath[j] = 0;
 }

// Fork a new child and check if pipe
 int forkChild(char** input, char* const envp[])
 {
    int pid;
    int status = 0;
    int isPipe = 0;
    int i = 0;
    char** head = NULL;
    char** tail = NULL;

    head = (char**)malloc(sizeof(char*)*20);
    tail = (char**)malloc(sizeof(char*)*20);
    pid = fork();

    if(pid < 0)
    {
        printf ("Could not fork child\n");
        exit(1);
    }

    if(pid)
    {   
        pid = wait(&status);
    }
    else
    {
        
        // Check for pipe
        isPipe = hasPipe(input, head, tail);
        if(isPipe)
        {
            doPipe(head, tail, envp);
        }
        else
        {
            runProg(input, envp);
        }
    }
    free(head);
    free(tail);
    return status;
 }

// Determine if there is an I/O redirect. Set redirect 1 for input, 2 for output, 
//   3 for append output 0 if no redirect. Return filename to redirect to/from
 char* isRedirect(char** input, int* red)
 {
    int i = 1;
    char* temp = NULL;
    char* temp2 = NULL;
    while(!(*red) && input[i])
    {
        if(!(strcmp(input[i], "<"))) // Redirect input
        {
            *red = 1;
        }
        else if(!(strcmp(input[i], ">"))) //Redirect output
        {
            *red = 2;
        }
        else if(!(strcmp(input[i], ">>"))) //Redirect output & append
        {
            *red = 3;
        }
        if(*red)
        {
            // set redirect indicator in input array to NULL
            temp = input[i+1];          
            temp2 = input[i];
            input[i] = NULL;
            input[i+1] = NULL;
            free(temp2);
        }
        i++;
    }
    return temp;
 }

// redirect to different input or output
 void redirect(char* file, int red)
 {
    switch(red)
    {
        case 1: close(0);
                open(file, O_RDONLY);
                break;
        case 2: close(1);
                open(file, O_WRONLY|O_CREAT, 0644);
                break;
        case 3: close(1);
                open(file, O_APPEND|O_WRONLY|O_CREAT, 0644);
                break;
        default: printf("Error, bad redirect\n");
                break;
    }
 }

// Determine if input has a pipe (if so return 1 else return 0)
// If pipe found set head to beginning portion of input (before pipe)
// and tail to end of input (after pipe)
int hasPipe(char** input, char** head, char** tail)
{
    int i = 0, j = 0, found = 0;
    char *temp;
    while(!found && input[i])
    {
        if(!strcmp(input[i], "|")) // Pipe found
        {
            found = 1;
            temp = input[i];
            input[i++] = NULL;
            free(temp);
            while(input[i]) // Set tail
            {
                tail[j++] = input[i++];
            }
            tail[j] = NULL;
            input[i] = NULL;
            j = 0;
            i = 0;
            while(input[i]) // Set head
            {
                head[j++] = input[i++];
            }
            head[j] = NULL;
            free(input);
        }
        i++;
    }
    return found;
}

// fork a new child and redirect input/output through pipe
void doPipe(char** head, char** tail, char* const envp[])
{
   int pd[2];
   int pid;
   
   pipe(pd); // Create pipe

    // Create new child to share pipe

   if(fork() == 0) //Child process 
   {
        close(pd[0]);
        dup2(pd[1], 1);
        close(pd[1]);
        runProg(head, envp);
    }
    else // Parent process
    {
        close(pd[1]);
        dup2(pd[0], 0);
        close(pd[0]);
        runProg(tail, envp);
    }
        

}

// Check for redirect and run output
void runProg(char** input, char* const envp[])
{
    int red = 0;
    char* file = NULL;

    // Check for file redirection
    
    file = isRedirect(input, &red);
    if(red)
    {
        redirect(file, red);
    }
    otherCommand(input, envp);
}
