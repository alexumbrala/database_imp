#include "LRU_Cache.h"
#include <iostream>
#include <memory>
#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>

using std::make_shared;
using std::cout;
using std::endl;
using std::vector;

void test_MyDB_page();
void test_IO();
// these functions write a bunch of characters to a string... used to produce data
void writeNums (char *bytes, size_t len, int i);
void writeLetters (char *bytes, size_t len, int i);
void writeSymbols (char *bytes, size_t len, int i);
void projTest();


int main(){

    projTest();

    return 0;
}



void projTest(){
    // create a buffer manager
    MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
    MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");
    // allocate a pinned page
    cout << "allocating pinned page\n";
    MyDB_PageHandle pinnedPage = myMgr.getPinnedPage (table1, 0);
    char *bytes = (char *) pinnedPage->getBytes ();
    writeNums (bytes, 64, 0); //{0,1,2,3,..,9,0,1,...,0,1,2,'\0'}
    pinnedPage->wroteBytes ();


    // create a bunch of pinned pages and remember them
    vector <MyDB_PageHandle> myHandles;
    for (int i = 1; i < 10; i++) {
        cout << "allocating pinned page\n";
        MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
        char *bytes = (char *) temp->getBytes ();
        writeNums (bytes, 64, i);
        temp->wroteBytes ();
        myHandles.push_back (temp);
    }

    // now forget the pages we created
    vector <MyDB_PageHandle> temp;
    myHandles = temp;

    // now remember 8 more pages
    for (int i = 0; i < 8; i++) {
        cout << "allocating pinned page\n";
        MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
        char *bytes = (char *) temp->getBytes ();

        // write numbers at the 0th position
        if (i == 0)
            writeNums (bytes, 64, i);
        else
            writeSymbols (bytes, 64, i);
        temp->wroteBytes ();
        myHandles.push_back (temp);
    }

    // now correctly write nums at the 0th position
    cout << "allocating unpinned page\n";
    MyDB_PageHandle anotherDude = myMgr.getPage (table1, 0);
    bytes = (char *) anotherDude->getBytes ();
    writeSymbols (bytes, 64, 0);
    anotherDude->wroteBytes ();

    // now do 90 more pages, for which we forget the handle immediately
    for (int i = 10; i < 100; i++) {
        cout << "allocating unpinned page\n";
        MyDB_PageHandle temp = myMgr.getPage (table1, i);
        char *bytes = (char *) temp->getBytes ();
        writeNums (bytes, 64, i);
        temp->wroteBytes ();
    }

    // now forget all of the pinned pages we were remembering
    vector <MyDB_PageHandle> temp2;
    myHandles = temp2;

    // now get a pair of pages and write them
    for (int i = 0; i < 100; i++) {
        cout << "allocating pinned page\n";
        MyDB_PageHandle oneHandle = myMgr.getPinnedPage ();
        char *bytes = (char *) oneHandle->getBytes ();
        writeNums (bytes, 64, i);
        oneHandle->wroteBytes ();
        cout << "allocating pinned page\n";
        MyDB_PageHandle twoHandle = myMgr.getPinnedPage ();
        writeNums (bytes, 64, i);
        twoHandle->wroteBytes ();
    }

    // make a second table
    MyDB_TablePtr table2 = make_shared <MyDB_Table> ("tempTable2", "barfoo");
    for (int i = 0; i < 100; i++) {
        cout << "allocating unpinned page\n";
        MyDB_PageHandle temp = myMgr.getPage (table2, i);
        char *bytes = (char *) temp->getBytes ();
        writeLetters (bytes, 64, i);
        temp->wroteBytes ();
    }
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
    size_t pageSize = 1;
    size_t pageNum = 4;
    MyDB_BufferManager manager(pageSize,pageNum,"file");
    MyDB_TablePtr table1 = make_shared<MyDB_Table>("table1","home");
    MyDB_TablePtr table2 = make_shared<MyDB_Table>("table2","home");

    MyDB_PageHandle handler1 = manager.getPinnedPage(table1,0);
    char* data1 = (char*)handler1->getBytes();
    *data1 = '0';
    handler1->wroteBytes();

    MyDB_PageHandle handler2 = manager.getPage(table1,1);
    char* data2 = (char*)handler2->getBytes();
    *data2 = '1';
    handler2->wroteBytes();
    manager.unpin(handler1);

    MyDB_PageHandle tempHandler = manager.getPage(table2,0);
    char* data3 = (char*)tempHandler->getBytes();
    *data3 = 'a';
    tempHandler->wroteBytes();
    tempHandler = manager.getPage(table1,2);
    tempHandler = manager.getPage(table2,2);
    tempHandler = manager.getPage(table2,3);
    char* data23 = (char*)tempHandler->getBytes();
    *data23 = '3';
    tempHandler->wroteBytes();

    char* byte =  (char*)handler1->getBytes();
//    exit(1);
    cout<<"table1 p1 ->"<< *byte <<endl;
    *byte = '2';
    handler1->wroteBytes();
//    exit(1);

    MyDB_PageHandle tempHandler1 = manager.getPinnedPage();
    MyDB_PageHandle tempHandler2 = manager.getPinnedPage();
    MyDB_PageHandle tempHandler3 = manager.getPinnedPage();
    handler1 = manager.getPinnedPage(table2,5);
    byte = (char*)handler1->getBytes();
    *byte = 'e';
    handler1->wroteBytes();
}

// these functions write a bunch of characters to a string... used to produce data
void writeNums (char *bytes, size_t len, int i) {
    for (size_t j = 0; j < len - 1; j++) {
        bytes[j] = '0' + (i % 10);
    }
    bytes[len - 1] = 0;
}

void writeLetters (char *bytes, size_t len, int i) {
    for (size_t j = 0; j < len - 1; j++) {
        bytes[j] = 'i' + (i % 10);
    }
    bytes[len - 1] = 0;
}

void writeSymbols (char *bytes, size_t len, int i) {
    for (size_t j = 0; j < len - 1; j++) {
        bytes[j] = '!' + (i % 10);
    }
    bytes[len - 1] = 0;
}