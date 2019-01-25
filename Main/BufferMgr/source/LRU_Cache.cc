#include "LRU_Cache.h"
using std::make_shared;


void breakFromList(CachePagePtr pCachePage){
    pCachePage->prev.lock()->next = pCachePage->next;
    pCachePage->next->prev.lock() = pCachePage->prev.lock();
    pCachePage->prev.lock() = nullptr;
    pCachePage->next = nullptr;
}


CachePage::~CachePage() {
    cout<<"destroy cachePage : "<<name<<endl;
}

/**
 * LRU_Cache implementation
 */
LRU_Cache::LRU_Cache(size_t pageSize, size_t pageNum):
    pageSize(pageSize),
    buffer((char*)malloc(sizeof(char)*pageNum*pageSize)){
        head = make_shared<CachePage>();
        head->page = make_shared<Page<char>>(pageSize,' ', nullptr);
        head->name = "head";
        CachePagePtr cur = head;
        for(int i=0; i<pageNum; i++){
            CachePagePtr pCachePage = make_shared<CachePage>();
            pCachePage->page = make_shared<Page<char>>(pageSize,' ',buffer+(i*pageNum));
            pCachePage->prev = cur;
            cur->next = pCachePage;
            cur = pCachePage;
        }
        tail = make_shared<CachePage>();
        tail->name = "tail";
        tail->page = make_shared<Page<char>>(pageSize,' ',nullptr);
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

CachePagePtr LRU_Cache::getPage(MyDB_TablePtr table, long idx, bool pinned){
    string name = assemblePageName(table,idx);

    // if the given table page exists in normal cache, we return the cached page
    if(cache.find(name)!=cache.end()){
        CachePagePtr pCachePage = cache[name];
        if (pinned){
            breakFromList(pCachePage);
            cache.erase(name);
            pinnedCache[name]=pCachePage;
        }
        return pCachePage;
    }
    // if the given table page exists in pinned cache, we return the cached page
    else if(pinnedCache.find(name)!=pinnedCache.end()){
        return pinnedCache[name];
    }
    // if the given table page doesn't exist in buffer pool, we will create a page
    // and store the page into hash map as well as insert it to the tail of the link list.
    if (head->next == tail){
        // if there is no more slot for new page, we throw an error
        /**
         * todo: maybe we should throw an error since there is no more room for new page
         */
        std::cerr<<"Buffer pool of page is full, there is no room for new page"<<endl;
        return nullptr;
    }

    //we evict the page next to head which is the least used among pages in the buffer pool;
    CachePagePtr leastPage = head->next;
    breakFromList(leastPage);
    cache.erase(leastPage->name);
    // flush the content of the page since we are reusing its buffer
    leastPage->page->writeToFile();

    // we insert a new page to the tail
    PagePtr page = make_shared<Page<char>>(pageSize,' ', leastPage->page->getData());
    page->readFromTable(table,idx);
    CachePagePtr pCachePage = make_shared<CachePage>(name,page,0);
    pCachePage->prev = tail->prev;
    pCachePage->next = tail;
    tail->prev.lock()->next = pCachePage;
    tail->prev = pCachePage;
    // store the page into hash map
    cache[name] = pCachePage;
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

    