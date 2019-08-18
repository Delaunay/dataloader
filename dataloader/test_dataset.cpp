#include "dataset.h"
#include <iostream>

int main(){
    const char* data_loc = "/media/setepenre/UserData/tmp/fake";

    ImageFolder dataset(data_loc, single_threaded_loader);

    for (auto& item : dataset.samples()){
        std::cout << std::get<0>(item) << " " << std::get<1>(item) << " " << std::get<2>(item) << std::endl;
    }

    auto item = dataset.get_item(0);

    std::cout << std::get<0>(item).size() << std::endl;

    return 0;
}
