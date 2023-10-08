#include "utility.h"

using namespace std;

//Convert string to char*
char* getCharPointerFromString(string str){
    char* s = (char*)calloc(str.length(), sizeof(char));
    for(int i=0;i<str.length();i++){
        s[i] = str[i];
    }

    return s;
}

//Convert char* to string
string getStringFromCharPointer(char* str){
    int len = strlen(str);
    string s = "";
    for(int i=0;i<len;i++){
        s += str[i];
    }

    return s;
}

vector<string> getTokens(string str, string delimiter){
    vector<string> result;
    char* token = strtok(getCharPointerFromString(str), getCharPointerFromString(delimiter));
    while (token != NULL) {
        result.push_back(token);
        token = strtok(NULL, getCharPointerFromString(delimiter));
    }
    return result;
}

//Get Present Working Directory
string getPwd(){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    return getStringFromCharPointer(cwd);
}

//Get Previous Directory compared to existing one
string getPreviousDirectory(string userpath){
    string currentDirectory = userpath;
    string result = "";
    if(currentDirectory == "/"){
        return "/";
    }else{
        int j=currentDirectory.length()-1;
        while(j>=0 && currentDirectory[j] != '/'){
            j--;
        }
        return currentDirectory.substr(0, j);
    }
}

//To check if given path is file or not
bool isPathFile(string path){
    struct stat path_stat;
    stat(getCharPointerFromString(path), &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//Check if path is absolute or not
bool isPathAbsolute(string path){
    return path[0] == '/';
}


string trim(string str) {
    const char* typeOfWhitespaces = " \t\n\r\f\v";
    str.erase(str.find_last_not_of(typeOfWhitespaces) + 1);
    str.erase(0,str.find_first_not_of(typeOfWhitespaces));
    return str;
}

void printStringVector(vector<string> v){
    for(int i=0;i<v.size();i++){
        cout<<v[i]<<" ";
    }
    cout<<endl;
}