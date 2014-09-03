/* Kristin Onorati
*  10874126
*  Group 1 Seq 3
*  CPTS 360 Lab 4
*/

#include "server.h"

int server_init(char *name)
{
   printf("==================== server init ======================\n");   
   // get DOT name and IP address of this host

   printf("1 : get and show server host info\n");
   hp = gethostbyname(name);
   if (hp == 0){
      printf("unknown host\n");
      exit(1);
   }
   //printf("Made it");
   fflush(stdout);
   printf("    hostname=%s  IP=%s\n",
               hp->h_name,  inet_ntoa(*(long *)hp->h_addr));
  
   //  create a TCP socket by socket() syscall
   printf("2 : create a socket\n");
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0){
      printf("socket call failed\n");
      exit(2);
   }

   printf("3 : fill server_addr with host IP and PORT# info\n");
   // initialize the server_addr structure
   server_addr.sin_family = AF_INET;                  // for TCP/IP
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address  
   server_addr.sin_port = 0;   // let kernel assign port

   printf("4 : bind socket to host info\n");
   // bind syscall: bind the socket to server_addr info
   r = bind(sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
   if (r < 0){
       printf("bind failed\n");
       exit(3);
   }

   printf("5 : find out Kernel assigned PORT# and show it\n");
   // find out socket port number (assigned by kernel)
   length = sizeof(name_addr);
   r = getsockname(sock, (struct sockaddr *)&name_addr, &length);
   if (r < 0){
      printf("get socketname error\n");
      exit(4);
   }

   // show port number
   serverPort = ntohs(name_addr.sin_port);   // convert to host ushort
   printf("    Port=%d\n", serverPort);

   // listen at port with a max. queue of 5 (waiting clients) 
   printf("5 : server is listening ....\n");
   listen(sock, 5);
   printf("===================== init done =======================\n");
}

 // Create array of strings from input string
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

 //Run command given from client
 void runProg(char** input, int sock)
 {   
    int i = 0, found = 0;
    int pid, status;
    char line[MAX];
    char tmpLine[MAX];
    char cwd[MAX];
    const char *myCommands[] = {"pwd", "ls", "cd", "mkdir", "rmdir", "rm", "get", "put", "quit"};
    memset(line, 0, MAX); 

    // Find command
    while(i < NUM_CMNDS && !found)
    {
        if(strcmp(input[0], myCommands[i]) == 0)
        {
            found = 1;
        }
        else
        {
            i++;
        }
    }
    if(found) // Valid command was found
    {
        if(!input[1]) //If no filename/pathname given replace null pointer with cwd
        {
            getcwd(tmpLine, MAX);
            strcpy(cwd, tmpLine);
            input[1] = cwd;
        }
        // fork a child to handle the syscall
        pid = fork();
        if(pid < 0)
        {
            printf("ERROR: Could not fork child\n");
        }
        else if(pid == 0) //This is the child
        {
            switch(i)
            {
                case 0: //pwd
                    getcwd(tmpLine, MAX);
                    sprintf(line, "CWD = %s", tmpLine);
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                    break;
                case 1: //ls
                    getInfo(input[1], sock);
                    break;
                case 3: //mkdir
                    if(!mkdir(input[1], 0777))
                        sprintf(line, "New directory made. New Directory = %s", input[1]);
                    else
                        sprintf(line, "Error creating new directory: %s", strerror(errno));
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                    break;
                case 4: //rmdir
                    if(!rmdir(input[1]))
                        sprintf(line, "Directory removed. Old Directory = %s", input[1]);
                    else
                        sprintf(line, "Error removing directory: %s", strerror(errno));
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                    break;
                case 5: //rm
                    if(!unlink(input[1]))
                        sprintf(line, "File Removed: Old File Name = %s\n", input[1]);
                    else
                        sprintf(line, "Error removing file: %s\n", strerror(errno));
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                    break;
                case 6: //get
                    getFile(input[1], sock);
                    break;
                case 7: //put
                    putFile(input[1], sock);
                    break;
                case 8: //quit
                    sprintf(line, "Thank you and goodbye");
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
                    break;
            }
            exit(0);
        }
        else
        {
            if(i == 2) //Change directory
            {
                    chdir(input[1]);
                    getcwd(tmpLine, MAX);
                    sprintf(line, "New CWD = %s", tmpLine);
                    n = write(newsock, line, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
            }
            wait(&status);
            memset(line, 0, MAX);
            strcpy(line, "FIN");
            n = write(newsock, line, MAX);
            printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
        }
    }
    else // Push error message to client
    {
        strcpy(line, "Invalid Command");
        n = write(newsock, line, MAX);
        printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
        memset(line, 0, MAX);
        strcpy(line, "FIN");
        n = write(newsock, line, MAX);
        printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
    }
 }

// Get file from server and write to socket
int getFile(char* filename, int sock)
{
    struct stat ss;
    char buf[MAX];
    FILE *fr;
    int total = 0, m = 0;
    if(-1 == stat(filename, &ss) || !S_ISREG(ss.st_mode)) //If invalid file return "BAD"
    {
        sprintf(buf, "BAD");
        n = write(sock, buf, MAX);
        printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buf);
        return -1;
    }
    memset(buf, 0, MAX);
    sprintf(buf, "%lu", ss.st_size); // put SIZE in buffer to send to client
    //printf("SIZE = %lu\n", (unsigned long)ss.st_size); //FOR TESTING
    n = write(sock, buf, MAX);
    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buf);
    fr = fopen(filename, "r");
    while(n = fread(buf, 1, MAX, fr)) //While there is more file...read from file and write to socket
    {
        printf("server: wrote n=%d bytes; LINE=[%s]\n", n, buf);
        total += n;
        write(sock, buf, n);
        memset(buf, 0, MAX);
    }
    memset(buf, 0, MAX);
    if(total < ss.st_size) //Write the tail of the file (if tail size < MAX)
    {
        n = fread(buf, 1, (unsigned int)(ss.st_size - total), fr);
        printf("server: wrote n=%d bytes; LINE=[%s]\n", n, buf);
        total += n;
        write(sock, buf, n);
    }
    fclose(fr);
    return 0;
}

