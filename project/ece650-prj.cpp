// Compile with c++ ece650-prjcpp -std=c++11 -o ece650-prj
#include <memory>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>
#include <future>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace Minisat;

// Define mutex to protect shared resources
std::mutex mtx;

// Thread synchronization
std::promise<void> cnfSatVCPromise;
std::future<void> cnfSatVCFuture = cnfSatVCPromise.get_future();
std::promise<void> approxVC1Promise;
std::future<void> approxVC1Future = approxVC1Promise.get_future();

// Graph: edges (pair of ids)
using Edges = unordered_set<pair<unsigned, unsigned>>;

// Hash for pair
namespace std
{
    template <>
    struct hash<pair<unsigned int, unsigned int>>
    {
        size_t operator()(const pair<unsigned int, unsigned int>& p) const
        {
            return hash<unsigned int>{}(p.first) ^ hash<unsigned int>{}(p.second);
        }
    };
}

// Function to read the graph from a file
std::string runGraphGen(unsigned numNodes)
{
    std::string graphGenCommand;

    // Check if graphGen is in the original path
    FILE* originalPathCheck = popen("/home/agurfink/ece650/graphGen 2>/dev/null", "r");
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
    
    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) return "";

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }

    pclose(pipe);
    return result;
}

// Function to read the graph from the output
bool readGraph(const std::string& graphGenOutput, uint& numNodes, Edges& edges)
{
    std::istringstream input(graphGenOutput);
    std::string line;

    while (std::getline(input, line))
    {
        // Skip empty lines
        if (line.empty())
            continue; 

        // Read number of nodes
        if (line[0] == 'V')
        {
            // Skip first 2 char and read
            string extract = line.substr(2);
            numNodes = stoi(extract);

            // Error check
            if (numNodes <= 1)
            {
                std::cerr << "Error: Number of nodes must be larger than 1" << std::endl;
                numNodes = 0; // reset
                return false;
            }
        }

        // Read edges
        else if (line[0] == 'E')
        {
            // Reset edges
            edges.clear();

            // Skip first 3 characters and last character
            std::string extract = line.substr(3, line.size() - 4);
            std::istringstream ss(extract);
            if (extract.length() == 0)
            {
                std::cout << std::endl;
                // std::cerr << "Error: Empty graph, no vertex cover" << std::endl;
                return false;
            }

            // Define variables
            char openBracket = '<', closeBracket = '>', comma = ',';
            unsigned node1, node2;

            // Loop through each pair of nodes
            while (ss >> openBracket >> node1 >> comma >> node2 >> closeBracket)
            {
                edges.insert({node1, node2});

                // End of last pair
                if (!(ss >> comma))
                    return true;
            }
        }
    }

    // Should not reach here
    return false;
}

// CNF-SAT-VC solver Rule 1: At least one vertex is the ith vertex in the vertex cover
// Expected Clauses = k
void ruleAtLeastOneVertex(Solver& solver, int n, int k)
{
    // Clause i in k
    for (int i = 0; i < k; ++i)
    {
        // Create new clause
        vec<Lit> clause;
        // Add literals
        for (int j = 0; j < n; ++j)
            clause.push(mkLit(Var(i * n + j)));
        // Add clause
        solver.addClause(clause);
    }
}

// CNF-SAT-VC solver Rule 2: No one vertex can appear twice in a vertex cover
// Expected Clauses = n * (k choose 2)
void ruleNoRepeatedVertex(Solver& solver, int n, int k)
{
    // Clause m in n
    for (int m = 0; m < n; ++m)
        // Clause k choose 2
        for (int q = 0; q < k; ++q)
            for (int p = q + 1; p < k; ++p)
                solver.addClause(~mkLit(Var(p * n + m)), ~mkLit(Var(q * n + m)));
}

// CNF-SAT-VC solver Rule 3: No more than one vertex appears in the mth position of the vertex cover
// Expected Clauses = k * (n choose 2)
void ruleNoMoreThanOneVertexPerPosition(Solver& solver, int n, int k)
{
    // Clause m in k
    for (int m = 0; m < k; ++m)
        // Clause n choose 2
        for (int q = 0; q < n; ++q)
            for (int p = q + 1; p < n; ++p)
                solver.addClause(~mkLit(Var(m * n + p)), ~mkLit(Var(m * n + q)));
}

