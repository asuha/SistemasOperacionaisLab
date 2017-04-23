//includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

//variables
char sUrl[150];
int iApplicationToFinish;
char* sWebBrowser;
char* sTextEditor;
char* sTerminal;
char* sFinishApplication;
pid_t pWebBrowser;
pid_t pTextEditor;
pid_t pTerminal;
pid_t pFinishApplication;

//functions
void printMenu(void);
void handleSignals(int signum);
void prepareSignals(void);
void initSigaction(struct sigaction*);
void processInput(int option);
void executeFork(void (*f)(), pid_t *pid);
void webBrowser(void);
void textEditor(void);
void terminal(void);
void finishApplication();
void quit(void);
char* putPidAndStatusInBaseString(char* sBase, pid_t pProcesso, char* sStatus);
void getStatus(pid_t pProcesso, char* sStatus);


int main(int argc, char* argv[]){
    prepareSignals();
    
    for(;;){
        sleep(5);
        printMenu();
    }
    
}

void prepareSignals(){
    struct sigaction confSigaction;
    initSigaction(&confSigaction);
    
    if (sigaction(SIGINT, &confSigaction, NULL) == -1){
        printf("%s","error preparing SIGINT");
        exit(EXIT_FAILURE);
    }
    
    if (sigaction(SIGCHLD, &confSigaction, NULL) == -1){
        printf("%s", "error preparing SIGCHLD");
        exit(EXIT_FAILURE);
    }
}

void handleSignals(int signum){
    printMenu();
}

void printMenu(void){
    printf("<<<< Applications Menu >>>>\n");
    printf("%s", putPidAndStatusInBaseString("1) Web Browser", pWebBrowser, sWebBrowser));
    printf("%s", putPidAndStatusInBaseString("2) Text Editor", pTextEditor, sTextEditor));
    printf("%s", putPidAndStatusInBaseString("3) Terminal", pTerminal, sTerminal));
    printf("%s", putPidAndStatusInBaseString("4) Finalizar Processo", pFinishApplication, sFinishApplication));
    printf("5) Quit\n");
    printf("Option: \n");
    
    int option;
    while( (scanf("%d", &option) != 1) || (option < 1) || (option > 5)){
        printf("Invalid Selection, please enter a valid option. \n");
        return;
    };
    
    processInput(option);
}

char* putPidAndStatusInBaseString(char *sBase, pid_t pProcesso, char* sStatus){
    char sReturn[100];
    strcpy(sReturn, sBase);
    
    if ((pProcesso) > 0){
        
        getStatus(pProcesso, sStatus);
        
        char sPidAndStatus[50];
    
        sprintf(sPidAndStatus, "     (pid: %d, status: %s)", pProcesso, sStatus);
        
        strcat(sReturn, sPidAndStatus);
    }
    
    return strcat(sReturn, "\n");
}

void getStatus(pid_t pProcesso, char* sStatus){
    
    int iExitStatus;
    pid_t pStatus = waitpid(pProcesso, &iExitStatus, WNOHANG);
    
    if (pStatus == 0)
        sStatus = "executing";
    
    if (pStatus == pProcesso){
        if(WIFEXITED(iExitStatus)){
            if (WEXITSTATUS(iExitStatus))
                sStatus = "failed";
            sStatus = "finished";
        }
        sStatus = "aborted";
    }
    
}

void processInput(int option){
    
    switch(option) {
        case 1:
            printf("%s", "Please inform the url you want to access: \n");
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
            printf("%s", "Please inform the application you want to finish: \n");
            scanf("%d", &iApplicationToFinish);
            executeFork(&finishApplication, &pFinishApplication);
            break;
        case 5:
            quit();
            break;
        default:
            printf("other option\n");
            exit(0);
            
    }
}

void executeFork(void (*f)(), pid_t *pid){
    switch((*pid)=fork()) {
        case -1:
            printf("falhou\n");
            exit(EXIT_FAILURE);
        case 0:
            printf("processo filho\n");
            f();
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
    printf("text Editor\n");
    
    execlp("gedit",
           "gedit",
           NULL);
    
    exit(EXIT_FAILURE);
    
}

void terminal(){
    printf("terminal\n");
    
    execlp("gnome-terminal",
           "terminal",
           NULL);
    
    exit(EXIT_FAILURE);
}

void quit(){
    exit(EXIT_SUCCESS);
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
            printf("%s", "Invalid Value");
            exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void initSigaction(struct sigaction* confSignal) {
    memset(confSignal, 0, sizeof(struct sigaction));
    confSignal->sa_flags = 0;
    sigemptyset(&confSignal->sa_mask);
    confSignal->sa_handler = &handleSignals;
}
