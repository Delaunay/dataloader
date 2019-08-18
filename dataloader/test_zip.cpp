#include <cstdio>
#include <zip.h>

int main(){
    const char* file_name = "/media/setepenre/UserData/tmp/fake.zip";

    int error;
    zip_t* handle = zip_open(file_name, ZIP_RDONLY, &error);
    int entries = zip_get_num_entries(handle, ZIP_FL_UNCHANGED);

    for(int i = 0; i < entries; ++i){
        zip_stat_t stat;

        zip_stat_index(handle, i,  ZIP_FL_UNCHANGED, &stat);

        printf("    %s %d\n", stat.name, stat.size);

        zip_file_t* file_h = zip_fopen_index(handle, i, ZIP_FL_UNCHANGED);

        int read_bytes = zip_fread(file_h, buffer, nbytes);

        zip_fclose(file_h);
    }

    printf("entries: %d \n", entries);

    zip_close(handle);
    return 0;
}


// zip_file_t* file = zip_fopen(file, file_name, ZIP_FL_UNCHANGED);
