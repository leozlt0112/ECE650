// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <tuple> // for tuple
#include <list>
#include <climits>
#include <string>
// using namespace std;

class graphs
{
public:
    std::vector<std::vector<int>> edges;
    int vertexes = 0;
    std::vector<std::vector<int>> predecessor;
    std::list<int> bfs_queue;
    std::vector<int> distances;
    int graph_source;
    int graph_destination;
    std::string error_message;
    graphs(int num_vertexes)
    {
        vertexes = num_vertexes;
        distances.clear();
        predecessor.clear();
        distances.resize(num_vertexes);
        predecessor.resize(num_vertexes + 1);
        for (long unsigned int i = 0; i < distances.size(); ++i)
        {
            distances[i] = INT_MAX;
        }
    }
    bool set(std::vector<std::tuple<int, int>> edge_vector_a)
    {
        int limit = vertexes * vertexes;
        edges.clear();
        edges.resize(limit);
        for (auto i = edge_vector_a.begin(); i != edge_vector_a.end(); ++i)
        {
            int x = std::get<0>(*i);
            int y = std::get<1>(*i);
            if (x == 0 || x > vertexes || y == 0 || y > vertexes)
            {
                return false;
            }
        }
        for (auto i = edge_vector_a.begin(); i != edge_vector_a.end(); ++i)
        {
            int x = std::get<0>(*i);
            int y = std::get<1>(*i);
            edges[x].push_back(y);
            edges[y].push_back(x);
        }
        return true;
    }
    bool check_for_source_dest(int graph_source, int graph_destination, std::vector<std::vector<int>> edges)
    {
        bool source_exist = false;
        bool dest_exist = false;
        if (graph_source <= 0 || graph_source > vertexes)
        {
            error_message = "Error: bad source";
            return false;
        }
        for (auto i = edges.begin(); i != edges.end(); ++i)
        {
            for (auto j = i->begin(); j != i->end(); ++j)
            {
                if (*j == graph_source)
                {
                    source_exist = true;
                }
                if (*j == graph_destination)
                {
                    dest_exist = true;
                }
            }
        }
        if (source_exist == false || dest_exist == false)
        {
            error_message = "Error source and destination do not exist";
            return false;
        }
        return true;
    }
    bool bfs_distances(int input_source, int input_destination)
    {
        graph_source = input_source;
        graph_destination = input_destination;
        bool bad_source_or_dest = check_for_source_dest(graph_source, graph_destination, edges);
        if (bad_source_or_dest == false)
        {
            return false;
        }
        bool path_exists = false;
        distances[graph_source - 1] = 0;
        bfs_queue.clear();
        bfs_queue.push_back(graph_source);

        while (bfs_queue.empty() == false)
        {
            int u = bfs_queue.front();
            bfs_queue.pop_front();
            for (auto i = edges[u].begin(); i != edges[u].end(); ++i)
            {
                if (distances[*i - 1] == INT_MAX)
                {
                    distances[*i - 1] = distances[u - 1] + 1;
                    predecessor[*i].push_back(u);
                    bfs_queue.push_back(*i);
                }
                if (*i == graph_destination)
                {
                    path_exists = true;
                }
            }
        }
        if (path_exists == false)
        {
            error_message = "Error: no path";
            return false;
        }
        else
        {
            return true;
        }
    }
    void print_predessor()
    {
        std::vector<int> final_output;
        final_output.push_back(graph_destination);
        int next = predecessor[graph_destination][0];
        final_output.push_back(next);
        while (next != graph_source)
        {
            next = predecessor[next][0];
            final_output.push_back(next);
        }
        for (int i = final_output.size() - 1; i >= 0; i--)
        {
            if (i != 0)
            {
                std::cout << final_output[i] << "-";
            }
            else
            {
                std::cout << final_output[i];
            }
        }
        std::cout << std::endl;
    }
};
char cmd;
std::vector<std::tuple<int, int>> edge_vector;
int num_vertexes;
int source;
int destination;

void parse_line(std::string input_line, bool first)
{
    std::istringstream input(input_line);

    input >> cmd;
    if (cmd == 'V')
    {
        input >> num_vertexes;
    }
    else if (cmd == 'E')
    {
        if (first)
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

    else if (cmd == 's')
    {
        input >> source >> destination;
    }
}
int main(int argc, char **argv)
{
    // std::vector<std::vector<int>> edges2;
    char previous = '\0';
    bool first = true;
    while (!std::cin.eof())
    {
        std::string input_line;
        std::getline(std::cin, input_line);

        if (input_line.size() == 0)
        {
            continue;
        }
        else
        {
            parse_line(input_line, first);
            if (cmd == 'E')
            {
                first = false;
            }
            if (cmd == 's')
            {
                graphs a(num_vertexes);
                a.set(edge_vector);
                if (source == destination)
                {
                    if (a.set(edge_vector) == false)
                    {
                        std::cerr << "Error: graph do not exist "
                                  << std::endl;
                        continue;
                    }
                    if (source > 0 && source <= num_vertexes)
                    {
                        std::cout << source << std::endl;
                        continue;
                    }
                }
                bool returned = a.bfs_distances(source, destination);
                if (returned == false)
                {
                    std::cerr << "Error: " << a.error_message << std::endl;
                }
                else
                {
                    a.print_predessor();
                    first = true;
                }
            }
            else if (cmd == 'E')
            {
                if (previous == 'E')
                {
                    continue;
                }
                std::cout << input_line << std::endl;
                graphs a(num_vertexes);
                if (a.set(edge_vector) == false)
                {
                    std::string error_message = "Error: edge was above max vertexes allowed or below equal to 0";
                    std::cerr << "Error: " << error_message << std::endl;
                    goto jump;
                }
            }
            else if (cmd == 'V')
            {
                std::cout << input_line << std::endl;
                if (num_vertexes <= 1)
                {
                    std::string error_message = "Error: vertexes below or equal 0";
                    std::cerr << "Error: " << error_message << std::endl;
                }
                first = true;
            }
        jump:
            previous = cmd;
        }
    }
    return 0;
}
