#include "CLI.h"

#define DEFAULT_BUFFER_LENGTH 100
#define DEFAULT_DELIMITER " "
#define DEFAULT_ARGS_LENGTH 10

CLI::DispatchListNode::DispatchListNode(
    const char * name,
    int (*fn)(int, char**),
    CLI::DispatchListNode * next
): name(name), fn(fn), next(next){
}


CLI::CLI(
    Stream& inputStream,
    unsigned short bufferLength,
    const char *delimiter,
    unsigned short argsLength
): stream(inputStream), bufferLength(bufferLength), delimiter(delimiter), argsLength(argsLength){
    init();
}


CLI::CLI(
    Stream& inputStream,
    unsigned short bufferLength,
    const char *delimiter
): stream(inputStream), bufferLength(bufferLength), delimiter(delimiter), argsLength(DEFAULT_ARGS_LENGTH){
    init();
}


CLI::CLI(
    Stream& inputStream,
    unsigned short bufferLength
):  stream(inputStream),
    bufferLength(bufferLength),
    delimiter(DEFAULT_DELIMITER),
    argsLength(DEFAULT_ARGS_LENGTH){
    init();
}


CLI::CLI(
    Stream& inputStream
):  stream(inputStream),
    bufferLength(DEFAULT_BUFFER_LENGTH),
    delimiter(DEFAULT_DELIMITER),
    argsLength(DEFAULT_ARGS_LENGTH){
    init();
}


void CLI::init(){
    buffer = new char[bufferLength];
    args = new char*[argsLength];
    functionList = NULL;
}


void CLI::registerFunction(const char *name, int (*function)(int, char**)) {
    functionList = new CLI::DispatchListNode(name, function, functionList);
}


int CLI::dispatch(int argc, char **argv){
    DispatchListNode *row = functionList;
    while (row != NULL){
        if (strcmp(argv[0], row->name) == 0){
            return (*(row->fn))(argc-1, argv+1);
        }
        row = row->next;
    }
    return E_CLI_UNKNOWN_COMMAND;
}


int CLI::readArgs(){
    char* next;
    int count = 0;
    args[count++] = strtok(buffer, delimiter);
    while (next = strtok(NULL, delimiter)){
        args[count++] = next;
        if (count >= argsLength){
            break;
        }
    }
    return count;
}


int CLI::process(){
    int argc = readArgs();
    if (argc == 0){
        return E_CLI_NO_COMMAND; // no command at all (empty line, e.g all spaces)
    }
    return dispatch(argc, args);
}


int CLI::feed(){
    char ch;
    while (stream.available()) {
        ch = stream.read();

        if (ch == '\n' or ch == '\r'){
            if (bytesRead == 0){
                continue; // no input so far, ignore
            }
            buffer[bytesRead] = '\0';
            bytesRead = 0;
            return process();
        }
        if (bytesRead < bufferLength-1){
            buffer[bytesRead++] = ch;
        }
    }
    return E_CLI_NO_COMMAND; // no command performed this time (no line terminator arrived)
}

