#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

// Global flag to track if we're in the process of handling SIGINT
volatile sig_atomic_t handling_signal = 0;

// Function to handle cleanup operations
void cleanup() {
    printf("\nPerforming cleanup operations...\n");
    // Add your cleanup code here, such as:
    // - Closing open files
    // - Freeing allocated memory
    // - Closing network connections
    // - Saving any important state
}

void sigint_handler(int signum) {
    // Prevent re-entrance
    if (handling_signal) {
        return;
    }
    handling_signal = 1;

    char input[32];
    printf("\nDo you really want to exit? (y/n): ");
    fflush(stdout);  // Ensure the prompt is displayed

    if (fgets(input, sizeof(input), stdin)) {
        if (input[0] == 'y' || input[0] == 'Y') {
            printf("\nReceived confirmation. Cleaning up...\n");
            cleanup();
            exit(0);
        } else {
            printf("\nContinuing program execution...\n");
            handling_signal = 0;
            return;
        }
    }

    // If we couldn't read input (maybe stdin was closed),
    // perform cleanup and exit anyway
    cleanup();
    exit(1);
}

int main() {
    // Set up the signal handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    printf("Program is running. Press Ctrl+C to test the handler...\n");

    // Main program loop
    while(1) {
        // Your program's main logic goes here
        sleep(1);  // Just to prevent CPU hogging in this example
    }

    return 0;
}