// Get file from client (through socket) and save to servers cwd
int putFile(char *filename, int sock)
{
    char buf[MAX];
    int count, m;
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
    sscanf(buf, "%lu", &size); // Set size variable to returned size
    //printf("SIZE = %lu", size); //FOR TESTING
    while(count < size) // While haven't read SIZE bytes keep reading and write to file
    {
        memset(buf, 0, MAX);
        m = read(sock, buf, MAX);
        printf("server: read  n=%d bytes; ECHO=[%s]\n",m , buf);
        count += m;
        fwrite(buf, 1, m, fw);
    }
    fclose(fw); 
    return m;
}


main(int argc, char *argv[])
{
   char *hostname;
   char line[MAX];
   char **input;
   int i;

   if (argc < 2)
      hostname = "localhost";
   else
      hostname = argv[1];
 
   server_init(hostname); 

   // Try to accept a client request
   while(1){
     printf("server: accepting new connection ....\n"); 

     // Try to accept a client connection as descriptor newsock
     length = sizeof(client_addr);
     newsock = accept(sock, (struct sockaddr *)&client_addr, &length);
     if (newsock < 0){
        printf("server: accept error\n");
        exit(1);
     }
     printf("server: accepted a client connection from\n");
     printf("-----------------------------------------------\n");
     printf("        IP=%s  port=%d\n", inet_ntoa(client_addr.sin_addr.s_addr),
                                        ntohs(client_addr.sin_port));
     printf("-----------------------------------------------\n");

     // Processing loop
     while(1){
       n = 0;
       n = read(newsock, line, MAX);
       if (n==0){
           printf("server: client died, server loops\n");
           close(newsock);
           break;
        }      
        // show the line string
        printf("server: read  n=%d bytes; line=[%s]\n", n, line);

        // Parse string into argv array
        input = parseInput(line);
        n = 0;
        runProg(input, newsock);

        //printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
        printf("server: ready for next request\n");
     }
  }
}


