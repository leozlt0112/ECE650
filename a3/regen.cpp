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
    int sint_value;
    std::string lvalue;
    int lint_value;
    int c;
    while ((c = getopt(argc, argv, "s:l:")) != -1)
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
        }
    std::cout << sint_value << lint_value << std::endl;
    return 0;
}
