#include "ffilesystem.h"
#include <iostream>

int main(){

    auto vals = cfs::directory_iterator("/home/user1/test_database/train");

    for(auto& item: vals){
        std::cout << item.name << " " << item.is_file  << " " << item.size << std::endl;
    }

    return 0;
}
