#include "mystat.h"

// int  stat(const char *filename, struct stat *buf);
// int lstat(const char *filename, struct stat *buf);
int test_mode(struct stat *input, enum stat_mode mode)
{
   if (((input->st_mode >> 12) & mode) == mode)
   {
      return 1;
   }
   return 0;
}

int test_perm(struct stat *input, enum perm_mode mode)
{
   if (((input->st_mode) & mode) != 0)
   {
      return 1;
   }
   return 0;
}

int getInfo(char* pathname, int sock)
{
   struct stat file_info;
   char buf[MAX];
   int n;
   memset(buf, 0, MAX);
   if(lstat(pathname, &file_info) == -1)
   {
        printf("Invalid Pathname\n");
        return -1;
   }
   else 
   {
        memset(buf, 0, MAX);
        if(S_ISDIR(file_info.st_mode))
        {
            printDir(pathname, sock);
        }
        else // Not a directory
        {
                sprintf(buf, "%s %s", printInfo(&file_info), printFileName(pathname, &file_info));
                if(sock)
                {
                    n = write(sock, buf, MAX);
                    printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buf);
                }
                else //Print to console
                {
                    printf("%s\n", buf);   
                }
        }
        return 0;
    }
}

// returns file/dir name or link name and linked file name
char* printFileName(char* filename, struct stat *file_info)
{
    char buft[MAX];
    char *buf;
    char temp[MAX];
    int i = 0;
    if(S_ISLNK(file_info->st_mode))    
    {
        i = readlink(filename, temp, MAX);
        temp[i] = 0;
        sprintf(buft, " %s -> %s ", filename, temp);
    }
    else
    {
        sprintf(buft, " %s ", filename);
    }
    buf = (char*)malloc(sizeof(char)*strlen(buft));
    strcpy(buf, buft);
    return buf;
}

// Print contents of directory
int printDir(char* pathname, int sock)
{
    DIR* fd;
    struct dirent* dp;
    struct stat st;
    char buf[MAX];
    char *temp1, *temp2;
    int n;

    if(!(fd = opendir(pathname)))
    {
        printf("Invalid Directory\n");
        return -1;
    }
    while(dp = readdir(fd))
    {
        memset(buf, 0, MAX);
        lstat(dp->d_name, &st);
        sprintf(buf, "%s %s",  temp1 = printInfo(&st), temp2 = printFileName(dp->d_name, &st));
        if(sock) //Print to socket
        {
            n = write(sock, buf, MAX);
            printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, buf);
        }
        else
        {
            printf("%s\n", buf);
        }
        free(temp1);
        free(temp2);
    }
}

char* printInfo(struct stat *file_info)
{
    char buf[MAX];
    char *bufRet;
    char temp[MAX];
    int i = 0;
    memset(buf, 0, MAX);
    if (S_ISLNK(file_info->st_mode))
   {
      sprintf(buf, " %s ", "LNK");
   }
   else if (S_ISREG(file_info->st_mode))
   {
      sprintf(buf, " %s ", "REG");
   }
   else if(S_ISDIR(file_info->st_mode))
   {
      sprintf(buf, " %s ", "DIR");
   }
   else
   {
        sprintf(buf, " %s ", "N/A");
   }
   if (test_perm(file_info, USER_READ) != 0)
      {strcat(buf, "r");} 
   else {strcat(buf, "-");}

   if (test_perm(file_info, USER_WRITE) != 0)
      {strcat(buf, "w");}
   else {strcat(buf, "-");}

   if (test_perm(file_info, USER_EXEC) != 0)
      {strcat(buf, "x");}
   else {strcat(buf, "-");}

   strcat(buf, " ");

   if (test_perm(file_info, GROUP_READ) != 0)
      {strcat(buf, "r");}
   else {strcat(buf, "-");}

   if (test_perm(file_info, GROUP_WRITE) != 0)
      {strcat(buf, "w");}
   else {strcat(buf, "-");}

   if (test_perm(file_info, GROUP_EXEC) != 0)
      {strcat(buf, "x");}
   else {strcat(buf, "-");}

   strcat(buf, " ");

   if (test_perm(file_info, OTHER_READ) != 0)
      {strcat(buf, "r");}
   else {strcat(buf, "-");}

   if (test_perm(file_info, OTHER_WRITE) != 0)
      {strcat(buf, "w");}
   else {strcat(buf, "-");}

   if (test_perm(file_info, OTHER_EXEC) != 0)
      {strcat(buf, "x");}
   else {strcat(buf, "-");}

   strcat(buf, " ");
    
    // Print UID
    sprintf(temp, " %hu ", (unsigned short)file_info->st_uid);
    strcat(buf, temp);

    // Print size
    sprintf(temp, " %lu ", (unsigned long)file_info->st_size);
    strcat(buf, temp);

    // Print ctime in calendar format
    sprintf(temp, " %s ", ctime(&(file_info->st_ctime)));
    i = 0;
    while(i < MAX)
    {
        if(temp[i] == '\n')
        {
            temp[i] = 0;
        }
        i++;
    }
    strcat(buf, temp);  
    bufRet = (char*)malloc(sizeof(char)*strlen(buf));
    strcpy(bufRet, buf);
    return bufRet;
}
