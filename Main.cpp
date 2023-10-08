#include "utility/utility.h"
#include "cd/cd.h"
#include "ls/ls.h"
#include "back/back.h"
#include "history/history.h"

using namespace std;

/*
    Utility functions for other functions
*/

void handleCtrlC(int signal);

//Display user path on terminal 
string getDisplayUserpath(string str, string homeDirectory){
    char login_name[256];
    getlogin_r(login_name, sizeof(login_name));

    struct utsname uts;
    uname(&uts);

    // char host_name[256];
    // gethostname(host_name, sizeof(host_name));

    if(str == homeDirectory){
        str = "~";
    }

    string temp1 = login_name;
    string temp2 = uts.sysname;

    string userPath = temp1 + "@" + temp2 + ":" + str + ">";
    return userPath;
}


//Echo implementation
// void implementEcho(vector<string> v){
//     if(v.size() == 1){
//         cout<<endl;
//     }else{
//         for(int i=1;i<v.size();i++){
//             if(i==v.size()-1){
//                 cout<<v[i]<<endl;
//             }else{
//                 cout<<v[i]<<" ";
//             }
//         }
//     }
// }

void searchFile(string current_userpath, string file_name, bool& found_flag){
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(getCharPointerFromString(current_userpath)))) {
        perror("opendir");
    }

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            // Skip "." and ".." directories to avoid infinite loops
            if (getStringFromCharPointer(entry->d_name) == "." || getStringFromCharPointer(entry->d_name) == "..") {
                continue;
            }

            string new_path = current_userpath + "/" + getStringFromCharPointer(entry->d_name);
            searchFile(new_path, file_name, found_flag);
        }else {
            if(getStringFromCharPointer(entry->d_name) == file_name){
                found_flag = true;
                closedir(dir);
                return;
            }
        }
    }
    closedir(dir);
}

void initializeHistory(vector<string>& history){
    char buffer[1];
    int bytes;
    int fd = open("history/history.txt", O_RDONLY);
    if (fd < 0){
        perror("open");
    }

    string str = "";
    while ((bytes = read(fd, buffer, 1)) > 0) {
        if(buffer[0] != '\n'){
            str += buffer[0];
        }else{
            history.push_back(str);
            str = "";
        }
    }
}

//Read each character from user
char readChar() {
    struct termios old, newVal;
    char ch;
    tcgetattr(STDIN_FILENO, &old);
    newVal = old;
    newVal.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newVal);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return ch;
}


int main(){

    // Register the signal handler for Ctrl+C (SIGINT)
    if (signal(SIGINT, handleCtrlC) == SIG_ERR) {
        perror("Signal registration failed");
        return 1;
    }

    // Register the signal handler for Ctrl+Z (SIGINT)
    if (signal(SIGTSTP, handleCtrlC) == SIG_ERR) {
        perror("Signal registration failed");
        return 1;
    }


   bool flag = true;
    string homeDirectory = getPwd();
    string prevDirectory = "";
    vector<int> background_processes;
    vector<string> history;       // Actual history vector which will be copied to external file
    vector<string> dummy_history; // Dummy history vector which will only be used to show recent changes in memory
    string input;                 // We finally get what user has stored

    int idx;

    // current_userpath is used to store current user path in our terminal
    string current_userpath = getPwd();

    while (flag)
    {

        // clear history & dummy_history first
        history.clear();
        dummy_history.clear();

        // Initialize history from the history file
        initializeHistory(history);

        // copy history to dummy_history
        copy(history.begin(), history.end(), back_inserter(dummy_history));

        input = "";
        dummy_history.push_back(input);
        idx = dummy_history.size() - 1;
        char ch;

        cout << getDisplayUserpath(current_userpath, homeDirectory) << input;

        while (true)
        {
            ch = readChar();

            if (ch == '\033')
            { // Possible escape sequence for an arrow key
                ch = readChar();
                if (ch == '[')
                { // An arrow key was pressed
                    ch = readChar();

                    // Up Arrow
                    if (ch == 'A')
                    {
                        printf("\33[2K\r");

                        if (idx == 0)
                        {
                            cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
                        }
                        else
                        {
                            idx--;
                            cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
                        }
                    }

                    // Down Arrow
                    else if (ch == 'B')
                    {
                        printf("\33[2K\r");

                        if (idx == dummy_history.size() - 1)
                        {
                            cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
                        }
                        else
                        {
                            idx++;
                            cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
                        }
                    }
                }
            }
            else if (ch == '\n')
            { // For Enter key
                input = dummy_history[idx];
                cout << endl;
                break;
            }
            else if (ch == '\x7F')
            { // For Delete key(on MAC OS)
                printf("\33[2K\r");
                dummy_history[idx] = dummy_history[idx].substr(0, dummy_history[idx].length() - 1);
                cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
            }
            else if (ch == '\t')
            { // For Tab key
                printf("\33[2K\r");
                
                cout << getDisplayUserpath(current_userpath, homeDirectory) << "Tab";
            }
            else
            { // For all characters
                printf("\33[2K\r");
                dummy_history[idx] += ch;
                cout << getDisplayUserpath(current_userpath, homeDirectory) << dummy_history[idx];
            }
        }

        //add new input to history
        addHistory(history, input);
        // addToFile(history);

        vector<string> commands;
        if (input.find(";") != string::npos)
        {
            commands = getTokens(input, ";");
        }
        else
        {
            commands.push_back(input);
        }

        for (int i = 0; i < commands.size(); i++)
        {

            string cmd = trim(commands[i]);
            if (cmd.find("echo") == 0)
            {
                cout << trim(cmd.substr(4)) << endl;
                continue;
            }

            vector<string> tokens = getTokens(commands[i], " ");

            if (tokens[0] == "exit")
            {
                flag = false;
                continue;
            }

            if (tokens[0] == "pwd")
            {
                cout << getPwd() << endl;
                continue;
            }

            if (tokens[0] == "cd")
            {
                implementCd(tokens, homeDirectory, prevDirectory, current_userpath);
                continue;
            }

            if (tokens[0] == "ls")
            {
                implementLs(tokens, homeDirectory, current_userpath);
                continue;
            }

            if (tokens[tokens.size() - 1] == "&")
            {
                implementBackgroundProcess(tokens, background_processes);
                for (int i = 0; i < background_processes.size(); i++)
                {
                    cout << "[" << i + 1 << "] : " << background_processes[i] << endl;
                }
                continue;
            }

            if (tokens[0] == "search")
            {
                if (tokens.size() > 2)
                {
                    cout << "error : invalid arguments for error handling" << endl;
                    continue;
                }

                bool found_flag = false;
                searchFile(current_userpath, tokens[1], found_flag);
                if (found_flag)
                {
                    cout << "true" << endl;
                }
                else
                {
                    cout << "false" << endl;
                }
                continue;
            }

            if (tokens[0] == "history")
            {
                showHistory(tokens, history);
                continue;
            }
            else
            {
                implementForegroundProcess(tokens);
                // cout<<tokens[0]<<": command not found"<<endl;
            }
        }
    }


    return 0;
}

//Handle `Control + C` event
void handleCtrlC(int signal) {
    cout<<endl;
    main();
    // logic();
}