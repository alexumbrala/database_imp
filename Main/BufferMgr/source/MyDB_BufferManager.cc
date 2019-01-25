
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <memory>
#include <iostream>
#include <stdio.h>

using namespace std;



MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long idx) {
	//try to get page from the buffer pool
	CachePagePtr page = lru->getPage(table,idx, false);
	MyDB_PageHandle handler = make_shared<MyDB_PageHandleBase>(lru,page,table,idx, false);
	return handler;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	CachePagePtr page = lru->getPage(tempTable,tempIdx, false);
	MyDB_PageHandle handler = make_shared<MyDB_PageHandleBase>(lru,page,tempTable,tempIdx, true);
	tempIdx++;
	return handler;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table, long idx) {
	CachePagePtr page = lru->getPage(table,idx, true);
	MyDB_PageHandle handler = make_shared<MyDB_PageHandleBase>(lru,page,table,idx, false);
	return handler;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	CachePagePtr page = lru->getPage(tempTable,tempIdx, true);
	MyDB_PageHandle handler = make_shared<MyDB_PageHandleBase>(lru,page,tempTable,tempIdx, true);
	tempIdx++;
	return handler;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	unpinMe->unPin();
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string temp):
	lru (new LRU_Cache(pageSize, numPages)),
	tempFile(move(temp)),
	tempIdx(0)
	{
		tempTable = make_shared<MyDB_Table>(tempFile,"");
	}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	cout<<"buffer manager destroy"<<endl;
	string fileName = tempTable->getStorageLoc()+"_"+tempTable->getName();
	remove(fileName.data());
	delete lru;
}
	
#endif


