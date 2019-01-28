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
    unordered_map<string, CachePagePtr> pinnedCache;
    char* buffer;

    void moveToHead(CachePagePtr pCachePage);
    // move the page to cache buffer's tail;
    void moveToTail(CachePagePtr pCachePage);
public:

    explicit LRU_Cache(size_t pageSize, size_t pageNum);

    CachePagePtr getPage(MyDB_TablePtr table, long idx, bool pinned);

    void visitPage(CachePagePtr pCachePage);

    void unpin(CachePagePtr pCachePage);

    void makeTempPageAvailable(CachePagePtr pCachePage);

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
    bool pinned;
    CachePage():
        name(""),
        page(nullptr),
        next(nullptr),
        handlerCount(0)
    {}

    ~CachePage();

} ;
#endif