#include "dataset.h"
#include <iostream>

int main(){
    const char* data_loc = "/media/setepenre/UserData/tmp/fake";

    ImageFolder dataset(data_loc, single_threaded_loader);

    for (auto& item : dataset.samples()){
        std::cout << item.name << " " << item.label<< " " << item.size << std::endl;
    }

    return 0;
}
