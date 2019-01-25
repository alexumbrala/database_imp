
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void MyDB_PageHandleBase :: init(){
	handlerPage->handlerCount++;
}

void *MyDB_PageHandleBase :: getBytes () {
	CachePagePtr cachePage = lru->getPage(pTable,idx);
	//check if the page exists in lru buffer
	if(cachePage != handlerPage){
		handlerPage->handlerCount--;
		handlerPage = cachePage;
		handlerPage->handlerCount++;
	}
	lru->visitPage(handlerPage);
	return handlerPage->page->getData();
}

void MyDB_PageHandleBase :: wroteBytes () {
	handlerPage->page->setDataDirty();
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	// if no more handler to this page, we ask the buffer manager to make the page available for reusing
	cout<<"page handler destroy"<<endl;
	// if the num of handler to the page is 0, and this page is anonymous
	// then we free this page's memory
	handlerPage->handlerCount--;
	if(false/* anonymous && handlerCount==0 */){
		lru->freePage(handlerPage);
        handlerPage->page->resetDirtyFlag();
	}
}

#endif

