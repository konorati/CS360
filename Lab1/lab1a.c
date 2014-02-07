// Lab1 functions
#include "lab1a.h"

char* command_array[] = {"menu", "mkdir", "rmdir", "cd", "ls", "pwd", "creat", "rm", "save", "reload", "quit"};

/* Initialize the '/' root directory of the tree */
void initRoot()
{
   root = malloc(sizeof(Node));
   strcpy(root->name, "/");
   root->type = 'D';
   root->childPtr = NULL;
   root->siblingPtr = NULL;
   root->parentPtr = NULL;
   cwd = root;
}

// Prints menu of possible commands
void printMenu()
{
   printf("********************* MENU *********************\n");
   printf("mkdir rmdir cd ls pwd creat rm save reload quit\n");
   printf("************************************************\n");
}  

// Gets input string from console
void getInput()
{
   int i = 0, j = 0;
   char* str;
   strcpy(command, "");
   strcpy(pathname, "");
   fgets(line, 128, stdin);
   line[strlen(line)] = 0;
   if(!line[i]) // Error, no input
   {
      return;
   }
   /*str = strtok(line, " ");
   strcpy(command, str);
   str = strtok(NULL, " ");
   strncpy(pathname, str, 64);*/
    while(line[i] && line[i] != ' ' && isalpha(line[i])) // Everthing before space is a command
   {
      command[i] = line[i];
      i++;
   }  
   command[i] = 0; // Null terminator at end of command
   if(line[i] == ' ') // If there is a pathname continue
   {
      i++;
   }
   while(line[i])
   {
      pathname[j++] = line[i++];
   }
   pathname[j] = 0;
   if(pathname[j-1] == '\n')
   {
      pathname[j-1] = 0;
   }
}

// Finds command in list of possible commands, returns -1 if bad command
int findCommand()
{
   int code = 10, found = 0;
   while(code >= 0 && !found)
   {
      if(strcmp(command_array[code], command) == 0)
      {
         found = 1;
      }
      else
      {
         code--;
      }
   }
   return code;
}

