#include "LRU_Cache.h"
#include <iostream>
#include <memory>
#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using std::make_shared;
using std::cout;
using std::endl;


void test_MyDB_page();
void test_IO();

int main(){

    test_MyDB_page();
//    test_IO();

    return 0;
}

void test_IO(){
    int fd;
    string fileName = "test_file";

    if((fd = open(fileName.data(),O_FSYNC|O_RDWR)) == -1){
        cout<<"open file fail"<<endl;
        fd = open(fileName.data(),O_FSYNC|O_CREAT|O_RDWR,0777);
    }
    long idx = 1;
    size_t  pageSize = 64;

    __off_t fileLen = lseek(fd,idx*pageSize,SEEK_SET);
    string content = "Yes, I am fine";
    ssize_t writeLen = write(fd,content.data(),content.size()-6);
    cout<<"fd->"<<fd<<"   file len->"<<fileLen<<"   wirte len->"<<writeLen<<endl;
    int fd2 = open(fileName.data(),O_FSYNC|O_RDWR);
    if(fd2==-1) cout<<"open file fd2 fails"<<endl;
    lseek(fd2,idx*pageSize,SEEK_SET);
    char* readBuf = (char*) malloc(sizeof(char)*pageSize);
    ssize_t readLen = read(fd2,readBuf,pageSize);
    cout<<"readLen:"<<readLen<<endl;
    printf("read buffer: %s",readBuf);
    close(fd);

}


void test_MyDB_page(){
    size_t pageSize =1;
    size_t pageNum = 4;
    MyDB_BufferManager manager(pageSize,pageNum,"file");
    MyDB_TablePtr table1 = make_shared<MyDB_Table>("table1","home");
    MyDB_TablePtr table2 = make_shared<MyDB_Table>("table2","home");

    MyDB_PageHandle handler1 = manager.getPage(table1,1);
    char* data1 = (char*)handler1->getBytes();
    *data1 = 'a';
    handler1->wroteBytes();

    MyDB_PageHandle handler2 = manager.getPage(table1,2);
    char* data2 = (char*)handler2->getBytes();
    *data2 = 'b';
    handler2->wroteBytes();

    MyDB_PageHandle tempHandler = manager.getPage(table2,1);
    char* data3 = (char*)tempHandler->getBytes();
    *data3 = 'b';
    tempHandler->wroteBytes();
    tempHandler = manager.getPage(table1,2);
    tempHandler = manager.getPage(table2,2);
    tempHandler = manager.getPage(table2,3);
//    handler1->getBytes();
    handler2->getBytes();
}