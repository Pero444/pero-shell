#ifndef SHELL_H_
#define SHELL_H_

typedef struct {
    char* username;
    char* hostname;
    char* homepath;
} Config;

typedef Config* config;

int setConfig(config);


void peroLoop(config);

char* prompt(config);

void printTitle();
void printCWD();



#endif // SHELL_H_