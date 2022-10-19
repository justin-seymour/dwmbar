
// Simple function to let shell scripts be called
const char * callScript(char *cmd)
{
    FILE *cmdf = popen(cmd, "r");

    if (cmdf == NULL)
        return "Err";

    char tempHold[100] = "";

    if (fgets(tempHold, 100, cmdf) == NULL)
        return "Err";

    pclose(cmdf);

    // Remove new line
    char *token = strtok(tempHold, "\n");

    /* strcpy(output, token); */
    return token;
}

// -----------------------------------------------
// Actual functions/scripts start here

void getMemoryUsage(char *output)
{
    strcpy(output, callScript("~/bin/memory"));
}

void getBattery(char *output)
{
    strcpy(output, callScript("~/bin/battery"));
}

void getVolume(char *output)
{
    strcpy(output, callScript("~/bin/volume"));
}

void getTime(char *output)
{
    strcpy(output, callScript("~/bin/time"));
}

// Set commands as array
Block blocks[] = {
    /* {"", "~/bin/spotify", 5}, */
    {"M: ", &getMemoryUsage, 20},
    {"B: ", &getBattery, 60},
    {"V: ", &getVolume, 1},
    {"", &getTime, 1}
};

// set delimiter
char *delim = " | ";
