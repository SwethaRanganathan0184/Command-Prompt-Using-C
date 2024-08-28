#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024

// Define a structure for a simple linked list to manage command history
typedef struct CommandNode {
    char command[MAX_INPUT_SIZE];
    struct CommandNode* next;
} CommandNode;

CommandNode* history = NULL;

void read_command(char* input, int inputSize) {
    printf("$ ");
    fgets(input, inputSize, stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove newline character
}

void add_to_history(char* command) {
    CommandNode* newNode = (CommandNode*)malloc(sizeof(CommandNode));
    if (newNode == NULL) {
        perror("Memory allocation error");
        return;
    }
    strcpy(newNode->command, command);
    newNode->next = history;
    history = newNode;
}

void execute_command(char* input) {
    if (strlen(input) == 0) return;

    if (strncmp(input, "!", 1) == 0) {
        // Handle !n (rerun from history) command
        int index = atoi(input + 1); // Extract the number after "!"
        CommandNode* current = history;
        int count = 1;

        while (current != NULL) {
            if (count == index) {
                printf("Rerunning: %s\n", current->command); // Display the rerun command
                system(current->command); // Rerun the command
                return;
            }
            current = current->next;
            count++;
        }
        printf("Command not found in history\n");
    } else if (strncmp(input, "move", 4) == 0) {
        // Handle the "cd" command to change the current directory
        if (strcmp(input + 5, "..") == 0) {
            // Go to the parent directory
            if (chdir("..") == 0) {
                printf("Directory changed to the parent directory\n");
            } else {
                perror("move");
            }
        } else {
            if (chdir(input + 5) == 0) {
                printf("Directory changed to %s\n", input + 5);
            } else {
                perror("move");
            }
        }
    } else if (strcmp(input, "clear") == 0) {
        // Clear the terminal (Windows and Unix)
        #ifdef _WIN32
        system("cls"); // Windows
        #else
        system("clear"); // Unix-based systems (Linux)
        #endif
    } else {
        add_to_history(input); // Add the command to the history

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (strcmp(input, "ls") == 0) {
            // Implement custom "ls" command to list files and directories
            system("dir");
        } else if (strcmp(input, "dir") == 0) {
            // Implement custom "dir" command to list files and directories
            system("dir");
        } else if (strcmp(input, "exit") == 0) {
            exit(0);
        } else if (strcmp(input, "history") == 0) {
            // Display command history
            CommandNode* current = history;
            int index = 1;
            while (current != NULL) {
                printf("%d: %s\n", index, current->command);
                current = current->next;
                index++;
            }
        } else {
            if (!CreateProcess(NULL, input, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                DWORD error = GetLastError();
                LPVOID errorMessage;
                DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

                #ifdef _UNICODE
                // For wide character strings
                FormatMessageW(dwFlags, NULL, error, 0, (LPWSTR)&errorMessage, 0, NULL);
                #else
                // For narrow character strings
                FormatMessageA(dwFlags, NULL, error, 0, (LPSTR)&errorMessage, 0, NULL);
                #endif

                wprintf(L"CreateProcess failed with error %d: %s\n", error, errorMessage);

                LocalFree(errorMessage);
            } else {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        read_command(input, sizeof(input));
        execute_command(input);
    }

    // Clean up the command history
    CommandNode* current = history;
    while (current != NULL) {
        CommandNode* temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}
