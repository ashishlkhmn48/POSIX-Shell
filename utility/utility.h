#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <sstream> 
#include <pwd.h>
#include <unordered_set>
#include <termios.h>

using namespace std;

char* getCharPointerFromString(string str);
string getStringFromCharPointer(char* str);
vector<string> getTokens(string str, string delimiter);
string getPwd();
string getPreviousDirectory(string userpath);
bool isPathFile(string path);
bool isPathAbsolute(string path);
string trim(string str);
void printStringVector(vector<string> v);