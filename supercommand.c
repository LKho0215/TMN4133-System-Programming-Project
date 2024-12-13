#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

void create_file(char *filename){
    int fd = open(filename, O_CREAT | O_RDWR);
    if (fd == -1){
        perror("Error in creating file\n");
        exit(1);
    }
    close(fd);
    printf("File named '%s' created successfully\n", filename);
}

void change_permission(char *filename, mode_t mode){
    if (chmod(filename, mode) == -1){
        perror("Error in changing permission\n");
        exit(1);
    }
    printf("Permission for %s changed successfully\n", filename);
}

void read_file(char *filename){
    char buffer[1024];
    int fd = open(filename, O_RDONLY);

    if (fd == -1){
        perror("Error in reading file\n");
        exit(1);
    }

    ssize_t readFile;
    while ((readFile = read(fd, buffer, sizeof(buffer))) > 0){
        buffer[readFile] = '\0';
        printf("%s", buffer);
    }

    close(fd);
}

void write_file (char *filename){
    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd == -1){
        perror("Error in writing file\n");
        exit(1);
    }

    char buffer[1024];
    printf("Stop writing by pressing 'Ctrl+D'\n");
    printf("Enter the content to write in file:\n ");
    while (fgets(buffer, sizeof(buffer), stdin)) {
        write(fd, buffer, strlen(buffer));
    }
    close(fd);
}

void delete_file(const char *filename){
    if (remove(filename) == -1){
        perror("Error in deleting file\n");
        exit(1);
    }
    printf("File named '%s' deleted successfully\n", filename);
}

void create_dir(char *dirname){
     if (mkdir(dirname, 0755) == -1) {
        perror("Error creating directory\n");
    } else {
        printf("Directory %s created successfully.\n", dirname);
    }
}

void delete_dir(char *dirname){
    if (rmdir(dirname) == -1) {
        perror("Error deleting directory\n");
    } else {
        printf("Directory %s deleted successfully.\n", dirname);
    }
}

void print_current_directory(){
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error");
    } else {
        printf("Current working directory: %s\n", cwd);
    }
}

void list_directory(char *dirname){
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(dirname))) {
        perror("Error opening directory\n");
        return;
    }else {
        printf("*****Current directory:[%s]*****\n", dirname);
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}
int main(int argc, char *argv[]){
	if (argc < 3){
        fprintf(stderr, "Please enter in correct format!\nFormat: %s -m [mode] [argument]\n", argv[0]);
        return 1;
	}

    int mode = atoi(argv[2]);
    int file_operation_mode = atoi(argv[3]);
    int directory_operation_mode = atoi(argv[3]);

    switch (mode){
        case 1:
            if (argc < 5){
                perror("Please enter correct format of arguments for file operation");
                break;
            }

            switch (file_operation_mode){
                case 1:
                    create_file(argv[4]);
                    break;

                case 2:
                    if (argc < 6){
                        perror("Please enter correct format of arguments for changing permission");
                        break;
                    }
                    mode_t new_mode = strtol(argv[5], NULL, 8);
                    change_permission(argv[4], new_mode);
                    break;

                case 3:
                    read_file(argv[4]);
                    break;

                case 4:
                    write_file(argv[4]);
                    break;

                case 5:
                    delete_file(argv[4]);
                    break;

                default:
                    perror("Invalid file operation\n");
            }
            break;

        case 2:
            if (argc < 4){
                perror("Please enter correct format of arguments for directory operation");
                return 1;
            }

            switch (directory_operation_mode){
                case 1:
                    create_dir(argv[4]);
                    break;

                case 2:
                    delete_dir(argv[4]);
                    break;

                case 3:
                    list_directory(argv[4]);
                    break;

                case 4:
                    print_current_directory();
                    break;

                default:
                    perror("Invalid directory operation\n");
            }
            break;


    }
}
