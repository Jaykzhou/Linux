// 递归读取某个目录下的所有文件
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
using namespace std;

// 目录结构：树形结构 -> 递归
void DirCount(DIR* dirname)
{
    if(dirname == NULL) return;

    struct dirent* dirent = NULL; 
    while((dirent = readdir(dirname)) != NULL)
    {
        stringstream ss;
        
        // 去除 . 和 .. 的情况
        if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
        {
            continue;
        }

        char buf[1024];
        strcpy(buf, dirent->d_name);
        if(dirent->d_type == DT_DIR)
        {
            // 当前文件是一个目录 -> 目录
            DIR* dir = opendir(buf);
            DirCount(dir);
        }
        else if(dirent->d_type == DT_REG) 
        {
            // 当前不是一个目录 -> 输出信息
            ss << "文件名称: " << dirent->d_name << " ";
            ss << "文件inode: " << dirent->d_ino << " ";
            ss << "文件大小: " << dirent->d_reclen << endl;

            cout << ss.str();
        }
    }

    closedir(dirname);
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "./a.out 目录" << endl;
        exit(-1);
    }

    // 获取指定目录名称
    char* dirname = argv[1];

    // 打开目录
    DIR* dir = opendir(dirname);
    if(dir == NULL)
    {
        cout << "opendir error!" << endl;
        exit(-1);
    }

    // 递归处理目录
    DirCount(dir);

    return 0;
}
