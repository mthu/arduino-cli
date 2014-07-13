/**
Arduino-cli, easy configurable CLI for stream like input (e.g. Serial).

Copyright (c) 2014, Martin Plicka
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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

