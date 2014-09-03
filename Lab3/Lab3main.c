/* Kristin Onorati
 * 10874126
 * CPTS 360 Lab 3
 * Group 1 Seq 3
 */

#include "Lab3.h"

int main (int argc, char **argv, char* const envp[])
{
    char* inputString;
    char** inputArray;
  
    findPath();
    findHome();

    while(1)
    {
        inputString = getInput();
        inputArray = parseInput(inputString);
        handleCommand(inputArray, envp);
    }
	return 0;
}
