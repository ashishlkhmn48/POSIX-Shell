#include "../utility/utility.h"
#include "cd.h"

//All cd implementation
void implementCd(vector<string> tokens, string homeDirectory, string& prevDirectory, string& userpath){

    if(tokens.size() == 1){
        return;
    }

    if(tokens.size() > 2){
        cout<<"error : invalid arguments for error handling"<<endl;
        return;
    }
            
    if(tokens[1] == ".."){
        prevDirectory = getPwd();
        chdir("..");
        userpath = getPwd();
        return;
    }

    if(tokens[1] == "~"){
        chdir(getCharPointerFromString(homeDirectory));
        userpath = "~";
        return;
    }

    if(tokens[1] == "-"){
        if(prevDirectory==""){
            chdir("..");
            perror("error");
        }else{
            string temp = getPwd();
            chdir(getCharPointerFromString(prevDirectory));
            
            if(getPwd() == homeDirectory){
                userpath = "~";
            }else{
                userpath = getPwd();
            }
            
            prevDirectory = temp;
            cout<<getPwd()<<endl;
        }
        return;
    }

    if(chdir(getCharPointerFromString(tokens[1])) != 0){
        perror("error");
    }else{
        userpath = getPwd();
    }
}