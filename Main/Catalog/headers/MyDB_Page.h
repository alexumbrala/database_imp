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
    size_t size;

    // content of this page
    T* data;

    // if data changed
    bool dirty;

    T defaultValue;

    MyDB_TablePtr table;

    long idx;

public:
    /**
     *  constructor
     */
    Page(size_t pageSize, T deft): size(pageSize),defaultValue(deft){
        dirty = false;
        data = (T*)malloc(sizeof(T)*pageSize);
    }
    
    ~Page(){
        std::cout<<"destroy page"<<std::endl;
        writeToFile();
        free(data);
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
            std::cerr<<"cannot read content from table"<<endl;
        close(fd);
    }


    void writeToFile(){
        if(!dirty) return;
        dirty = false;
        int fd;
        string tableFile = table->getStorageLoc()+"_"+table->getName();
        if ((fd=open(tableFile.data(),O_FSYNC|O_RDWR))==-1){
            std::cerr<<"fail to open table for writing"<<endl;
            return;
        }
        lseek(fd,idx*size,SEEK_SET);
        ssize_t writeLen = write(fd,data,size);
        if(writeLen<=0)
            std::cerr<<"cannot read content from table"<<endl;
        close(fd);
    }

    void resetDirtyFlag(){
        dirty = false;
    }



};

#endif

