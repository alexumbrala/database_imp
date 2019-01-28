#ifndef MYDB_PAGE_H
#define MYDB_PAGE_H
#include <stddef.h>
#include <iostream>
#include <string>
#include "MyDB_Table.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
using std::string;

template<typename T>
class Page;

typedef shared_ptr<Page<char>> PagePtr;

template<typename T>
class Page{

private:
    
    // number of data
    const size_t size;

    // content of this page
    T* data;

    // if data changed
    bool dirty;

    T defaultValue;

    MyDB_TablePtr table;

    long idx;

public:

    Page(const size_t pageSize, T deft,T *data):
        size(pageSize),
        defaultValue(deft),
        data(data)
        {}

    ~Page(){
        std::cout<<"destroy page"<<std::endl;
        writeToFile();
    }

    T* getData(){
        return data;
    }

    void setDataDirty(){
        dirty = true;
    }

    void readFromTable(MyDB_TablePtr table, long idx){
        Page::table = table;
        Page::idx = idx;
        int fd;
        string tableFile = table->getStorageLoc()+"_"+table->getName();
        if ((fd=open(tableFile.data(),O_FSYNC|O_RDWR))==-1){
            fd = open(tableFile.data(),O_FSYNC|O_CREAT|O_RDWR,0777);
        }
        lseek(fd,idx*size,SEEK_SET);
        ssize_t readLen = read(fd,data,size);
        if(readLen<0)
            std::cerr<<"cannot read content from table:"<<table->getName()<<" page:"<<idx<<endl;
        close(fd);
    }

    void writeToFile(){
        if(!dirty) return;
        dirty = false;
        int fd;
        string tableFile = table->getStorageLoc()+"_"+table->getName();
        if ((fd=open(tableFile.data(),O_FSYNC|O_RDWR))==-1){
            std::cerr<<"fail to open table for writing:"<<table->getName()<<" page:"<<idx<<endl;
            return;
        }
        lseek(fd,idx*size,SEEK_SET);
        ssize_t writeLen = write(fd,data,size);
        if(writeLen<=0)
            std::cerr<<"cannot read content from table:"<<table->getName()<<" page:"<<idx<<endl;
        close(fd);
    }
};

#endif

