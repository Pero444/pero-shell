#ifndef INPUT_H_
#define INPUT_H_

typedef enum {
    DEFAULT_PROMPT,
    INCOMPLETE_PROMPT

} Prompt;

//const char* promptTexts[];

char* takeInput(Prompt prompt);

#endif // INPUT_H_