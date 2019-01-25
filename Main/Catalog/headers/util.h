//
// Created by zristiano on 1/22/19.
//
#ifndef MAIN_UTIL_H
#define MAIN_UTIL_H

#include <string>
#include "MyDB_Table.h"
using std::string;

inline string assemblePageName(MyDB_TablePtr const table, long idx){
    return table->getName()+"#"+table->getStorageLoc()+"@"+to_string(idx);
}


#endif //MAIN_UTIL_H
