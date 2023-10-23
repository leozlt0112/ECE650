#include <iostream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
bool genrate_random_num(int *rand_num, int max, int min, std::string failed)
{
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail())
    {
        failed = "Error: cannot open /dev/urandom";
        return false;
    }
    unsigned int num;
    urandom.read(reinterpret_cast<char *>(&num), sizeof(int));
    std::cout << num << "\n";
    num = min + (num % ((max) - (min) + 1));
    *rand_num = num;
    return true;
}
int main(int argc, char **argv)
{
    // expected options are '-a', '-b', and '-c value'
    std::string svalue;
    int sint_value = 10;
    std::string lvalue;
    int lint_value = 5;
    std::string nvalue;
    int nint_value = 5;
    std::string cvalue;
    int cint_value = 20;
    int c;
    while ((c = getopt(argc, argv, "s:l:n:c:")) != -1)
    {
        switch (c)
        {
        case 's':
            svalue = optarg;
            sint_value = atoi(svalue.c_str());
            break;
        case 'l':
            lvalue = optarg;
            lint_value = atoi(lvalue.c_str());
            break;
        case 'n':
            nvalue = optarg;
            nint_value = atoi(nvalue.c_str());
            break;
        case 'c':
            cvalue = optarg;
            cint_value = atoi(cvalue.c_str());
            break;
        case '?':
            if (optopt == 'c')
            {
                cint_value = 20;
                std::cout << "Unknown " << cint_value << "\n";
                break;
            }
            else if (optopt == 's')
            {
                sint_value = 10;
                std::cout << "Unknown " << sint_value << "\n";
                break;
            }
            else if (optopt == 'l')
            {
                lint_value = 5;
                std::cout << "Unknown " << lint_value << "\n";
                break;
            }
            else if (optopt == 'n')
            {
                nint_value = 5;
                std::cout << "Unknown " << nint_value << "\n";
                break;
            }
        }
    }
    std::cout << sint_value << " " << lint_value << " " << cint_value << " " << nint_value << " " << std::endl;
    return 0;
}
