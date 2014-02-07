// lab1 main
# include "lab1a.h"

int main()
{   int command_code = 0;
    initRoot();  // Initialize the '/' root directory of the tree
    cwd = root; // Set cwd to root
    while(command_code != 10)
    {
       printf("Enter a command or enter menu: ");
       getInput();
       command_code = findCommand();
       breakPathname();
      // printf("pathname = %s, command = %s, command code = %d, dirname = %s, basename = %s\n", pathname, command, command_code, dirname, basename); // For testing
       switch (command_code)
       {
          case 0: printMenu(); break;
          case 1: mkdir(); break;
          case 2: rmdir(); break;
          case 3: cd(); break;
          case 4: ls(); break;
          case 5: pwd(); break;
          case 6: creat(); break;
          case 7: rm(); break;
          case 8: save(); break;
          case 9: reload(); break;
          case 10: quit(); break;
          default: printf("Invalid command\n"); break;
       }
    }
   return 0;
}
             
