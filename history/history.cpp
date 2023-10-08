#include "../utility/utility.h"
#include "history.h"

void addToFile(vector<string> history){
    int fd = open("history/history.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open");
        return;
    }

    // Write each line to the file
    for (int i = 0; i < history.size(); i++) {
        long bytes_written = write(fd, getCharPointerFromString(history[i]+"\n"), history[i].length()+1);

        if (bytes_written == -1) {
            perror("write");
            close(fd);
        }
    }

    close(fd);
}

void addHistory(vector<string>& history, string input){
    if(history.size() < 20){
        history.push_back(input);
    }else{
        vector<string> new_history;
        for(int i=1;i<20;i++){
            new_history.push_back(history[i]);
        }
        new_history.push_back(input);
        history = new_history;
    }
    addToFile(history);
}

bool isNumber(string s){
    for(int i=0;i<s.length();i++){
        if(s[i] < '0' || s[i] > '9'){
            return false;
        }
    }

    return true;
}

void showHistory(vector<string> tokens, vector<string> history){
    if(tokens.size() > 2){
        cout<<endl<<"error : Invalid number of arguments"<<endl;
        return;
    }

    if(tokens.size() == 1){
        for(int i=0;i<min(10, (int)history.size());i++){
            cout<<i+1<<"    "<<history[i]<<endl;
        }
        return;
    }

    if(tokens.size() == 2){
        if(!isNumber(tokens[1])){
            cout<<"error : 2nd argument should be a number"<<endl;
            return;
        }

        if(stoi(tokens[1]) > 20 || stoi(tokens[1]) <= 0){
            cout<<"error : 2nd argument should be greater than 0 and less than equal or to 20"<<endl;
            return;
        }

        cout<<"Number = "<<stoi(tokens[1])<<endl;
        for(int i=0;i<min(stoi(tokens[1]), (int)history.size());i++){
            cout<<i+1<<"    "<<history[i]<<endl;
        }
    }

}