// Function makes a directory if possible (if pathname is bad return 0 else     returns
int mkdir()
{
   Node *current = NULL;
   Node *pTemp = NULL;
   int valid = 0;
   current = findNode(); 
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   valid = nameAlreadyExistsError(current);
   if(!valid) {return 0;}
   if(strcmp(basename, "") == 0)
   {
      printf("Invalid directory name\n");
      return 0;
   }
   pTemp = makeNode(current, 'D');
   if(!pTemp) {return 0;}
  // printf("current->name = %s", current->name);  // For testing
   //printf("current->childPtr = %s", current->childPtr->name); // for testing
   if(current->childPtr) //Directory already has children so new directory should be added to end of childs sibling list
   {
      current = current->childPtr;
      while(current->siblingPtr)
      {
            current = current->siblingPtr;
      }
      current->siblingPtr = pTemp;
     // printf("current->child->sibling = %s", current->siblingPtr->name);
   }
   else
   {
      current->childPtr = pTemp;
   }
  return 1;
} 
// Function removes a directory if directory exists  
int rmdir()
{
   int valid = 0;
   Node* current = NULL;
   Node* pPrev = NULL;
   Node* pParent = NULL;
   pParent = findNode();
   valid = invalidPathError(pParent);
   if(!valid) {return 0;};
   // Find dir from parent node
   current = pParent->childPtr;
   while(current != NULL && strcmp(current->name, basename) != 0)
   {
      pPrev = current;
      current = current->siblingPtr;
   }
   if(!current)
   {
      printf("Error: Directory does not exist\n");
      return 0;
   }
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   if(pParent->childPtr == current)
   {
      pParent->childPtr = current->siblingPtr;
      printf("Parent = %s, child = %s, sibling = %s\n", pParent->name, current->name, current->siblingPtr->name); //For testing
   }
   else
   {
      pPrev->siblingPtr = current->siblingPtr;
   }

   //Directory has been found so delete it and everything within it
   deleteDir(current);
   return 1;
}
// Function changes the current directory to given directory or to root if p    athname is null
int cd()
{
   Node *current = NULL;
   int valid = 0;
   if(pathname[0] == 0)
   {
      cwd = root;
      return 1;
   }
   current = findNode();
  // printf("Current = %s\n", current->name); // For testing
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   current = current->childPtr;
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   while(current != NULL && strcmp(current->name, basename) != 0)
   {
        current = current->siblingPtr;
   }
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   cwd = current;
   //printf("CWD = %s\n", cwd->name); // For Testing
   return 1;
}
// List directory contents of pathname, use cwd if no pathname given
// return 0 if invalid pathname or non-directory given
int ls()
{
   Node *current = NULL;
   int valid = 0;
   current = findNode();
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   current = current->childPtr;
   //printf("CWD = %s", cwd->name); // For Testing
   while(current)
   {
      printf("%c\t%s\n", current->type, current->name);
      current = current->siblingPtr;
   }
   return 1;
}
// Print absolute pathname of cwd
void pwd()
{
   Node* current = cwd;
   char pathFor[256];
   findAbsolutePath(current, pathFor);
   printf("CWD = %s\n", pathFor);
}
// Create a new file node at end of given pathname (or cwd if no pathname)
// Return 0 if invalid pathname or no filename given
int creat()
{
   int valid = 0;
   Node *current = NULL;
   Node *pTemp = NULL;
   current = findNode();
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'D');
   if(!valid) {return 0;}
   valid = nameAlreadyExistsError(current);
   if(!valid) {return 0;}

   pTemp = makeNode(current, 'F');
   if(current->childPtr)
   {
      current = current->childPtr;
      while(current->siblingPtr)
      {
         current = current->siblingPtr;
      }
      current->siblingPtr = pTemp;
   }
   else
   {
      current->childPtr = pTemp;
   }
   return 1;
}
// Delete file from tree 
// Return 0 if file does not exist
int rm()
{
   int valid = 0;
   Node *current = NULL;
   Node *pPrev = NULL;
   Node *pParent = NULL;
   current = findNode();
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   pParent = current;
   current = current->childPtr;
   while(current && strcmp(current->name, basename) != 0)
   {
      pPrev = current;
      current = current->siblingPtr;
   }
   valid = invalidPathError(current);
   if(!valid) {return 0;}
   valid = invalidFileTypeError(current, 'F');
   if(!valid) {return 0;}
   if(pParent->childPtr == current)
   {
      pParent->childPtr = current->siblingPtr;
   }
   else
   {
      pPrev->siblingPtr = current->siblingPtr;
   }
   free(current);
   return 1;
}
// Save current tree to given file
int save()
{
   FILE *outFile = NULL;
   Node* current = root;
   outFile = fopen(pathname, "w");
   if(outFile)
   {
       printFile(outFile, current);
       fclose(outFile);
       return 1;
   }
   else
   {
      printf("Could not open file\n");
      return 0;
   }
}

// Load tree from given file
int reload()
{
   FILE *inFile = NULL;
   char type = 0;
   char buffer[128];
   //char *str;
   int i = 0, j = 0;
   inFile = fopen(pathname, "r");
   if(inFile)
   {
      if(fgets(buffer, 128, inFile) == NULL)
      {
         printf("No data to input\n");
         return 0;
      }
      initRoot();
      // printf("Root = %s", root->name); // For testing
      // Read file line by line until eof
      while(fgets(buffer, 128, inFile) != NULL)
      { 
            type = buffer[0];
            buffer[0] = ' ';
            //str = strtok(buffer, " ");
            i = 0, j = 0;
            while(buffer[i] != '/')                                    
            {
               i++;
            }
            // printf("%c\n", buffer[i]); // for testing
            while(buffer[i])
            {
               pathname[j++] = buffer[i++];
            }
            pathname[j] = 0;
            if(pathname[j-1] == '\n')
            {
               pathname[j-1] = 0;
               // printf("newline deleted\n");
            }
            //printf("pathname = %s\n", pathname); // For testing
            //strcpy(pathname, str);
            breakPathname();
            //printf("dirname = %s, basename = %s\n", dirname, basename); // For testing
            if(type == 'D')
            {
               mkdir();
            }
            else if (type == 'F')
            {
               creat();
            }
         
      }
   }
   else
   {
      printf("Could not open file\n");
      return 0;
   }
   return 1;
}
// Exit program
int quit()
{
   return 0;
}

