#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fnmatch.h>

int main(int argc, char **argv) {
    int i;
    DIR *dir;
    struct dirent *d;

    for (i = 1; i < argc; i++) {
        printf("%d .. %s\n", i, argv[i]);
        dir = opendir(".");
        while ((d = readdir(dir))) {
            if (fnmatch(argv[i], d->d_name, 0) == 0)
                printf("%s == %s\n", d->d_name, argv[i]);
        }
        closedir(dir);
    }

    return EXIT_SUCCESS;
}
