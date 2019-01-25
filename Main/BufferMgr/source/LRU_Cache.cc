#include "LRU_Cache.h"
using std::make_shared;




void breakFromList(CachePagePtr pCachePage){
    pCachePage->prev.lock()->next = pCachePage->next;
    pCachePage->next->prev = pCachePage->prev;
    pCachePage->prev.lock() = nullptr;
    pCachePage->next = nullptr;
}


CachePage::~CachePage() {
    cout<<"destroy cachePage : "<<name<<endl;
}

/**
 * LRU_Cache implementation
 */
LRU_Cache::LRU_Cache(size_t pageSize, size_t pageNum)
    :pageSize(pageSize){
        head = make_shared<CachePage>();
        head->page = make_shared<Page<char>>(pageSize,' ');
        head->name = "head";
        CachePagePtr cur = head;
        for(int i=0; i<pageNum; i++){
            CachePagePtr pCachePage = make_shared<CachePage>();
            pCachePage->page = make_shared<Page<char>>(pageSize,' ');
            pCachePage->prev = cur;
            cur->next = pCachePage;
            cur = pCachePage;
        }
        tail = make_shared<CachePage>();
        tail->name = "tail";
        tail->page = make_shared<Page<char>>(pageSize,' ');
        tail->prev = cur;
        cur->next = tail;
}

LRU_Cache::~LRU_Cache(){
    std::cout<<"LRU_Cache destroy"<<std::endl;
}

PagePtr LRU_Cache::getPage(string name){
    if(cache.find(name)==cache.end()){
        return nullptr;
    }
    CachePagePtr pCachePage = cache[name];
    // move the page visited to cache's tail;
    moveToTail(pCachePage);
    return pCachePage->page;

}

CachePagePtr LRU_Cache::getPage(MyDB_TablePtr table, long idx){
    string name = assemblePageName(table,idx);
    // if the given table page doesn't exist in buffer pool, we will create a page
    // and store the page into hash map as well as insert it to the tail of the link list.
    if(cache.find(name)==cache.end()){
        // if there is no more slot for new page, we throw an error
        if (head->next == tail){
            /**
             * todo: maybe we should throw an error since there is no more room for new page
             */
            std::cerr<<"Buffer pool of page is full, there is no room for new page"<<endl;
            return nullptr;
        }
        // we insert a new page to the tail
//        PagePtr page = make_shared<Page<char>>(pageSize);
        PagePtr page = make_shared<Page<char>>(pageSize,' ');
        page->readFromTable(table,idx);
        CachePagePtr pCachePage = make_shared<CachePage>(name,page,0);
        pCachePage->prev = tail->prev;
        pCachePage->next = tail;
        tail->prev.lock()->next = pCachePage;
        tail->prev = pCachePage;
        // store the page into hash map
        cache[name] = pCachePage;

        //we evict the page next to head which is the least used among pages in the buffer pool;
        CachePagePtr leastPage = head->next;
        breakFromList(leastPage);
        cache.erase(leastPage->name);
        // flush the content of the page we are reusing
        leastPage->page->writeToFile();
        return pCachePage;
    }
    // if the given table page exists in buffer pool, we return the cached page
    CachePagePtr pCachePage = cache[name];
//    moveToTail(pCachePage);
    return pCachePage;
}

void LRU_Cache::visitPage(CachePagePtr pCachePage) {
    // move the page visited to cache's tail;
    moveToTail(pCachePage);
}

void LRU_Cache::setPage(string name, PagePtr page){
    if(cache.find(name)==cache.end()){
        CachePagePtr pCachePage = head->next;
        cache.erase(pCachePage->name);
        pCachePage->name = name;
        moveToTail(pCachePage);
        pCachePage->page = page;
        cache[name]=pCachePage;
    }else{
        CachePagePtr pCachePage = cache[name];
        pCachePage->page = page;
        moveToTail(pCachePage);
    }
}

void LRU_Cache::moveToTail(CachePagePtr pCachePage){

    breakFromList(pCachePage);

    pCachePage->prev = tail->prev;
    pCachePage->next = tail;
    
    tail->prev.lock()->next = pCachePage;
    tail->prev = pCachePage;
}

void LRU_Cache::moveToHead(CachePagePtr pCachePage) {

    breakFromList(pCachePage);

    pCachePage->prev = head;
    pCachePage->next = head->next;

    head->next->prev=pCachePage;
    head->next = pCachePage;
}



void LRU_Cache::freePage(CachePagePtr pCachePage) {
    moveToHead(pCachePage);
}

/**
 * LRU_Cache implementation
 */

    