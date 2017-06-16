//includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

//consts
const int ALARM_DEFAULT_TIMEOUT = 5;
const int TIME_TO_WRITE = 8;

//variables
int iSignalReceived;
int iApplicationToFinish;
char sUrl[150];
char* sWebBrowser;
char* sTextEditor;
char* sTerminal;
char* sFinishApplication;
pid_t pWebBrowser;
pid_t pTextEditor;
pid_t pTerminal;
pid_t pFinishApplication;

//functions
void executeFork(void (*functionToExecute)(), pid_t *pid);
void getStatus(pid_t pProcess, char** sStatus);
void getInput(int *iOption);
void printMenu();
void processInput(int iOption);
char* putPidAndStatusInBaseString(char* sBase, pid_t pProcess, char** sStatus);

//signals
void defineSignalHandlers();
void configureSigaction(struct sigaction*);
void handleSignals(int signum);

//options handler
void webBrowser();
void textEditor();
void terminal();
void finishApplication();
void quit();



int main(int argc, char* argv[]){
    defineSignalHandlers();
    
    for(;;){
        printMenu();
        
        //set alarm to update screen each 5 seconds
        alarm(ALARM_DEFAULT_TIMEOUT);
        
        int iOption;
        getInput(&iOption);
        
        processInput(iOption);
    }
    
}

void defineSignalHandlers(){
    struct sigaction sigactionConfig;
    configureSigaction(&sigactionConfig);
    
    if (sigaction(SIGINT, &sigactionConfig, NULL) == -1){
        printf("%s","error preparing SIGINT");
        exit(EXIT_FAILURE);
    }
    
    if (sigaction(SIGCHLD, &sigactionConfig, NULL) == -1){
        printf("%s", "error preparing SIGCHLD");
        exit(EXIT_FAILURE);
    }
    
    if (sigaction(SIGALRM, &sigactionConfig, NULL) == -1){
        printf("%s", "error preparing SIGALRM");
        exit(EXIT_FAILURE);
    }
}

void configureSigaction(struct sigaction* signalConfig) {
    memset(signalConfig, 0, sizeof(struct sigaction));
    signalConfig->sa_flags = 0;
    sigemptyset(&signalConfig->sa_mask);
    signalConfig->sa_handler = &handleSignals;
}

void handleSignals(int signum){
    //Defines that a signal was received
    iSignalReceived = 1;
}

void printMenu(void){
    printf("<<<< Applications Menu >>>>\n");
    printf("%s", putPidAndStatusInBaseString("1) Web Browser", pWebBrowser, &sWebBrowser));
    printf("%s", putPidAndStatusInBaseString("2) Text Editor", pTextEditor, &sTextEditor));
    printf("%s", putPidAndStatusInBaseString("3) Terminal", pTerminal, &sTerminal));
    printf("%s", putPidAndStatusInBaseString("4) Finalizar Processo", pFinishApplication, &sFinishApplication));
    printf("5) Quit\n");
    printf("Option: \n");
}

char* putPidAndStatusInBaseString(char *sBase, pid_t pProcess, char** sStatus){
    char sReturn[100];
    //Copy string to array with 100 characters to guarantee it will have space enough space to concatenate
    strcpy(sReturn, sBase);
    
    if (pProcess > 0){
        
        getStatus(pProcess, sStatus);
        
        char sPidAndStatus[50];
        
        sprintf(sPidAndStatus, "     (pid: %d, status: %s)", pProcess, *(sStatus));
        
        //Concatenate the received application option to its pid and status so it will have all its information
        strcat(sReturn, sPidAndStatus);
    }
    
    return strcat(sReturn, "\n");
}

void getStatus(pid_t pProcess, char** sStatus){
    int iExitStatus;
    //execute waitpid with WNOHANG to return immediately
    pid_t pStatus = waitpid(pProcess, &iExitStatus, WNOHANG);
    
    //pid 0 means the process still executing
    if (pStatus == 0)
        *(sStatus) = "executing";
    
    else
        //status pid eq to process pid means that the execution finished
        if (pStatus == pProcess){
            
            //Check if process finished normally
            if(WIFEXITED(iExitStatus)){
                
                //check if process finished normally with error status
                if (WEXITSTATUS(iExitStatus))
                    *(sStatus) = "failed";
                else
                    *(sStatus) = "finished";
                
            } else
                //termination was not normal
                *(sStatus) = "aborted";
        }
    
}

void getInput(int *iOption){
    //if signal was received abort so menu is printed
    
    while( (scanf("%d", iOption) != 1) || (*(iOption) < 1) || (*(iOption) > 5)){
        //If it was aborted because of a signal do not print invalid selection and just return
        if (iSignalReceived != 1)
            printf("%s \n","Invalid Selection, please enter a valid option.");
        
        return;
    };
}

void processInput(iOption){
    //if signal was received abort and return so menu is printed
    if (iSignalReceived == 1){
        iSignalReceived = 0;
        return;
    }
    
    switch(iOption) {
        case 1:
            
            //increase alarm time to write URL
            alarm(TIME_TO_WRITE);
            
            printf("%s \n", "Please inform the url you want to access: ");
            scanf(" %149[^\n]s", sUrl);
            
            executeFork(&webBrowser, &pWebBrowser);
            break;
        case 2:
            executeFork(&textEditor, &pTextEditor);
            break;
        case 3:
            executeFork(&terminal, &pTerminal);
            break;
        case 4:
            //increase alarm time to choose which application to finish
            alarm(TIME_TO_WRITE);
            
            printf("%s \n", "Please inform the application you want to finish: ");
            scanf("%d", &iApplicationToFinish);
            
            executeFork(&finishApplication, &pFinishApplication);
            break;
        case 5:
            quit();
            break;
        default:
            printf("%s \n", "Invalid Option");
            
    }
}

void executeFork(void (*functionToExecute)(), pid_t *pid){
    switch((*pid)=fork()) {
        case -1:
            printf("%s \n","fork failed");
            exit(EXIT_FAILURE);
        case 0:
            functionToExecute();
            break;
        default:
            return;
    }
    
}

void webBrowser(){
    
    execlp("firefox",
           "firefox",
           sUrl,
           NULL);
    
    exit(EXIT_FAILURE);
}

void textEditor(){

    execlp("gedit",
           "gedit",
           NULL);
    
    exit(EXIT_FAILURE);
    
}

void terminal(){
    
    execlp("gnome-terminal",
           "terminal",
           NULL);
    
    exit(EXIT_FAILURE);
}

void finishApplication(){
    switch (iApplicationToFinish) {
        case 1:
            kill(pWebBrowser, SIGTERM);
            break;
            
        case 2:
            kill(pTextEditor, SIGTERM);
            break;
            
        case 3:
            kill(pTerminal, SIGTERM);
            break;
            
        default:
            printf("%s \n", "Invalid Value");
            exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void quit(){
    exit(EXIT_SUCCESS);
}
