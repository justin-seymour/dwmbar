
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include"unistd.h"

#include <X11/Xlib.h>

// Treat as linked list
struct block {
    char command[50];
    char label[50];
    int count;
    // Bring in signal functionality?

    struct block *next;
};

static char statusstr[256];
char blockNum = 0;
struct block *commandHead = NULL;
struct block *currCommand = NULL;

char *delim = " | ";

//void setroot();
//static void (*writestatus) () = setroot;
void runCommand(struct block *block, char *output);
void getCommandOutput(int count, char commandArr[blockNum][100]);
void initCommands();
void writeStatus();

// Write to status bar
void writeStatus(char commandArr[blockNum][100])
{
    // Combine commands to string
    char tempHold[256] = "";
    
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
void runCommand(struct block *block, char *output)
{
    char *cmd = block->command;
    FILE *cmdf = popen(cmd, "r");

    if (!cmdf)
        return;

    char tempHold[100] = "";
    fgets(tempHold, 100, cmdf);
    pclose(cmdf);

    strcpy(output, "");
    strcat(output, block->label);
    strcat(output, " ");
    strcat(output, tempHold);

    // Remove new line
    char *token = strtok(output, "\n");

    strcpy(output, token);
}

// Get output from all blocks and place into array
void getCommandOutput(int time, char commandArr[blockNum][100])
{
    struct block *temp = commandHead;

    for (int i = 0; i < blockNum; ++i)
    {
        if ((time % temp->count == 0 && temp->count != 0) || time == -1)
            runCommand(temp, commandArr[i]);

        temp = temp->next;
    }
}

// Read commands from file and insert into linked list
void initCommands()
{
    FILE *fp = fopen("commands","r");

    if (fp == NULL)
    {
        printf("Could not open file\n");
        exit(EXIT_FAILURE);
    }

    int bufferLength = 100;
    char buffer[bufferLength];

    char *token;
    struct block *temp = NULL;

    while(fgets(buffer, bufferLength, fp))
    {
        // Rudimentary comment blocking
        if (buffer[0] != '#')
        {
            // Init node
            temp = (struct block*)malloc(sizeof(struct block));
            temp->next = NULL;

            // Get values
            token = strtok(buffer, "#");
            //temp->label = token;
            strcpy(temp->label, token);

            token = strtok(NULL, "#");
            //temp->command = token;
            strcpy(temp->command, token);

            token = strtok(NULL, "/n");
            temp->count = atoi(token);

            if (commandHead == NULL)
                commandHead = temp;
            else
            {
                currCommand = commandHead;
                while (currCommand->next != NULL)
                    currCommand = currCommand->next;

                currCommand->next = temp;
            }

            ++blockNum;
        }
    }

    fclose(fp);
}

int main()
{
    // Setup daemon stuff

    // Setup commands
    initCommands();

    char commandOutput[blockNum][100];

    // Remove garbage
    for (int i = 0; i < blockNum; ++i)
        strcpy(commandOutput[i], "");

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
