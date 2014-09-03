/* Kristin Onorati
*  10874126
*  Group 1 Seq 3
*  CPTS 360 Lab 4
*/

//**************************** ECHO CLIENT CODE **************************
// The echo client client.c

#include "client.h"

// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n"); 
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n", 
          hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

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

 // Get file from server and save to client cwd
 int getFile(char* filename, int sock)
 {
    char buf[MAX];
    int count, n;
    unsigned long size;
    FILE *fw;
    memset(buf, 0, MAX);
    
    // Read from socket. If "BAD" exit, else set size to returned size
    read(sock, buf, MAX);
    if(strcmp(buf, "BAD") == 0)
    {
        printf("Invalid filename\n");
        return -1;
    }
    count = 0;
    fw = fopen(filename, "w");
    sscanf(buf, "%lu", &size);
    //printf("SIZE = %lu", size); //FOR TESTING
    while(count < size)
    {
        memset(buf, 0, MAX);
        n = read(sock, buf, MAX);
        printf("client: read  n=%d bytes; echo=(%s)\n",n , buf);
        count += n;
        fwrite(buf, 1, n, fw);
    }
    fclose(fw); 
    return n;
 }

 // Send file to server to save on server's cwd
 int putFile(char* filename, int sock)
 {
    struct stat ss;
    char buf[MAX];
    FILE *fr;
    int total = 0, n = 0;
    if(-1 == stat(filename, &ss) || !S_ISREG(ss.st_mode))
    {
        sprintf(buf, "BAD");
        n = write(sock, buf, MAX);
        return -1;
    }
    memset(buf, 0, MAX);
    sprintf(buf, "%lu", ss.st_size);
    printf("SIZE = %lu\n", (unsigned long)ss.st_size);
    n = write(sock, buf, MAX);
    fr = fopen(filename, "r");
    while(n = fread(buf, 1, MAX, fr))
    {
        printf("client: wrote n=%d bytes; line=[%s]\n", n, buf);
        total += n;
        write(sock, buf, n);
        memset(buf, 0, MAX);
    }
    memset(buf, 0, MAX);
    if(total < ss.st_size)
    {
        n = fread(buf, 1, (unsigned int)(ss.st_size - total), fr);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, buf);
        total += n;
        write(sock, buf, n);
    }
    fclose(fr);
    return n;
 }

 int cat(char* filename)
 {
    FILE* fr;
    fr = fopen(filename, "r");
    char buf[1024];
    if(!fr)
    {
        printf("Invalid filename\n");
        return -1;
    }
    while(fgets(buf, 1024, fr) != NULL)
    {
        printf("%s", buf);
    }
    fclose(fr);
    return 0;
 }

 // Run local program
 int runProg(char** input, int cmnd)
 {
    int pid, status, i = 0;
    char input1[10];
    char cwd[256];
    char *pwd;
    char *home;
    home = getenv("HOME");
    getcwd(cwd, MAX);
    i = 1;
    while(input[0][i])
    {
        input1[i-1] = input[0][i];
        //printf("input1[%d] = %c, input[0][%d] = %c\n", i-1, input1[i], i, input[0][i]); //FOR TESTING
        i++;
    }
    input1[i] = 0;
    strcpy(input[0], input1);
    if(!input[1])
    {
        if(strcmp(input[0], "ls") == 0) //Put cwd in input[1]
        {
            pwd = (char*)malloc(sizeof(char)*strlen(cwd));
            strcpy(pwd, cwd);
            input[1] = pwd;
        }
        else if(strcmp(input[0], "cd") == 0) //Put home in input[1]
        {
            input[1] = home;  
        }
    }
    pid = fork();
    if(pid == 0) //Child process
    {
        if(!strcmp(input[0], "cat")) //If cat, pwd, or ls run function with execvp
        {
            cat(input[1]);
        }
        else if(!strcmp(input[0], "pwd")) //Print current working directory
        {
            printf("CWD = %s\n", cwd);
        }
        else if(!strcmp(input[0], "ls")) //run ls function
        {
            getInfo(input[1], 0);
        }
        else if(!strcmp(input[0], "mkdir")) //Make new directory with syscall
        {
            if(mkdir(input[1], 0777) == -1)
            {
                printf("Error creating new directory: %s\n", strerror(errno));
            }
        }
        else if(!strcmp(input[0], "rmdir")) //Remove directory with syscall
        {
            if(rmdir(input[1]) == -1)
            {
                printf("Error removing directory: %s\n", strerror(errno));
            }
        }
        else if(!strcmp(input[0], "rm")) //Remove file with syscall
        {
            if(unlink(input[1]) == -1)
            {
                printf("Error removing file: %s\n", strerror(errno));
            }
        }
        exit(0);
    }
    else //Parent process
    {
        if(!strcmp(input[0], "cd")) // Change cwd with syscall
        {
            chdir(input[1]);
        }
                wait(&status);
    }
    return 0;
 }

main(int argc, char *argv[ ])
{
  int n, i, found;
  char line[MAX], ans[MAX];
  const char* lCommands[] = {"lcat", "lpwd", "lls", "lcd", "lmkdir", "lrmdir", "lrm"};
  char **inputArr;
  char temp[256];

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);

  printf("********  processing loop  *********\n");
  while (1){
    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
       exit(0);
    found = 0;
    strcpy(temp, line);
    inputArr = parseInput(temp);
    i = 0;
    n = 0;
    while(i < 7 && !found) // Check if local command
    {
        if(strcmp(inputArr[0], lCommands[i]) == 0)
        {
            // Run local program
            runProg(inputArr, i);
            found = 1;
        }
        else
            i++;
    }
    if(!found)
    {
        // Send ENTIRE line to server
        n = write(sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        if(strcmp(inputArr[0], "get") == 0) // get file command
        {
            getFile(inputArr[1], sock);
        }
        else if(strcmp(inputArr[0], "put") == 0) // put file command
        {
            putFile(inputArr[1], sock);
        }
        // Read a line from sock and show it
        while((n = read(sock, ans, MAX)) && strcmp(ans, "FIN") != 0)
        {
            printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
        }
        
    }
    i = 0;
    while(inputArr[i]) //Free strings in input Array
    {
        free(inputArr[i]);
        i++;
    }
    if(strcmp(line, "quit") == 0)
    {
        printf("client: Now disconnecting from server\n");
        exit(0);
    }
  }
}


