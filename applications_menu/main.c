//includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

//variables

int option = -1;
char sUrl[150];
pid_t pWebBrowser;
pid_t pTextEditor;
pid_t pTerminal;

//functions
void printMenu(void);
void handleSignals(void);
void initSigaction(struct sigaction* confSignal);
void processInput(int option);
void executeFork(void (*f)(), pid_t *pid);
void webBrowser(void);
void textEditor(void);
void terminal(void);
void finishApplication(int iApplication);
void quit(void);
char* putPidAndStatusInBaseString(char* sBase, pid_t pid);
char* getStatus(pid_t pProcesso);


int main(int argc, char* argv[]){
    for(;;){
        sleep(5);
        printMenu();
    }
    
}

void handleSignals(){
    struct sigaction confSignal;
    initSigaction(&confSignal);
    confSignal.sa_handler = &printMenu;
    
}

void printMenu(void){
    printf("<<<< Applications Menu >>>>\n");
    printf("%s", putPidAndStatusInBaseString("1) Web Browser", pWebBrowser));
    printf("%s", putPidAndStatusInBaseString("2) Text Editor", pTextEditor));
    printf("%s", putPidAndStatusInBaseString("3) Terminal", pTerminal));
    printf("4) Finalizar Processo\n");
    printf("5) Quit\n");
    printf("Option: \n");
    
    while( (scanf("%d", &option) != 1) || (option < 1) || (option > 5)){
        printf("Invalid Selection, please enter a valid option. \n");
        return;
    };
    
    processInput(option);
}

char* putPidAndStatusInBaseString(char *sBase, pid_t pProcesso){
    char sReturn[100];
    strcpy(sReturn, sBase);
    
    if ((pProcesso) > 0){
        
        char sPidAndStatus[50];
        
        sprintf(sPidAndStatus, "     (pid: %d, status: %s)", pProcesso, getStatus(pProcesso));
        
        strcat(sReturn, sPidAndStatus);
    }
    
    return strcat(sReturn, "\n");
}

char* getStatus(pid_t pProcesso){
    
    int iExitStatus;
    pid_t pStatus = waitpid(pProcesso, &iExitStatus, WNOHANG);
    
    if (pStatus == 0)
        return "executing";
    
    if (pStatus == pProcesso){
        if(WIFEXITED(iExitStatus)){
            if (WEXITSTATUS(iExitStatus))
                return "failed";
            return "finished";
        }
        return "aborted";
    }
    
    return "check status failed";
    
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
            int iApplication;
            scanf("%d", &iApplication);
            finishApplication(iApplication);
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
    //pid_t pidteste;
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

void finishApplication(int iApplication){
    switch (iApplication) {
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
    }
}

void initSigaction(struct sigaction* confSignal) {
    memset(confSignal, 0, sizeof(struct sigaction));
    confSignal->sa_flags = 0;
    sigemptyset(&confSignal->sa_mask);
    confSignal->sa_handler = NULL;
}
