
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <memory>
#include <iostream>

using namespace std;



MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long idx) {
	//try to get page from the buffer pool
	CachePagePtr page = lru->getPage(table,idx);
	MyDB_PageHandle handler = make_shared<MyDB_PageHandleBase>(lru,page,table,idx);
	return handler;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile):
	lru (new LRU_Cache(pageSize, numPages)),
	tempFile(move(tempFile))
	{}

string MyDB_BufferManager :: getTempFileName(){
	return tempFile;
}

void MyDB_BufferManager :: freePage(CachePagePtr pCachePage){
	lru->freePage(pCachePage);
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	cout<<"buffer manager destroy"<<endl;
	delete lru;
}
	
#endif


