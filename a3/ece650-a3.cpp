#include <vector>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
pid_t child_pid = -1;
pid_t child_pid2 = -1;
pid_t child_pid3 = -1;
void sigchld_handler(int signo)
{
    pid_t pid;
    int status;
    // Check which child process has exited
    if ((pid = waitpid(child_pid, &status, WNOHANG)) > 0)
    {
        if (child_pid2 != -1)
            kill(child_pid2, SIGTERM); // Send SIGTERM to the other child process
        if (child_pid3 != -1)
            kill(child_pid3, SIGTERM);
    }
    else if ((pid = waitpid(child_pid2, &status, WNOHANG)) > 0)
    {
        if (child_pid3 != -1)
            kill(child_pid3, SIGTERM); // Send SIGTERM to the other child process
        if (child_pid != -1)
            kill(child_pid, SIGTERM);
    }
    else if ((pid = waitpid(child_pid3, &status, WNOHANG)) > 0)
    {
        if (child_pid2 != -1)
            kill(child_pid2, SIGTERM); // Send SIGTERM to the other child process
        if (child_pid != -1)
            kill(child_pid, SIGTERM);
    }
    exit(1);
}
int main(int argc, char **argv)
{
    // Install the SIGCHLD handler https:https://en.wikipedia.org/wiki/Sigaction
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1)
    {
        perror(0);
        exit(1);
    }
    int pipe1[2]; // regen to a1;

    int bigpipe[2]; // driver to c++
    //                 // python to c++
    pipe(pipe1);
    pipe(bigpipe);
    std::vector<pid_t> kids;
    child_pid = fork();
    // regen pipe
    if (child_pid == 0)
    {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(bigpipe[0]);
        close(bigpipe[1]);
        argv[0] = (char *)"./regen";
        execv("./regen", argv);
    }
    kids.push_back(child_pid);
    child_pid2 = fork();
    // child 1 python
    if (child_pid2 == 0)
    {
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(bigpipe[0]);
        dup2(bigpipe[1], STDOUT_FILENO);
        close(bigpipe[1]);
        execlp("/usr/bin/python3", "python3", "ece650-a1.py", NULL);
    }
    kids.push_back(child_pid2);
    child_pid3 = fork();
    // python to c++
    if (child_pid3 == 0)
    {

        dup2(bigpipe[0], STDIN_FILENO);
        close(bigpipe[0]);
        close(pipe1[0]);
        close(bigpipe[1]);
        close(pipe1[1]);
        execl("./ece650-a2", "./ece650-a2", NULL);
    }
    kids.push_back(child_pid3);
    close(pipe1[0]);
    close(pipe1[1]);
    close(bigpipe[0]);
    dup2(bigpipe[1], STDOUT_FILENO);
    close(bigpipe[1]);
    std::string input_line;
    while (std::getline(std::cin, input_line))
    {
        std::cout << input_line << std::endl;
    }
    for (pid_t k : kids)
    {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }
    return 0;
}