#include <vector>

#include <iostream>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
// int procB(char **argv)
//{

//    argv[0] = (char *)"./ece650-a1.py";
//    argv[1] = nullptr;
//    argv[2] = nullptr;

//    execvp("./ece650-a1.py", argv);

//    return 0;
//}
// int procA(int argc, char **argv)
//{
//    argv[0] = (char *)"regen";
//    execvp("./regen", argv);
//    return 0;
//}
int main(int argc, char **argv)
{
    int pipe1[2]; // regen to a1;
    pipe(pipe1);
    std::vector<pid_t> kids;
    pid_t child_pid;
    child_pid = fork();
    if (child_pid == 0)
    {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        argv[0] = (char *)"regen";
        execvp("./regen", argv);
        // std::cout << "add \"fzri\" (-12,4) (-14,-13) (7,14) (5,-9) (-7,6)" << std::endl;
        // std::cout << "add \"yezo\" (6,-14) (7,-15) (14,-5) (-19,-10) (20,-5)" << std::endl;
        // std::cout << "gg"
        //           << "\n";
        // exit(0);
        // return procA(argc, argv);
    }
    kids.push_back(child_pid);
    pid_t child_pid2 = fork();
    if (child_pid2 == 0)
    {
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        // return procB(argv);
        // execl("./ece650-a1.py", "./ece650-a1.py", NULL);
        while (!std::cin.eof())
        {
            std::string input_line;
            std::getline(std::cin, input_line);
            std::cout << input_line << std::endl;
        }
    }
    kids.push_back(child_pid2);
    close(pipe1[0]);
    close(pipe1[1]);
    sleep(30);
    for (pid_t k : kids)
    {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }
    return 0;
}