// Break up pathname into dirname and basename
void breakPathname()
{
   int i = 0, j = 0;
   strcpy(dirname, "");
   strcpy(basename, "");
   j = strlen(pathname)-1;
   if(j < 0) // No pathname
   {
      return;
   }
   while(pathname[j] != '/' && j > 0)
   {
      j--;
   }
   if(j > 0 || (j == 0 && pathname[j] == '/'))
   { 
       for(i = 0; i <= j; i++)
       {
          dirname[i] = pathname[i];
       }
       dirname[++j] = 0;
       i = 0;
       while(pathname[j])
       {
        basename[i++] = pathname[j++];
       }
       basename[i] = 0;
   }
   else
   {
      strcpy(basename, pathname);
   }
}

// Find node from given dirname
Node* findNode()
{
   Node* pCurrent = cwd;
   int i = 0, j = 0, found = 0;
   char* str;
   if(dirname[i] == '/') //Absolute path so start at root not cwd
   {
      pCurrent = root; 
   }
   str = strtok(dirname, "/");
   //printf("str = %s\n", str); // For testing
   while(str)
   { 
      j = 0;
      found = 0;
      
      // Find node & check if directory
      pCurrent = pCurrent->childPtr;
     // printf("pCurrent->childPtr = %s\n", pCurrent->name); // For testing
      if(!pCurrent) // End of tree 
      {
         return NULL;
      } 
      do{
         if(strcmp(pCurrent->name, str) == 0)
         {
            found = 1;
           // printf("yay we found it\n");
         }
         else
         {
            pCurrent = pCurrent->siblingPtr;
           // printf("pCurrent->siblingPtr = %s\n", pCurrent->name); // For testing
         }
      }while(!found && pCurrent != NULL); // If not found & there are more nodes in list
      if(!found)
      {
         return NULL;
      }   
      str = NULL;
      str = strtok(NULL, "/");
      //printf("str = %s\n", str); // For testing
   }
   return pCurrent;
}

// Error message when invalid path is given
int invalidPathError(Node* current)
{
   if(!current)
   {
      printf("Invalid Path\n");
      return 0;
   }
   return 1;
}
//Error message when invalid file type is given
int invalidFileTypeError(Node* current, char type)
{
   int i = 1;
   if(current->type != type)
   {
      printf("Invalid Path Type\n");
      i = 0;
   }
   return i;
}

// Makes a new node
Node* makeNode(Node* parent, char type)
{
   Node* pTemp = malloc(sizeof(Node));
   if(pTemp == NULL) {return NULL;}
   strcpy(pTemp->name, basename);
   pTemp->type = type;
   pTemp->parentPtr = parent;
   pTemp->siblingPtr = NULL;
   pTemp->childPtr = NULL;
   return pTemp;
}

//Error message when file or dir already exists
int nameAlreadyExistsError(Node* current)
{
   current = current->childPtr;
   while(current)
   {
      if(strcmp(current->name, basename) == 0)
      {
         printf("Invalid: Name already exists\n");
         return 0;
      }
      current = current->siblingPtr;
   }
   return 1;
}

// Deletes the given directory and all of its contents
void deleteDir(Node* current)
{
   
   Node *pTemp = NULL;
   if(current->childPtr)
   {
      pTemp = current->childPtr->siblingPtr;
      while(pTemp)
      {
         current->childPtr->siblingPtr = pTemp->siblingPtr;
         deleteDir(pTemp);
         pTemp = current->childPtr->siblingPtr;
      }
      deleteDir(current->childPtr);
   }
   free(current);
}

void printFile(FILE* infile, Node* current)
{
   if(current)
   {
       char path[256];
       findAbsolutePath(current, path); 
       fprintf(infile, "%c %s\n", current->type, path);
   
       printFile(infile, current->childPtr);
       printFile(infile, current->siblingPtr);
   }
}
void findAbsolutePath(Node* current, char* pathFor)
{
     char pathBack[256];
     int i = 0, j = 0, count = 0, k = 0;

     strcpy(pathBack, "");
     while(current != root)
     {
          strcat(pathBack, current->name);
          strcat(pathBack, "/");
          current = current->parentPtr;
     }

   // printf("pathback = %s", pathBack); // For testing
     j = strlen(pathBack)-2;
     pathFor[i++] = '/';
     while(j >=0 )
     {
          count = 0;
          while(j >= 0 && pathBack[j] != '/')
          {
               j--;
               count++;
          }   
          for(k = j+1; k < j + count+1; k++)
          {
               pathFor[i++] = pathBack[k];
          }
      pathFor[i++] = '/';
      j--;
    }
    pathFor[i] = 0; 
    if(strcmp(pathFor, "/") != 0)
    { 
      pathFor[--i] = 0;
    }
}
