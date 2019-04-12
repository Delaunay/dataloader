#include "ffilesystem.h"

#ifdef USE_CPP
namespace kw{

bool is_directory(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::is_directory(entry.status());
#else
    return entry.is_directory();
#endif
}

bool is_regular_file(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::is_regular_file(entry.status());
#else
    return entry.is_regular_file();
#endif
}

std::size_t file_size(FS_NAMESPACE::directory_entry const& entry){
#ifdef USE_BOOST_FS
    return FS_NAMESPACE::file_size(entry.path());
#else
    return entry.file_size();
#endif
}

}
#else

#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

#include "utils.h"


namespace cfs{
void directory_iterator_rec(path p, std::vector<Entry>& files, int, char* str);


std::vector<Entry> directory_iterator(path p){
    std::vector<Entry> files;
    char name[FILENAME_MAX];

    strcpy(name, p);
    directory_iterator_rec(p, files, 0, name);
    return files;
}

void directory_iterator_rec(path p, std::vector<Entry>& files, int depth, char* str){
    struct dirent *dent;

    struct stat st;
    lstat(p, &st);

    DIR* dir = opendir(p);
    if (!dir){
        if (depth == 0){
            DLOG("Not a directory returning");
            return;
        }
        files.emplace_back(p, true, std::size_t(st.st_size));
        return;
    } else if (depth != 0){
        files.emplace_back(p, false, 0);
    }

    size_t len = strlen(p);
    str[len++] = '/';

    while((dent = readdir(dir))){
        size_t n = strlen(dent->d_name);
        if (n == 1 && dent->d_name[0] == '.')
            continue;

        if (n == 2 && (dent->d_name[0] == '.' && dent->d_name[1] == '.'))
            continue;

        strncpy(str + len, dent->d_name, FILENAME_MAX - len);
        directory_iterator_rec(str, files, depth + 1, str);
    }
    closedir(dir);
}
}
#endif







