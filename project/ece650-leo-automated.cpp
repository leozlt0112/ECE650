#include <iostream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <tuple>
#include <list>
#include <climits>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include <fstream>
// Include the MiniSat headers if you have MiniSat installed and properly configured.
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <memory>
#include <unordered_map>
#include <pthread.h>
#include <time.h>
char cmd;
std::vector<std::tuple<int, int>> edge_vector;
int num_vertexes;
pthread_mutex_t lock;
class Graphs
{
public:
    std::multimap<int, int> myMultimap;
    std::vector<int> true_assignment;
    Graphs(std::vector<std::tuple<int, int>> edge_vector)
    {
        for (auto i = edge_vector.begin(); i != edge_vector.end(); i++)
        {
            int first = std::get<0>(*i);
            myMultimap.insert({first, std::get<1>(*i)});
        }
    }
};
std::string runGraphGen(unsigned numNodes)
{
    std::string graphGenCommand;

    // Check if graphGen is in the original path
    FILE *originalPathCheck = popen("/home/agurfink/ece650/graphGen 2>/dev/null", "r");
    if (!originalPathCheck)
    {
        pclose(originalPathCheck);
        graphGenCommand = "/home/agurfink/ece650/graphGen";
    }
    else
    {
        // If not in the original path, use the project folder
        graphGenCommand = "./graphGen";
    }

    std::ostringstream command;
    command << graphGenCommand << " " << numNodes;

    FILE *pipe = popen(command.str().c_str(), "r");
    if (!pipe)
        return "";

    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }

    pclose(pipe);
    return result;
}
void *parse_line(void *arg)
{
    std::string *input_line = static_cast<std::string *>(arg);
    std::istringstream input(*input_line);
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream line_stream(line);
        char cmd;
        line_stream >> cmd;

        if (cmd == 'V')
        {
            line_stream >> num_vertexes;
        }
        else if (cmd == 'E')
        {
            edge_vector.clear();
            char ch;
            line_stream >> ch; // Skip the opening '{'
            while (line_stream >> ch && ch != '}')
            {
                if (ch == '<')
                {
                    int x, y;
                    line_stream >> x >> ch >> y >> ch; // Read '<x,y>'
                    edge_vector.push_back(std::make_tuple(x, y));
                }
            }
        }
    }
    return nullptr;
}
bool find_vertex_cover(Graphs *a, int k)
{

    int n = num_vertexes;
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::Lit propositions[n][k];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            propositions[i][j] = Minisat::mkLit(solver->newVar());
        }
    }
    Minisat::vec<Minisat::Lit> clause1;
    for (int j = 0; j < k; j++)
    {

        for (int i = 0; i < n; i++)
        {
            clause1.push(propositions[i][j]);
        }
        solver->addClause(clause1);
        clause1.clear();
    }

    for (int i = 0; i < n; i++)
    {
        for (int p = 0; p < k - 1; p++)
        {
            for (int q = p + 1; q < k; q++)
            {
                solver->addClause(~propositions[i][p], ~propositions[i][q]);
            }
        }
    }
    for (int j = 0; j < k; j++)
    {
        for (int p = 0; p < n; p++)
        {
            for (int q = p + 1; q < n; q++)
            {
                solver->addClause(~propositions[p][j], ~propositions[q][j]);
            }
        }
    }
    std::multimap<int, int> edges = a->myMultimap;
    Minisat::vec<Minisat::Lit> clause4;
    for (auto i = edges.begin(); i != edges.end(); i++)
    {
        int key = i->first;
        for (int vertex_cover_index = 0; vertex_cover_index < k; vertex_cover_index++)
        {
            clause4.push(propositions[key - 1][vertex_cover_index]);
        }
        int value = i->second;
        for (int vertex_cover_index = 0; vertex_cover_index < k; vertex_cover_index++)
        {
            clause4.push(propositions[value - 1][vertex_cover_index]);
        }
        solver->addClause(clause4);
        clause4.clear();
    }
    clause4.clear();
    clause1.clear();
    bool res = solver->solve();
    if (res == 0)
    {
        solver.reset(new Minisat::Solver());
        return res;
    }
    for (int j = 0; j < k; j++)
    {
        for (int i = 0; i < n; i++)
        {
            if (Minisat::toInt(solver->modelValue(propositions[i][j])) == 0)
            {
                a->true_assignment.push_back(i + 1);
            }
        }
    }
    solver.reset(new Minisat::Solver());

    return res;
}

