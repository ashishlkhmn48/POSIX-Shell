#include "../utility/utility.h"
#include "ls.h"

class File{
    public : 
    char isDirectory;
    string permissions;
    unsigned short link;
    string user_name;
    string group_name;
    long long size;
    string date;
    string name;
};

string get_permissions(mode_t mode) {
    string result = "";
    result += S_ISDIR(mode) ? 'd' : '-';

    result += mode & S_IRUSR ? 'r' : '-';
    result += mode & S_IWUSR ? 'w' : '-';
    result += mode & S_IXUSR ? 'x' : '-';

    result += mode & S_IRGRP ? 'r' : '-';
    result += mode & S_IWGRP ? 'w' : '-';
    result += mode & S_IXGRP ? 'x' : '-';

    result += mode & S_IROTH ? 'r' : '-';
    result += mode & S_IWOTH ? 'w' : '-';
    result += mode & S_IXOTH ? 'x' : '-';

    return result;
}

//Compare funtion for sorting File objects according 
bool compare(File f1, File f2)
{
    return (f1.name < f2.name);
}

void printInFormat(File file){
    printf("%s %2hu %-8s %-8s %8lld %s %s\n", 
    getCharPointerFromString(file.permissions) ,file.link,
    getCharPointerFromString(file.user_name), getCharPointerFromString(file.group_name),
    file.size, getCharPointerFromString(file.date), getCharPointerFromString(file.name));
}

//Get all file details inside given directory
File get_file_Details(string full_path){
    File file;
    struct stat info;
    if (stat(getCharPointerFromString(full_path), &info) == -1) {
        cout<<full_path<<endl;
        perror("stat");
        return file; 
    }

    struct passwd *user_info = getpwuid(info.st_uid);
    struct group *group_info = getgrgid(info.st_gid);
    char time_buf[80];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&info.st_mtime));

    file.isDirectory = S_ISDIR(info.st_mode) ? 'd' : '-';
    file.permissions = get_permissions(info.st_mode);
    file.link = info.st_nlink;
    file.user_name = user_info->pw_name;
    file.group_name = group_info->gr_name;
    file.size = (long long)info.st_size;
    file.date = time_buf;

    return file;
}


//Get all file/directory details inside given directory
//flag for the fact that when we should print the total value and which total value i.e. total_la or total_l
vector<File> get_Files_list_directory(string path, string flag) {
    vector<File> result;

    DIR *dir = opendir(getCharPointerFromString(path));
    if (dir == NULL) {
        perror("opendir");
        return result;
    }

    //If path given is a directory
    struct dirent *entry;
    long long total_la = 0;
    long long total_l = 0;
    while ((entry = readdir(dir)) != NULL) {
        File file;
        string full_path = path;

        full_path = full_path + entry->d_name;
        
        struct stat info;
        if (stat(getCharPointerFromString(full_path), &info) == -1) {
            cout<<full_path<<endl;
            perror("stat");
            continue;
        }

        file = get_file_Details(full_path);
        //Change file name again to be specific
        file.name = entry->d_name;

        total_la += info.st_blocks;
        if(file.name.find(".") != 0){
            total_l += info.st_blocks;;
        }

        result.push_back(file);
    }

    sort(result.begin(), result.end(), compare);

    if(flag == "l"){
        cout<<"total "<<total_l<<endl;
    }else if(flag == "la"){
        cout<<"total "<<total_la<<endl;
    }
    
    closedir(dir);
    return result;
}


