#include "../utility/utility.h"
#include "back.h"

void implementBackgroundProcess(vector<string> tokens, vector<int>& background_processes){

    int child_pid = fork();

    if (child_pid < 0) {
        perror("Fork failed");
        return;
    }

    if (child_pid == 0) {
        
        setsid(); // Create a new session and detach from the terminal
        char** arr = new char*[tokens.size()];
        for(int i=0;i<tokens.size()-1;i++){
            arr[i] = getCharPointerFromString(tokens[i]);
        }

        char* str = getCharPointerFromString(tokens[0]);

        if (execvp(str, arr) == -1) {
            perror("execvp"); // Print an error message if execvp fails
        }

    } else {
        background_processes.push_back(child_pid);
    }

}

void implementForegroundProcess(vector<string> tokens){

    int child_pid = fork();
    int status;

    if (child_pid < 0) {
        perror("Fork failed");
        return;
    }

    if (child_pid == 0) {
        
        setsid(); // Create a new session and detach from the terminal
        char** arr = new char*[tokens.size()];
        for(int i=0;i<tokens.size()-1;i++){
            arr[i] = getCharPointerFromString(tokens[i]);
        }

        char* str = getCharPointerFromString(tokens[0]);

        if (execvp(str, arr) == -1) {
            perror("execvp"); // Print an error message if execvp fails
        }

    } else {
        waitpid(child_pid, &status, 0);
        // background_processes.push_back(child_pid);
    }

}