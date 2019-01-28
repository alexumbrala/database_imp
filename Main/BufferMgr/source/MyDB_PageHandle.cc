
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

MyDB_PageHandleBase :: MyDB_PageHandleBase(LRU_Cache *lru, CachePagePtr page, MyDB_TablePtr table, long idx, bool anonymous):
		lru(lru),
        handlerPage(std::move(page)),
        pTable(table),
        idx(idx),
        anonymous(anonymous),
        pageName(assemblePageName(table,idx)){
	handlerPage->handlerCount++;
}

void *MyDB_PageHandleBase :: getBytes () {
	CachePagePtr cachePage = lru->getPage(pTable,idx, handlerPage->pinned);
	//check if the page exists in lru buffer
	if(cachePage != handlerPage){
		handlerPage->handlerCount--;
		handlerPage = cachePage;
		handlerPage->handlerCount++;
	}
	if(!handlerPage->pinned){
		lru->visitPage(handlerPage);
	}
	return handlerPage->page->getData();
}

void MyDB_PageHandleBase :: wroteBytes () {
	handlerPage->page->setDataDirty();
}

void MyDB_PageHandleBase ::unPin() {
	lru->unpin(handlerPage);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	// if no more handler to this page, we ask the buffer manager to make the page available for reusing
	cout<<"page handler destroy"<<endl;
	// if the num of handler to the page is 0, and this page is anonymous
	// then we free this page's memory
	handlerPage->handlerCount--;
	if(handlerPage->handlerCount==0){
		if(handlerPage->pinned){
			lru->unpin(handlerPage);
		}
		if(anonymous){
			lru->makeTempPageAvailable(handlerPage);
		}
	}
}

#endif