// CNF-SAT-VC solver Rule 4: Every edge is incident to at least one vertex in the vertex cover
// Expected Clauses = |E|
void ruleEdgeIncidence(Solver& solver, const Edges& edges, int n, int k)
{
    // Clause edge in edges
    for (const auto& edge : edges)
    {
        // Create new clause
        vec<Lit> clause;

        // Add literals
        for (int i = 0; i < k; ++i)
        {
            clause.push(mkLit(Var(i * n + edge.first - 1)));
            clause.push(mkLit(Var(i * n + edge.second - 1)));
        }

        // Add clause
        solver.addClause(clause);
    }
}

// CNF-SAT-VC solver
std::string mvcSolver(int n, Edges edges)
{
    // Master loop to start with k=1 until minimum-sized Vertex Cover is found
    int k = 1;
    while (true)
    {
        // Allocate on heap
        unique_ptr<Solver> solver(new Solver());

        // Create literals
        for (int i = 0; i < k; i++)
            for (int j = 0; j < n; j++)
                solver->newVar();

        // Generate clauses based on the current value of k
        ruleAtLeastOneVertex(*solver, n, k);
        ruleNoRepeatedVertex(*solver, n, k);
        ruleNoMoreThanOneVertexPerPosition(*solver, n, k);
        ruleEdgeIncidence(*solver, edges, n, k);

        // Solve the SAT problem
        bool res = solver->solve();
        if (res)
        {
            // Interpret SAT Result
            vector<int> vertexCover;
            for (int i = 0; i < k; ++i)
            {
                for (int j = 0; j < n; ++j)
                {
                    // Check if x_ij has SAT assignment of true
                    if (solver->modelValue(mkLit(i * n + j)) == l_True)
                    {
                        vertexCover.push_back(j + 1); // append node ID
                        break; // next vertex
                    }
                }
            }

            // Build the result string
            std::ostringstream result;
            // Output the sorted node IDs
            std::sort(vertexCover.begin(), vertexCover.end());
            for (size_t idx = 0; idx < vertexCover.size(); ++idx)
                result << (idx == 0 ? "" : ",") << vertexCover[idx];
            return result.str(); // Solution found
        }

        // Increment k
        ++k;

        // Error handling
        if (k > n)
        {
            cerr << "Error: Unsatisfiable based on given V and E" << endl;
            return ""; // No solution found
        }

        // De-allocate existing solver and allocate a new one
        solver.reset(new Solver());
    }
}

// Function to implement CNF-SAT-VC algorithm
void cnfSatVC(const uint numNodes, const Edges& edges)
{
    // Lock
    std::lock_guard<std::mutex> lock(mtx);

    // Output
    std::cout << "CNF-SAT-VC: " << mvcSolver(numNodes, edges) << endl;

    // Unblock
    cnfSatVCPromise.set_value();
}

// Function to implement APPROX-VC-1 algorithm
void approxVC1(const uint numNodes, const Edges& edges)
{
    // Lock
    std::lock_guard<std::mutex> lock(mtx);

    // Initialization
    Edges remainingEdges = edges;
    std::vector<uint> vertexCover;

    // Main loop
    while (!remainingEdges.empty())
    {
        // Count degree of all nodes
        std::unordered_map<uint, uint> degreeMap;
        for (const auto& edge : remainingEdges)
        {
            degreeMap[edge.first]++;
            degreeMap[edge.second]++;
        }

        // Get node with max degree
        uint maxDegreeNode = 0;
        uint maxDegree = 0;
        for (const auto& entry : degreeMap)
        {
            if (entry.second > maxDegree)
            {
                maxDegree = entry.second;
                maxDegreeNode = entry.first;
            }
        }

        // Add the node to the vertex cover
        vertexCover.push_back(maxDegreeNode);

        // Remove all edges incident to this node
        Edges edgesToRemove;
        std::copy_if
        (
            remainingEdges.begin(),
            remainingEdges.end(),
            std::inserter(edgesToRemove, edgesToRemove.begin()),
            [maxDegreeNode](const std::pair<unsigned int, unsigned int>& edge) -> bool
            {
                return edge.first == maxDegreeNode || edge.second == maxDegreeNode;
            }
        );

        for (const auto& edge : edgesToRemove)
            remainingEdges.erase(edge);
    }

    // Sort
    std::sort(vertexCover.begin(), vertexCover.end());

    // Output
    std::cout << "APPROX-VC-1: ";
    for (size_t idx = 0; idx < vertexCover.size(); ++idx)
        std::cout << (idx == 0 ? "" : ",") << vertexCover[idx];
    std::cout << std::endl;

    // Unblock
    approxVC1Promise.set_value();
}

