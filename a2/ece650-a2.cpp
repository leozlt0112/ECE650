// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <tuple> // for tuple
#include <list>
#include <climits>
#include <string>
using namespace std;

class graphs
{
public:
    std::vector<std::vector<int>> edges;
    int vertexes = 0;
    std::vector<std::vector<int>> predecessor;
    // vertexes goes from 1 to max (1 tp 5)
    // distances goes from 0 to max-1 (0 to 4)
    list<int> bfs_queue;
    std::vector<int> distances;
    int graph_source;
    int graph_destination;
    string error_message;
    graphs(std::vector<std::vector<int>> new_edges, int num_vertexes)
    {
        edges = new_edges;
        // graph_source = source1;
        // graph_destination = destination1;
        vertexes = num_vertexes;
        distances.clear();
        predecessor.clear();
        distances.resize(num_vertexes);
        predecessor.resize(num_vertexes + 1);
        for (int i = 0; i < distances.size(); ++i)
        {
            distances[i] = INT_MAX;
        }
    }
    void set(std::vector<std::tuple<int, int>> edge_vector_a)
    {
        int limit = vertexes * vertexes;
        edges.clear();
        edges.resize(limit);
        for (auto i = edge_vector_a.begin(); i != edge_vector_a.end(); ++i)
        {
            int x = get<0>(*i);
            int y = get<1>(*i);
            edges[x].push_back(y);
            edges[y].push_back(x);
        }
    }
    bool bfs_distances(int source1, int destination1)
    {
        graph_source = source1;
        graph_destination = destination1;
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
        bool path_exists = false;
        bool visted_vertex[vertexes];
        for (int i = 0; i < vertexes; ++i)
        {
            visted_vertex[i] = false;
        }

        distances[graph_source - 1] = 0;
        visted_vertex[graph_source - 1] = true;
        bfs_queue.clear();
        bfs_queue.push_back(graph_source);

        while (bfs_queue.empty() == false)
        {
            int u = bfs_queue.front();
            bfs_queue.pop_front();
            for (auto i = edges[u].begin(); i != edges[u].end(); ++i)
            {
                if (visted_vertex[*i - 1] == false)
                {
                    distances[*i - 1] = distances[u - 1] + 1;
                    predecessor[*i].push_back(u);
                    bfs_queue.push_back(*i);
                    visted_vertex[*i - 1] = true;
                }
                if (*i == graph_destination)
                {
                    path_exists = true;
                }
            }
        }
        if (path_exists = false)
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
                cout << final_output[i] << "-";
            }
            else
            {
                cout << final_output[i];
            }
        }
        cout << "\n";
    }
};
char cmd;
std::vector<std::tuple<int, int>> edge_vector;
int num_vertexes;
int source;
int destination;
char previous = '\0';
void parse_line(std::string line2)
{
    std::istringstream input(line2);

    input >> cmd;
    if (cmd == 'V')
    {
        input >> num_vertexes;
    }
    else if (cmd == 'E')
    {
        if (previous != 'E')
        {
            // nums_edges.resize((num_vertexes * num_vertexes));
            edge_vector.clear();
            line2 = line2.substr(3);
            while (line2.size() != 0)
            {
                if (line2[0] == '<' || line2[0] == '>')
                {
                    line2 = line2.substr(1);
                }
                else
                {
                    if (isdigit(line2[0]))
                    {
                        std::string num1("");
                        std::string num2("");
                        // please check the case
                        num1.push_back(line2[0]);
                        for (int i = 1; i < line2.size(); i++)
                        {
                            if (isdigit(line2[i]))
                            {
                                num1.push_back(line2[i]);
                            }
                            else
                            {
                                line2 = line2.substr(i + 1);
                                break;
                            }
                        }
                        if (isdigit(line2[0]))
                        {
                            num2.push_back(line2[0]);
                            for (int i = 1; i < line2.size(); i++)
                            {
                                if (isdigit(line2[i]))
                                {
                                    num2.push_back(line2[i]);
                                }
                                else
                                {
                                    line2 = line2.substr(i);
                                    break;
                                }
                            }
                        }
                        // cout << num1 << "\n";
                        // cout << num2 << "\n";
                        int x = stoi(num1);
                        int y = stoi(num2);
                        edge_vector.push_back(std::make_tuple(x, y));

                        // line2 = line2.substr(3);
                    }

                    else
                    {
                        line2 = line2.substr(1);
                    }
                }
            }
        }
    }

    else if (cmd == 's')
    {
        input >> source >> destination;
    }
    previous = cmd;
}
int main(int argc, char **argv)
{
    std::vector<std::vector<int>> edges2;
    char previous2;
    while (!std::cin.eof())
    {
        std::string line2;
        std::getline(std::cin, line2);

        if (line2.size() == 0)
        {
            continue;
        }
        else
        {
            parse_line(line2);
            if (cmd == 's')
            {
                graphs a(edges2, num_vertexes);
                a.set(edge_vector);
                edges2 = a.edges;
                if (source == destination)
                {
                    if (source > 0 && source <= num_vertexes)
                    {
                        cout << source << "-" << destination << "\n";
                        continue;
                    }
                }
                /*
                for (int i = 0; i < edges2.size(); ++i)
                {
                    for (int j = 0; j < edges2[i].size(); ++j)
                    {
                        cout << edges2[i][j] << "\n";
                    }
                }
                */
                bool returned = a.bfs_distances(source, destination);
                if (returned == false)
                {
                    std::cerr << "Error: " << a.error_message << "\n";
                }
                else
                {
                    a.print_predessor();
                }
            }
            else if (cmd == 'E')
            {
                if (previous2 == 'E')
                {
                    continue;
                }
                graphs a(edges2, num_vertexes);
                a.set(edge_vector);
                edges2 = a.edges;
                for (auto i = edges2.begin(); i != edges2.end(); ++i)
                {
                    for (auto j = i->begin(); j != i->end(); ++j)
                    {
                        if (*j > num_vertexes || *j <= 0)
                        {
                            string error_message = "Error: edge was above max vertexes allowed or below equal to 0";
                            std::cerr << "Error: " << error_message << "\n";
                            break;
                        }
                    }
                }
            }
            else if (cmd == 'V')
            {
                if (num_vertexes <= 0)
                {
                    string error_message = "Error: vertexes below or equal 0";
                    std::cerr << "Error: " << error_message << "\n";
                }
            }
            previous2 = cmd;
        }
    }
    return 0;
}
