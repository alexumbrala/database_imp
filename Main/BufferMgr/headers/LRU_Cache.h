#ifndef LRU_CACHE_H
#define LRU_CACHE_H
#include "MyDB_Page.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "util.h"

using std::unordered_map;
using std::string;
using std::shared_ptr;
using std::move;

class CachePage;
class LRU_Cache;

typedef shared_ptr<CachePage> CachePagePtr;
typedef weak_ptr<CachePage> CachePageWeakPtr;

class LRU_Cache{

private:
    size_t pageSize;
    unordered_map<string, CachePagePtr> cache;
    CachePagePtr head;
    CachePagePtr tail;

    // move the page to cache buffer's tail;
    void moveToTail(CachePagePtr pCachePage);
    void moveToHead(CachePagePtr pCachePage);
public:

    explicit LRU_Cache(size_t pageSize, size_t pageNum);

    PagePtr getPage(string name);

    CachePagePtr getPage(MyDB_TablePtr table, long idx);

    void visitPage(CachePagePtr pCachePage);

    void setPage(string name, PagePtr page);

    void freePage(CachePagePtr pCachePage);

    ~LRU_Cache();

};



class CachePage{
public:
    string name;
    PagePtr page;
    // we must use weak_ptr to solve the cyclic reference problem
    CachePageWeakPtr prev;
    CachePagePtr next;
    size_t handlerCount;
    CachePage(string name = "", PagePtr page = nullptr, size_t handlerCount = 0):
                name(std::move(name)),
                page(std::move(page)),
                handlerCount(handlerCount)
    {}

    ~CachePage();

} ;
#endif