void printDetailsForDirectory(unordered_set<char> flags, vector<string> directory_list){
    
    // For simple case `ls`
    if(flags.size() == 0){
        for(int i=0;i<directory_list.size();i++){
            if(directory_list.size() > 1){
                cout<<directory_list[i]<<":"<<endl;
            }

            if(isPathFile(directory_list[i])){
                cout<<directory_list[i]<<endl;
                    continue;
            }

            vector<File> files = get_Files_list_directory(directory_list[i], "");
            for(int j=0;j<files.size();j++){
                if(files[j].name.find(".") != 0){
                    cout<<files[j].name<<"\t";
                }
            }
            
            if(i != directory_list.size()-1){
                cout<<endl;
            }
        }
        cout<<endl;
        return;
    }

    // For case `ls -l`
    if(flags.find('l') != flags.end() && flags.find('a') == flags.end()){
        for(int i=0;i<directory_list.size();i++){
            if(directory_list.size() > 1){
                cout<<directory_list[i]<<":"<<endl;
            }

            if(isPathFile(directory_list[i])){
                File f = get_file_Details(directory_list[i]);
                printInFormat(f);
                cout<<endl;
                continue;
            }

            vector<File> files = get_Files_list_directory(directory_list[i], "l");
            for(int j=0;j<files.size();j++){
                File file = files[j];
                if(file.name.find(".") != 0){
                    printInFormat(file);
                }
            }
            if(i != directory_list.size()-1){
                cout<<endl;
            }
        }
        cout<<endl;
        return;
    }

    // For case `ls -a`
    if(flags.find('l') == flags.end() && flags.find('a') != flags.end()){
        for(int i=0;i<directory_list.size();i++){
            if(directory_list.size() > 1){
                cout<<directory_list[i]<<":"<<endl;
            }

            if(isPathFile(directory_list[i])){
                cout<<directory_list[i]<<endl;
                continue;
            }

            vector<File> files = get_Files_list_directory(directory_list[i], "a");
            for(int j=0;j<files.size();j++){
                cout<<files[j].name<<"\t";
            }
            if(i != directory_list.size()-1){
                cout<<endl<<endl;
            }
        }
        cout<<endl;
        return;
    }

    // For case `ls -la`
    if(flags.find('l') != flags.end() && flags.find('a') != flags.end()){
        for (int i = 0; i < directory_list.size(); i++){
            if (directory_list.size() > 1){
                cout << directory_list[i] << ":" << endl;
            }

            if (isPathFile(directory_list[i])){
                File f = get_file_Details(directory_list[i]);
                printInFormat(f);
                cout << endl;
                continue;
            }

            vector<File> files = get_Files_list_directory(directory_list[i], "la");
            for (int j = 0; j < files.size(); j++){
                printInFormat(files[j]);
            }

            if (i != directory_list.size() - 1){
                cout << endl;
            }
        }
        cout<<endl;
    }
}


//ls implementation
//Convention used : Whatever the directory is given by the user is converted to following format : 
//1. /dir_x/dir_y  --> /dir_x/dir_y/
//2. /dir_x/dir_y/  --> remains same
//3. Any relative path  --> Absolute path
void implementLs(vector<string> tokens, string homeDirectory, string userpath){
    unordered_set<char> flags;
    vector<string> directory_list;

    if(tokens.size() == 1){
        if(userpath != "/"){
            directory_list.push_back(userpath + "/");
        }else{
            directory_list.push_back(userpath);
        }
        printDetailsForDirectory(flags, directory_list);
    }else{
        int i=1;
        while(i<tokens.size() && tokens[i].find('-') == 0){
            for(int j=1;j<tokens[i].length();j++){
                flags.insert(tokens[i][j]);
            }
            i++;
        }

        if(i == tokens.size()){
            if(userpath != "/"){
                directory_list.push_back(userpath + "/");
            }else{
                directory_list.push_back(userpath);
            }
        }

        while(i<tokens.size()){
            if(tokens[i] == "."){
                string curr = userpath;
                if(curr != "/"){
                    directory_list.push_back(curr + "/");
                }else{
                    directory_list.push_back(curr);
                }
            }else if(tokens[i] == ".."){
                string prev = getPreviousDirectory(userpath);
                if(prev != "/"){
                    directory_list.push_back(prev + "/");
                }else{
                    directory_list.push_back(prev);
                }
            }else if(tokens[i] == "~"){
                if(homeDirectory != "/"){
                    directory_list.push_back(homeDirectory + "/");
                }else{
                    directory_list.push_back(homeDirectory);
                }
            }else{
                string path = tokens[i];
                if(!isPathAbsolute(path)){
                    path = userpath + path;
                }

                if(isPathFile(path)){
                    directory_list.push_back(path);
                }else{
                    if(path[path.length()-1] != '/'){   
                        directory_list.push_back(tokens[i]+"/");
                    }else{
                        directory_list.push_back(tokens[i]);
                    }
                }
            }
            i++;
        }

        printDetailsForDirectory(flags, directory_list);
    }
}