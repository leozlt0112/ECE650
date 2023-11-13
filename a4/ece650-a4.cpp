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
// using namespace std;
char cmd;
std::vector<std::tuple<int, int>> edge_vector;
int num_vertexes;
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
            // std::cout << first << std::endl;
            myMultimap.insert({first, std::get<1>(*i)});
        }
    }
    void print()
    {
        // int current_key = -1;
        for (auto i = myMultimap.begin(); i != myMultimap.end(); i++)
        {
            int key = i->first;
            std::cout << key << std::endl;
        }
    }
};
void parse_line(std::string input_line)
{
    std::istringstream input(input_line);

    input >> cmd;
    if (cmd == 'V')
    {
        input >> num_vertexes;
    }
    else if (cmd == 'E')
    {
        edge_vector.clear();
        input_line = input_line.substr(3);
        while (input_line.size() != 0)
        {
            if (input_line[0] == '<' || input_line[0] == '>')
            {
                input_line = input_line.substr(1);
            }
            else
            {
                if (isdigit(input_line[0]))
                {
                    std::string num1("");
                    std::string num2("");
                    num1.push_back(input_line[0]);
                    for (long unsigned int i = 1; i < input_line.size(); i++)
                    {
                        if (isdigit(input_line[i]))
                        {
                            num1.push_back(input_line[i]);
                        }
                        else
                        {
                            input_line = input_line.substr(i + 1);
                            break;
                        }
                    }
                    if (isdigit(input_line[0]))
                    {
                        num2.push_back(input_line[0]);
                        for (long unsigned int i = 1; i < input_line.size(); i++)
                        {
                            if (isdigit(input_line[i]))
                            {
                                num2.push_back(input_line[i]);
                            }
                            else
                            {
                                input_line = input_line.substr(i);
                                break;
                            }
                        }
                    }
                    int x = stoi(num1);
                    int y = stoi(num2);
                    edge_vector.push_back(std::make_tuple(x, y));
                }

                else
                {
                    input_line = input_line.substr(1);
                }
            }
        }
    }
}
bool find_vertex_cover(Graphs &a, int k)
{
    int n = num_vertexes;
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::Lit propositions[n][k];
    // the clause vector are already organized in
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            propositions[i][j] = Minisat::mkLit(solver->newVar());
        }
    }
    /* the clause 1 actually contains k clauses*/
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
    // std::cout << "clause 1 " << std::endl;
    /* clause 2 working on no one vertex can appear twice in vertex cover*/
    /* actually contains n clauses*/
    for (int i = 0; i < n; i++)
    {
        // Minisat::vec<Minisat::Lit> clause2;
        for (int p = 0; p < k; p++)
        {
            for (int q = p + 1; q < k; q++)
            {
                solver->addClause(~propositions[i][p], ~propositions[i][q]);
            }
        }
    }
    // std::cout << "clause 2 " << std::endl;
    /* clause 3, no more than one vertex appears in the mth postion of the vertex cover*/
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
    // std::cout << "clause 3 " << std::endl;
    /* clause 4 */
    std::multimap<int, int> edges = a.myMultimap;
    // int current_key = -1;
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
    // std::cout << "clause 4 " << std::endl;
    bool res = solver->solve();
    if (res == 0)
    {
        return res;
    }
    // std::cout << res << std::endl;
    for (int j = 0; j < k; j++)
    {
        for (int i = 0; i < n; i++)
        {
            if (Minisat::toInt(solver->modelValue(propositions[i][j])) == 0)
            {
                a.true_assignment.push_back(i + 1);
            }
        }
    }
    return res;
}
int main(int argc, char **argv)
{
    // std::vector<std::vector<int>> edges2;
    while (!std::cin.eof())
    {
        std::string input_line;
        std::getline(std::cin, input_line);

        if (input_line.empty())
        {
            break;
        }
        else
        {
            parse_line(input_line);
            if (cmd == 'E')
            {
                Graphs a(edge_vector);
                // a.print();
                for (int i = 1; i <= num_vertexes; i++)
                {
                    int k = i;
                    bool result = find_vertex_cover(a, k);
                    if (result == 1)
                    {
                        std::sort(a.true_assignment.begin(), a.true_assignment.end());
                        for (auto it = a.true_assignment.begin(); it != a.true_assignment.end(); ++it)
                        {
                            if (std::next(it) == a.true_assignment.end())
                            {
                                std::cout << *it << std::endl;
                            }
                            else
                            {
                                std::cout << *it << " ";
                            }
                        }
                        break;
                    }
                }
            }
            else if (cmd == 'V')
            {
                //     std::cout << "V" << num_vertexes << std::endl;
            }
        }
    }
    return 0;
}
