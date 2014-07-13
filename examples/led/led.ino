/**
 * Example for arduino-cli.
 * Blinks with LED on digital pin 13 (built in on Arduino Nano v.3 board)
 * Waits for command "blink 1" or "blink 0" on a new line on serial input.
 */

#include <CLI.h>

#define LED 13

// Create cli interface, using Serial as input stream, all other parameters are kept default
CLI cli = CLI(Serial);

boolean blinkEnabled = false;
boolean ledState = false;

void setup() {
    // initialize serial
    Serial.begin(9600);
    // set led pin to output
    pinMode(LED, OUTPUT);
    //register blinkLed function to CLI, command alias is "blink"
    cli.registerFunction("blink", &blinkLed);
}

/**
 * Function to be bound, it has to have these arguments
 * @param argc number of arguments
 * @param argv array of arguments
 */
int blinkLed(int argc, char **argv){
    //check the number of arguments
    if (argc != 1){
        Serial.println("Bad argument number");
        return -1;
    }
    //convert the first argument to integer value and evaluate. Everything other than zero is treated as true
    if (atoi(argv[0])) {
        Serial.println("LED BLINK ON");
        blinkEnabled = true;
        return 2;
    } else {
        Serial.println("LED BLINK OFF");
        blinkEnabled = false;
        digitalWrite(LED, LOW);
        return 1;
    }
}

void loop() {
    if (blinkEnabled){
        if (ledState){
            digitalWrite(LED, LOW);
        } else {
            digitalWrite(LED, HIGH);
        }
        ledState = !ledState;
        delay(500); //This cause the reaction to be quite slow. Better method should be used.
    }
}

void serialEvent() {
    /**
     * Reading from serial can be placed here.
     * We read at most one line each cycle,
     * to allow loop() function to be called
    */
    int retval = cli.feed(); //read at most one line from input, return value or error code.
    if (retval == E_CLI_UNKNOWN_COMMAND){
        Serial.println("Unknown command");
    }
}

