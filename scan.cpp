#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

// Recursive method that traverses a directory and displays files inside using dfs
int dir_scan(const char* dir_name) {
    int rc = 0;
    char ACL_string[30];
    DIR *d, *d2;
    struct dirent *dir, *dir2;
    struct stat buf;
    time_t ts;
    char next_dir[100];
    next_dir[0] = '\0';

    d = opendir(dir_name);
    if (d) {
        printf("%s\n", dir_name);
        while ((dir = readdir(d)) != NULL) {
            strcpy(next_dir, dir_name);
            strcat(next_dir, "/");
            strcat(next_dir, dir->d_name);

            if (stat(next_dir, &buf) != 0) {
                printf("Warning: File %s may not exist\n", next_dir);
            } else {
                ACL_string[0] = '\0';
                if (S_ISREG(buf.st_mode)) {
                    strcat(ACL_string, "-");
                } else if (S_ISDIR(buf.st_mode)) {
                    strcat(ACL_string, "d");
                } else if (S_ISLNK(buf.st_mode)) {
                    strcat(ACL_string, "l");
                } else if (S_ISCHR(buf.st_mode)) {
                    strcat(ACL_string, "c");
                } else if (S_ISBLK(buf.st_mode)) {
                    strcat(ACL_string, "b");
                } else if (S_ISFIFO(buf.st_mode)) {
                    strcat(ACL_string, "p");
                } else if (S_ISSOCK(buf.st_mode)) {
                    strcat(ACL_string, "s");
                }

                strcat(ACL_string, (buf.st_mode & S_IRUSR) ? "r" : "-");
                strcat(ACL_string, (buf.st_mode & S_IWUSR) ? "w" : "-");
                strcat(ACL_string, (buf.st_mode & S_IXUSR) ? "x" : "-");

                strcat(ACL_string, (buf.st_mode & S_IRGRP) ? "r" : "-");
                strcat(ACL_string, (buf.st_mode & S_IWGRP) ? "w" : "-");
                strcat(ACL_string, (buf.st_mode & S_IXGRP) ? "x" : "-");

                strcat(ACL_string, (buf.st_mode & S_IROTH) ? "r" : "-");
                strcat(ACL_string, (buf.st_mode & S_IWOTH) ? "w" : "-");
                strcat(ACL_string, (buf.st_mode & S_IXOTH) ? "x" : "-");

                printf("%s  %16lld %32s %s", ACL_string, buf.st_size, dir->d_name, ctime(&buf.st_ctime));
            }
        }
        rc = closedir(d);
        printf("\n");
    }

    d2 = opendir(dir_name);
    if (d2) {
        while ((dir2 = readdir(d2)) != NULL) {
            strcpy(next_dir, dir_name);
            strcat(next_dir, "/");
            strcat(next_dir, dir2->d_name);

            if (stat(next_dir, &buf) == 0) {
                if (S_ISDIR(buf.st_mode)) {
                    if (strcmp(dir2->d_name, ".") != 0 && strcmp(dir2->d_name, "..") != 0) {
                        dir_scan(next_dir);
                    }
                }
            }
        }
        rc = closedir(d2);
    }

    return rc;
}

int main(int argc, char** argv) {
    int rc = 0;
    char dir_name[20];

    if (argc != 2) {
        printf("Usage: %s directory_name\n", argv[0]);
        return 1;
    }

    strcpy(dir_name, argv[1]);

    rc = dir_scan(dir_name);

    return rc;
}
