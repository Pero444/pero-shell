#ifndef INPUT_H_
#define INPUT_H_

typedef struct {
    char *input;
    int size;
    //bool isValid;

} InputBuffer;


//InputBuffer createInputBuffer();

void formatInput(char*);
int readInput(char*, int);


#endif // INPUT_H_