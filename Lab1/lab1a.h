// Lab 1
# ifndef LAB1A_C
# define LAB1A_C
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

struct Node;
//Global Variables
struct Node *root, *cwd;		// root and CWD pointers
char line[128];			// User input line
char command[16], pathname[64];	// User inputs
char dirname[64], basename[64];	// String holders


//Node struct
typedef struct Node
{
	char name[64];
	char type;
	struct Node *childPtr, *siblingPtr, *parentPtr;
}Node;

void initRoot();
void printMenu();
void getInput();
int findCommand();
int mkdir();
int rmdir();
int cd ();
int ls();
void pwd();
int creat();
int rm();
int save();
int reload();
int quit();
void breakPathname();
Node* findNode();
int invalidPathError(Node* current);
int invalidFileTypeError(Node* current, char type);
Node* makeNode(Node* parent, char type);
int nameAlreadyExistsError(Node* current);
void deleteDir(Node* current);
void printFile(FILE* infile, Node* current);
void findAbsolutePath(Node* current, char* pathFor);
//int (*fptr[])() = {printMenu, mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, quit};
#endif