// Function to implement APPROX-VC-2 algorithm
void approxVC2(const uint numNodes, const Edges& edges)
{
    // Lock
    std::lock_guard<std::mutex> lock(mtx);

    // Initialization
    std::unordered_set<uint> vertexCover;
    Edges remainingEdges = edges;

    // Main loop
    while (!remainingEdges.empty())
    {
        // Pick edge <u,v>
        const auto& edge = *remainingEdges.begin();
        uint u = edge.first;
        uint v = edge.second;

        // Add both u and v to the vertex cover
        vertexCover.insert(u);
        vertexCover.insert(v);

        // Remove all edges attached to u and v
        Edges edgesToRemove;
        std::copy_if
        (
            remainingEdges.begin(),
            remainingEdges.end(),
            std::inserter(edgesToRemove, edgesToRemove.begin()),
            [u, v](const std::pair<unsigned int, unsigned int>& e) -> bool
            {
                return e.first == u || e.second == u || e.first == v || e.second == v;
            }
        );

        for (const auto& edge : edgesToRemove)
            remainingEdges.erase(edge);
    }

    // Sort
    std::vector<uint> sortedVertexCover(vertexCover.begin(), vertexCover.end());
    std::sort(sortedVertexCover.begin(), sortedVertexCover.end());

    // Output
    std::cout << "APPROX-VC-2: ";
    for (size_t idx = 0; idx < sortedVertexCover.size(); ++idx) {
        std::cout << sortedVertexCover[idx];
        if (idx < sortedVertexCover.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << std::endl;
}

// Main Entry
int main(int argc, char** argv)
{
    // Create threads for each algorithm
    std::thread ioThread;
    std::thread cnfSatVCThread;
    std::thread approxVC1Thread;
    std::thread approxVC2Thread;

    while (true)
    {
        // Option 1: Input from graphGen
        // std::string graphGenOutput = runGraphGen(10);

        // Option 2: Input from stdin
        std::string graphGenOutput;
        std::string line;
        if (!std::cin.eof())
        {
            std::getline(std::cin, line);
            if (line.size() == 0)
                continue;
            graphGenOutput += line;
            graphGenOutput += "\n";
            std::getline(std::cin, line);
            graphGenOutput += line;
            graphGenOutput += "\n";
        }
        else
            break; // EOF

        // std::cout << graphGenOutput;

        // Only error handling before enter multithreading, the graph is still read in multithreading
        // we don't want to interrupt the threads if IO thread encounter an issue during multithreading
        uint numNodes;
        Edges edges;
        if (!readGraph(graphGenOutput, numNodes, edges))
            continue;

        // Reset promises and futures for synchronization
        cnfSatVCPromise = std::promise<void>();
        cnfSatVCFuture = cnfSatVCPromise.get_future();
        approxVC1Promise = std::promise<void>();
        approxVC1Future = approxVC1Promise.get_future();

        // Create threads for each algorithm
        ioThread = std::thread(readGraph, graphGenOutput, std::ref(numNodes), std::ref(edges));
        ioThread.join();

        cnfSatVCThread = std::thread(cnfSatVC, std::cref(numNodes), std::cref(edges));
        approxVC1Thread = std::thread(approxVC1, std::cref(numNodes), std::cref(edges));
        approxVC2Thread = std::thread(approxVC2, std::cref(numNodes), std::cref(edges));

        // Wait
        cnfSatVCFuture.wait();
        approxVC1Future.wait();

        // Join threads
        cnfSatVCThread.join();
        approxVC1Thread.join();
        approxVC2Thread.join();

        // Delay
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
