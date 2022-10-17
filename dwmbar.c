
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Xlib.h>

// Stored as array in header
typedef struct {
    char label[50];
    void (*function)(char *);
    int count;

    // Bring in signal functionality?
} Block;

#include "commands.c"

static char statusstr[300];
char blockNum = 0;

// Write to status bar
void writeStatus(char commandArr[blockNum][100])
{
    // Combine commands to string
    char tempHold[300] = "";

    for (int i = 0; i < blockNum; ++i)
    {
        if (strcmp(tempHold, ""))    
            strcat(tempHold, delim);

        strcat(tempHold, commandArr[i]);
    }

    // Write to XSetRoot
    if (strcmp(statusstr, tempHold) != 0)
    {
        strcpy(statusstr, tempHold);

        // Write to xsetroot
        Display *d = XOpenDisplay(NULL);

        int screen = DefaultScreen(d);
        Window root = RootWindow(d, screen);
        XStoreName(d, root, statusstr);
        XCloseDisplay(d);
    }
}

// Run command and return output
void runCommand(Block *block, char *output)
{
    // Call function and write output to hold
    char hold[100];
    (*block->function)(hold);

    // Return early if string is empty
    if (strcmp(hold, "") == 0)
        return;

    // Add label to it
    char newString[150] = "";
    strcat(newString, block->label);
    strcat(newString, " ");
    strcat(newString, hold);

    // Output command
    strcpy(output, newString);
}

// Get output from all blocks and place into array
void getCommandOutput(int time, char commandArr[blockNum][100])
{
    for (int i = 0; i < blockNum; ++i)
    {
        if ((blocks[i].count != 0 && time % blocks[i].count == 0) || time == -1)
            runCommand(&blocks[i], commandArr[i]);

    }
}

void getBlockNum()
{
    blockNum = sizeof blocks / sizeof blocks[0];
}

int main()
{
    // Setup daemon stuff
    pid_t pid, sid;

    pid = fork();
    if (pid < 0)
        exit(1);

    if (pid > 0)
        exit(0);

    umask(0);

    sid = setsid();
    if (sid < 0)
        exit(1);


    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Get number of blocks
    getBlockNum();

    // Create output string
    char commandOutput[blockNum][100];

    // Remove garbage
    for (int i = 0; i < blockNum; ++i)
        strcpy(commandOutput[i], "");

    // Get initial state
    getCommandOutput(-1, commandOutput);
    writeStatus(commandOutput);

    int count = 1;
    int maxCount = 500;

    // The big loop
    while (1)
    {
        getCommandOutput(count, commandOutput);

        writeStatus(commandOutput);

        ++count;

        if (count == maxCount)
            count = 1;

        sleep(1);
    }

    return 0;
}
