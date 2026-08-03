#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "parser.h"

// typedef enum {
//     EXECTU
// } executorStatus;

typedef struct {
    Pipeline* pipeline;
    int pos;
    int count;

} Executor;

int execute(Pipeline*);

#endif // EXECUTOR_H_