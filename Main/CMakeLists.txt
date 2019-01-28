cmake_minimum_required(VERSION 3.13)
project(Main)

set(CMAKE_CXX_STANDARD 14)

include_directories(BufferMgr/headers)
include_directories(Catalog/headers)
include_directories(Qunit)
include_directories(Qunit/headers)

#add_executable(Main
#        BufferMgr/headers/LRU_Cache.h
#        BufferMgr/headers/MyDB_BufferManager.h
#        BufferMgr/headers/MyDB_PageHandle.h
#        BufferMgr/source/LRU_Cache.cc
#        BufferMgr/source/MyDB_BufferManager.cc
#        BufferMgr/source/MyDB_PageHandle.cc
#        BufferMgr/headers/MyDB_Page.h
#        Catalog/headers/MyDB_Table.h
#        Catalog/source/MyDB_Table.cc
#        Qunit/headers/QUnit.h
#        main.cc Catalog/headers/util.h)

add_executable(Main
        BufferMgr/headers/LRU_Cache.h
        BufferMgr/headers/MyDB_BufferManager.h
        BufferMgr/headers/MyDB_PageHandle.h
        BufferMgr/source/LRU_Cache.cc
        BufferMgr/source/MyDB_BufferManager.cc
        BufferMgr/source/MyDB_PageHandle.cc
        BufferMgr/headers/MyDB_Page.h
        Catalog/headers/MyDB_Table.h
        Catalog/source/MyDB_Table.cc
        Qunit/headers/QUnit.h
        BufferTest/source/BufferQUnit.cc Catalog/headers/util.h)