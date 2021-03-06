/**
Arduino-cli, easy configurable CLI for stream like input (e.g. Serial).

Copyright (c) 2014, Martin Plicka
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __CLI__h
#define __CLI__h

#include "string.h"
#include "stream.h"

#define E_CLI_UNKNOWN_COMMAND -1000
#define E_CLI_NO_COMMAND 0


class CLI {
    private:
        class DispatchListNode{
            public:
                const char * name;
                int (*fn)(int, char**);
                DispatchListNode *next;
                /**
                 * List node constructor
                 * @param name name of the function
                 * @param fn pointer to the function
                 * @param next pointer to the next chain in table, NULL if this is the last node
                 */
                DispatchListNode(const char * name, int (*fn)(int, char**), DispatchListNode * next);
        };
        
        /**
         * Reference to the input stream (e.g. Serial instance)
         */
        Stream & stream;

        /**
         * Length of the input buffer
         */
        unsigned short bufferLength;

        /**
         * Input buffer, will be allocated in constructor
         */
        char * buffer;

        unsigned short argsLength;
        /**
         * Array of pointers to chunked zero-terminated arguments stored in the buffer
         */
        char ** args;

        /**
         * At least one-character long string which delimits arguments on the command line
         */
        const char * delimiter;

        /**
         * Number of bytes read on the current line so far
         */
        unsigned short bytesRead;

        /**
         * Dispatch table begin
         */
        DispatchListNode *functionList;

        /**
         * Common constructor code
         */
        void init();

        /**
         * Find and executes command using dispatch table
         * @param argc number of arguments
         * @param argv array of arguments (null-terminated strings)
         * @return return value of command or error code
         */
        int dispatch(int argc, char **argv);

        /**
         * Process input line in buffer and calls command
         * @return return value of command or error code
         */
        int process();
        
        /**
         * Parse arguments from buffer into args array
         * @return number of arguments parsed
         * @see args
         */
        int readArgs();
    public:
        /**
         * Initialize CLI.
         * @param inputStream input stream, e.g. Serial instance
         */
        CLI(Stream& inputStream);

        /**
         * Initialize CLI.
         * @param inputStream input stream, e.g. Serial instance
         * @param lineLength maximum line lenght to be able to parse, default 100
         */
        CLI(Stream& inputStream, unsigned short lineLength);

        /**
         * Initialize CLI.
         * @param inputStream input stream, e.g. Serial instance
         * @param lineLength maximum line lenght to be able to parse, default 100
         * @param delimiter string used to delimit the arguments on each line, default is space (0x20)
         */
        CLI(Stream& inputStream, unsigned short lineLength, const char *delimiter);

        /**
         * Initialize CLI.
         * @param inputStream input stream, e.g. Serial instance
         * @param lineLength maximum line lenght to be able to parse, default 100
         * @param delimiter string used to delimit the arguments on each line, default is space (0x20)
         * @param argsLength maximum number of arguments to be parsed from input line, default 10
         */
        CLI(Stream& inputStream, unsigned short lineLength, const char *delimiter, unsigned short argsLength);

        /**
         * Register function into the dispatch table, see example
         * @param name function name, case sensitive
         * @param function pointer to the function with arguments (int, char**), returning an int.
         * See example
         */
        void registerFunction(const char *name, int (*function)(int, char**));

        /**
         * Read next line from input (if any) and execute command. 
         * @return return value of called function or error code.
         */
        int feed();
};

#endif
