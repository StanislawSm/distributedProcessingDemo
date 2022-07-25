#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>
//#include <processthreadsapi.h>

#define COUNTING_BORDER = 1000

int threadsNumber = 0;
HANDLE threadsHandlesArray[100];

DWORD WINAPI threadFunction(void* argumenty);
struct threadData {
    char* name;
    int parameter;
    HANDLE handle;
};

HANDLE threadsHandlesArray[100];
struct threadData threadData;
int pause = 1;


int main(int argc, char** argv) {
    printf("process pid: %d, started, priority: %ld\n", GetCurrentProcessId(), GetPriorityClass(GetCurrentProcess()));
    
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char inputSign;
    int inputNumber;
    char* codePath = "C:\\Users\\User\\Documents\\sem4\\PR\\lab4\\lab4visual\\main.exe";
    int priority;
    int processesNumber = 0;
    HANDLE thread;
    DWORD threadId;



    scanf("%c", &inputSign);
    while (inputSign != 'q') {
        if (inputSign == 'p') {
            scanf("%d", &inputNumber);
            switch (inputNumber) {
            case 0:
                priority = REALTIME_PRIORITY_CLASS;
                break;
            case 1:
                priority = HIGH_PRIORITY_CLASS;
                break;
            case 2:
                priority = BELOW_NORMAL_PRIORITY_CLASS;
                break;
            case 3:
                priority = IDLE_PRIORITY_CLASS;
                break;
            default:
                priority = NORMAL_PRIORITY_CLASS;
                break;
            }
            
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));

            // Start the child process. 
            if (!CreateProcess(
                L"C:\\Users\\User\\Documents\\sem4\\PR\\lab4\\lab4visual\\debug\\lab4visual.exe",   // No module name 
                NULL,               // Command line
                NULL,               // Process handle not inheritable
                NULL,               // Thread handle not inheritable
                FALSE,              // Set handle inheritance to FALSE
                CREATE_NEW_CONSOLE, // creation flags
                NULL,               // Use parent's environment block
                NULL,               // Use parent's starting directory 
                &si,                // Pointer to STARTUPINFO structure
                &pi)                // Pointer to PROCESS_INFORMATION structure
                )
            {
                printf("CreateProcess failed (%d).\n", GetLastError());
                return 0;
            }
                
            SetPriorityClass(pi.hProcess, priority);

        } else if (inputSign == 'i') {
            printf("pid: %d\n", GetCurrentProcessId());
        } else if (inputSign == 's') {
            pause = pause * (-1);
            printf("stop clicked\n");
        }
        else if (inputSign == 't') {
            scanf("%d", &inputNumber);
            switch (inputNumber) {
            case 0:
                priority = THREAD_PRIORITY_TIME_CRITICAL;
                break;
            case 1:
                priority = THREAD_PRIORITY_ABOVE_NORMAL;
                break;
            case 2:
                priority = THREAD_PRIORITY_BELOW_NORMAL;
                break;
            case 3:
                priority = THREAD_PRIORITY_LOWEST;
                break;
            default:
                priority = THREAD_PRIORITY_NORMAL;
                break;
            }
            
            thread = CreateThread(
                NULL,           // security                
                0,              // stack size
                threadFunction, // thread function
                (void*)&threadData, // thread function data
                0,              // flags
                &threadId);
            threadsHandlesArray[threadsNumber] = thread;
            if (threadsHandlesArray[threadsNumber] != INVALID_HANDLE_VALUE) {
                SetThreadPriority(threadsHandlesArray[threadsNumber], priority);
                threadsHandlesArray[threadsNumber] = thread;
                printf("thread careated, tip: %d, priority: %ld\n", GetThreadId(thread), GetThreadPriority(thread));
                threadsNumber++;
            } else {
                printf("thread creation failed");
            }
        
        } else if (inputSign == 'x') {
            //close last opened thread
            HANDLE threadToClose = threadsHandlesArray[threadsNumber - 1];
            if (threadToClose != INVALID_HANDLE_VALUE && threadToClose != 0) {
                printf("thread tid: %d is being closed\n", GetThreadId(threadToClose));
                TerminateThread(threadToClose, 0);
                threadsHandlesArray[threadsNumber] = NULL;
                threadsNumber--;
            } else {
                printf("closing thread tid: %d failed\n", GetThreadId(threadToClose));
            }
        }
        scanf("%c", &inputSign);
    }
    printf("process: %d has ended\n", GetCurrentProcessId());
    return 0;
}

DWORD WINAPI threadFunction(void *args) {
    int iterator = 0;
    while (1 > 0) {
        if (pause == 1) {
            time_t start = time(NULL);
            iterator = 0;
            while (iterator < 1000000000) {
                iterator++;
            }
            time_t now = time(NULL);
            double time_taken = ((double)(now - start));
            if (pause == 1) {
                printf("pid: %d, tid: %d, time: %f\n", GetCurrentProcessId(), GetCurrentThreadId(), time_taken);
            }
        }
    }
}