void *find_Approx_vc_first(void *arg)
{
    // std::string *input_line = static_cast<std::string *>(arg);
    pthread_mutex_lock(&lock);

    Graphs *a1 = static_cast<Graphs *>(arg);
    a1->true_assignment.clear();

    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);
    struct timespec start_time;
    clock_gettime(cid, &start_time);

    std::multimap<int, int> current_map = a1->myMultimap;
    std::unordered_map<int, int> vertex_edge_cnt;
    for (auto i = current_map.begin(); i != current_map.end(); i++)
    {
        vertex_edge_cnt[i->first]++;
        vertex_edge_cnt[i->second]++;
    }
    while (!current_map.empty())
    {
        int max = -1;
        int max_vertex = -100;
        for (auto &iter : vertex_edge_cnt)
        {
            if (iter.second > max)
            {
                max = iter.second;
                max_vertex = iter.first;
            }
        }
        a1->true_assignment.push_back(max_vertex);
        for (auto iter = current_map.begin(); iter != current_map.end();)
        {
            auto erase_iter = iter++;
            if (erase_iter->second == max_vertex || erase_iter->first == max_vertex)
            {
                current_map.erase(erase_iter);
                vertex_edge_cnt[erase_iter->second]--;
                vertex_edge_cnt[erase_iter->first]--;
                vertex_edge_cnt[max_vertex] = INT_MIN;
            }
        }
    }
    struct timespec end_time;
    clock_gettime(cid, &end_time);
    long elapsed_secs = end_time.tv_sec - start_time.tv_sec;
    long elapsed_nanosecs = end_time.tv_nsec - start_time.tv_nsec;
    long micro_sec = elapsed_secs * 1000000 + elapsed_nanosecs / 1000;
    std::sort(a1->true_assignment.begin(), a1->true_assignment.end());
    std::cout << "APPROX-VC-1:  ";
    for (auto it = a1->true_assignment.begin(); it != a1->true_assignment.end(); ++it)
    {
        if (std::next(it) == a1->true_assignment.end())
        {
            std::cout << *it << std::endl;
        }
        else
        {
            std::cout << *it << " ";
        }
    }
    std::cout << "Time APPROX-VC-1: micro seconds " << micro_sec << std::endl;
    pthread_mutex_unlock(&lock);
    return nullptr;
}
void *find_Approx_vc_second(void *arg)
{
    pthread_mutex_lock(&lock);
    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);
    struct timespec start_time;
    clock_gettime(cid, &start_time);
    Graphs *a1 = static_cast<Graphs *>(arg);
    a1->true_assignment.clear();
    std::multimap<int, int> current_map = a1->myMultimap;
    while (!current_map.empty())
    {
        auto i = current_map.begin();
        if (i != current_map.end())
        {
            int first_edge = i->first;
            int second_edge = i->second;
            a1->true_assignment.push_back(first_edge);
            a1->true_assignment.push_back(second_edge);
            // std::cout << std::endl;
            //  std::cout << "next round" << std::endl;
            for (i = current_map.begin(); i != current_map.end();)
            {
                auto erase_iter = i++;
                if (erase_iter->second == first_edge || erase_iter->first == first_edge || erase_iter->second == second_edge || erase_iter->first == second_edge)
                {
                    current_map.erase(erase_iter);
                }
            }
        }
    }
    std::sort(a1->true_assignment.begin(), a1->true_assignment.end());
    struct timespec end_time;
    clock_gettime(cid, &end_time);
    long elapsed_secs = end_time.tv_sec - start_time.tv_sec;
    long elapsed_nanosecs = end_time.tv_nsec - start_time.tv_nsec;
    long micro_sec = elapsed_secs * 1000000 + elapsed_nanosecs / 1000;
    std::cout << "APPROX-VC-2:  ";
    for (auto it = a1->true_assignment.begin(); it != a1->true_assignment.end(); ++it)
    {
        if (std::next(it) == a1->true_assignment.end())
        {
            std::cout << *it << std::endl;
        }
        else
        {
            std::cout << *it << " ";
        }
    }
    std::cout << "Time APPROX-VC-2: in micro seconds: " << micro_sec << std::endl;
    pthread_mutex_unlock(&lock);
    return nullptr;
}
void *callsatsolver(void *arg)
{
    pthread_mutex_lock(&lock);
    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);
    struct timespec start_time;
    clock_gettime(cid, &start_time);
    Graphs *a1 = static_cast<Graphs *>(arg);
    a1->true_assignment.clear();
    for (int i = 1; i <= num_vertexes; i++)
    {

        int k = i;
        bool result = find_vertex_cover(a1, k);
        struct timespec end_time;
        clock_gettime(cid, &end_time);
        if (result == 1)
        {

            std::cout << "CNF-SAT-VC:  ";
            std::sort(a1->true_assignment.begin(), a1->true_assignment.end());
            for (auto it = a1->true_assignment.begin(); it != a1->true_assignment.end(); ++it)
            {
                if (std::next(it) == a1->true_assignment.end())
                {
                    std::cout << *it << std::endl;
                }
                else
                {
                    std::cout << *it << " ";
                }
            }
            long elapsed_secs = end_time.tv_sec - start_time.tv_sec;
            long elapsed_nanosecs = end_time.tv_nsec - start_time.tv_nsec;
            long micro_sec = elapsed_secs * 1000000 + elapsed_nanosecs / 1000;
            std::cout << "Time SAT Solver, in micro seconds: " << micro_sec << std::endl;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    return nullptr;
}
int main(int argc, char **argv)
{
    pthread_t thread_io;
    pthread_t thread_vc1;
    pthread_t thread_vc2;
    pthread_t thread_sat;

    while (!std::cin.eof())
    {
        std::string input = runGraphGen(5);

        pthread_create(&thread_io, nullptr, parse_line, &input);
        pthread_join(thread_io, nullptr);
        // parse_line(input_line);
        Graphs a(edge_vector);
        pthread_create(&thread_sat, nullptr, callsatsolver, &a);

        pthread_create(&thread_vc1, nullptr, find_Approx_vc_first, &a);
        // find_Approx_vc_first(a);
        pthread_create(&thread_vc2, nullptr, find_Approx_vc_second, &a);
        pthread_join(thread_sat, nullptr);
        pthread_join(thread_vc1, nullptr);
        pthread_join(thread_vc2, nullptr);
        // find_Approx_vc_second(a);
        sleep(2);
    }
    return 0;
}
