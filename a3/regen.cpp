#include <iostream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <ctype.h>
#include <tuple> // for tuple
#include <list>
#include <climits>
#include <string>
#include <map>
#include <math.h>
bool genrate_random_num(int *rand_num, int min, int max, std::string &failed)
{
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail())
    {
        failed = "Error: cannot open /dev/urandom";
        return false;
    }
    unsigned int num;
    urandom.read(reinterpret_cast<char *>(&num), sizeof(int));
    // std::cout << num << std::endl;
    num = min + num % (max - min + 1);
    *rand_num = num;
    return true;
}
class new_graphs
{
public:
    std::multimap<std::string, std::tuple<int, int>> myMultimap;
    new_graphs()
    {
        myMultimap.clear();
        // myStreets_Segments.clear();
    }
};
bool check_if_same_pts(new_graphs &a1, std::tuple<int, int> my_tuple, std::string street_name, int curr)
{
    if (curr < 1)
    {
        return false;
    }
    auto range = a1.myMultimap.equal_range(street_name);
    for (auto itr = range.first; itr != range.second; ++itr)
    {
        int x0 = std::get<0>(itr->second);
        int y0 = std::get<1>(itr->second);
        if (x0 == std::get<0>(my_tuple) && y0 == std::get<1>(my_tuple))
        {
            return true;
        }
    }
    return false;
}
bool if_distance_equal(int x0, int y0, int x1, int y1, int x, int y)
{
    int dx0 = x - x0;
    int dy0 = y - y0;
    int dx1 = x1 - x;
    int dy1 = y1 - y;
    int dx = x1 - x0;
    int dy = y1 - y0;
    float distance_to_source = sqrt(dx0 * dx0 + dy0 * dy0);
    float distance_to_dst = sqrt(dx1 * dx1 + dy1 * dy1);
    float distance_between = sqrt(dx * dx + dy * dy);
    float tolerance = 1e-6;
    if (abs(distance_between) - abs(distance_to_source + distance_to_dst) <= tolerance)
    {
        return true;
    }
    return false;
}
// curr_num_line_segments the number of pts currently in the graph rn
bool check_two_lines_parallel_and_overlap(int existing_x0, int existing_y0, int existing_x1, int existing_y1,
                                          int new_x0, int new_y0, int new_x1, int new_y1, bool is_vertical_new_segment, bool is_vertical_existing)
{
    if (existing_x0 == new_x0 && existing_y0 == new_y0 && existing_x1 == new_x1 && existing_y1 == new_y1)
    {
        return true;
    }
    else
    {
        if (is_vertical_new_segment && is_vertical_existing)
        {
            if (existing_x0 == new_x0 && existing_x1 == existing_x0)
            {
                // new completely overlaps existing
                if (existing_y0 >= new_y0 && existing_y1 <= new_y1)
                {
                    return true;
                }
                // new  partially overlaps existing lower
                if (existing_y0 >= new_y0 && existing_y0 < new_y1)
                {
                    return true;
                }
                // new partially overlaps existing higher
                if (existing_y1 > new_y0 && existing_y1 <= new_y1)
                {
                    return true;
                }
                // existing completely overlaps new
                if (new_y0 >= existing_y0 && new_y1 <= existing_y1)
                {
                    return true;
                }
                // existing partially overlaps new lower
                if (new_y0 >= existing_y0 && new_y0 < existing_y1)
                {
                    return true;
                }

                if (new_y1 > existing_y0 && new_y1 <= existing_y1)
                {
                    return true;
                }
            }
        }
        else
        {
            double rise_existing = existing_y1 - existing_y0;
            double run_existing = existing_x1 - existing_x0;
            double rise_new = new_y1 - new_y0;
            double run_new = new_x1 - new_x0;
            if (run_new == 0 || run_existing == 0)
            {
                return false;
            }
            double slope_existing = (double)rise_existing / run_existing;
            double slope_new = (double)rise_new / run_new;
            double tolerance = 1e-6;
            if (abs(slope_existing - slope_new) < tolerance)
            {
                if (slope_existing >= 0)
                {
                    // existing completely overlaps new
                    if ((existing_x0 <= new_x0 && existing_x1 >= new_x1) && (existing_y0 <= new_y0 && existing_y1 >= new_y1))
                    {
                        bool point1 = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x0, new_y0);
                        bool point2 = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x1, new_y1);
                        if (point1 && point2)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if ((existing_x0 <= new_x0 && existing_x1 > new_x0) && (existing_y0 <= new_y0 && existing_y1 >= new_y0))
                        {
                            bool point = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x0, new_y0);
                            if (point)
                            {
                                return true;
                            }
                        }
                        else if ((existing_x0 < new_x1 && existing_x1 >= new_x1) && (existing_y0 <= new_y1 && existing_y1 >= new_y1))
                        {
                            bool point = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x1, new_y1);
                            if (point)
                            {
                                return true;
                            }
                        }
                    }
                    // new overlaps existing
                    if ((new_x0 <= existing_x0 && new_x1 >= existing_x1) && (new_y0 <= existing_y0 && new_y1 >= existing_y1))
                    {
                        bool point1 = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x0, existing_y0);
                        bool point2 = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x1, existing_y1);
                        if (point1 & point2)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if ((new_x0 <= existing_x0 && new_x1 > existing_x0) && (new_y0 <= existing_y0 && new_y1 >= existing_y0))
                        {
                            bool point = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x0, existing_y0);
                            if (point)
                            {
                                return true;
                            }
                        }
                        else if ((new_x0 < existing_x1 && new_x1 >= existing_x1) && (new_y0 <= existing_y1 && new_y1 >= existing_y1))
                        {
                            bool point = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x1, existing_y1);
                            if (point)
                            {
                                return true;
                            }
                        }
                    }
                }
                else
                {
                    // negative slope existing overlaps new
                    if ((existing_x0 >= new_x0 && existing_x1 <= new_x1) && (existing_y0 <= new_y0 && existing_y1 >= new_y1))
                    {
                        bool point1 = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x0, new_y0);
                        bool point2 = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x1, new_y1);
                        if (point1 && point2)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if ((existing_x0 >= new_x0 && existing_x1 < new_x0) && (existing_y0 <= new_y0 && existing_y1 >= new_y0))
                        {
                            bool point = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x0, new_y0);
                            if (point)
                            {
                                return true;
                            }
                        }
                        else if ((existing_x0 > new_x1 && existing_x1 <= new_x1) && (existing_y0 <= new_y1 && existing_y1 >= new_y1))
                        {
                            bool point = if_distance_equal(existing_x0, existing_y0, existing_x1, existing_y1, new_x1, new_y1);
                            if (point)
                            {
                                return true;
                            }
                        }
                    }
                    if ((new_x0 >= existing_x0 && new_x1 <= existing_x1) && (new_y0 <= existing_y0 && new_y1 >= existing_y1))
                    {
                        bool point1 = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x0, existing_y0);
                        bool point2 = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x1, existing_y1);
                        if (point1 && point2)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if ((new_x0 >= existing_x0 && new_x1 < existing_x0) && (new_y0 <= existing_y0 && new_y1 >= existing_y0))
                        {
                            bool point = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x0, existing_y0);
                            if (point)
                            {
                                return true;
                            }
                        }
                        else if ((new_x0 > existing_x1 && new_x1 <= existing_x1) && (new_y0 <= existing_y1 && new_y1 >= existing_y1))
                        {
                            bool point = if_distance_equal(new_x0, new_y0, new_x1, new_y1, existing_x1, existing_y1);
                            if (point)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool check_if_overlap(int curr_num_line_segments, new_graphs &a1, std::tuple<int, int> my_tuple, std::string key)
{
    // if (curr_num_line_segments <= 1)
    // {
    //     if (!a1.myMultimap.empty())

    //         for (auto i = a1.myMultimap.begin(); i != a1.myMultimap.end(); i++)
    //         {
    //         }
    // }
    auto range = a1.myMultimap.equal_range(key);
    int graph_src_x;
    int graph_src_y;
    int new_vector_x = std::get<0>(my_tuple);
    int new_vector_y = std::get<1>(my_tuple);
    int new_segment_src_x;
    int new_segment_src_y;
    int new_segment_dst_x;
    int new_segment_dst_y;
    if (range.first != a1.myMultimap.end())
    {
        // Set 'last' to the first element of the range to start
        auto last = range.first;

        // Iterate through the range, updating 'last' as we go
        for (auto it = range.first; it != range.second; ++it)
        {
            last = it;
        }
        graph_src_x = std::get<0>(last->second);
        graph_src_y = std::get<1>(last->second);
    }
    else
    {
        // std::cout << "There is a problem with multimap, checklogic"
        //         << std::endl;
    }
    bool is_vertical_new_segment = false;
    // dw about vertical line rn
    if (graph_src_x != new_vector_x)
    {

        if (graph_src_y == new_vector_y)
        {
            if (graph_src_x <= new_vector_x)
            {
                new_segment_src_x = graph_src_x;
                new_segment_src_y = graph_src_y;
                new_segment_dst_x = new_vector_x;
                new_segment_dst_y = new_vector_y;
            }
            else
            {
                new_segment_src_x = new_vector_x;
                new_segment_src_y = new_vector_y;
                new_segment_dst_x = graph_src_x;
                new_segment_dst_y = graph_src_y;
            }
        }
        else
        {
            if (graph_src_x <= new_vector_x)
            {
                if (graph_src_y <= new_vector_y)
                {
                    new_segment_src_x = graph_src_x;
                    new_segment_src_y = graph_src_y;
                    new_segment_dst_x = new_vector_x;
                    new_segment_dst_y = new_vector_y;
                }
                else
                {
                    new_segment_src_x = new_vector_x;
                    new_segment_src_y = new_vector_y;
                    new_segment_dst_x = graph_src_x;
                    new_segment_dst_y = graph_src_y;
                }
            }
            else
            {
                if (graph_src_y <= new_vector_y)
                {
                    new_segment_src_x = graph_src_x;
                    new_segment_src_y = graph_src_y;
                    new_segment_dst_x = new_vector_x;
                    new_segment_dst_y = new_vector_y;
                }
                else
                {
                    new_segment_src_x = new_vector_x;
                    new_segment_src_y = new_vector_y;
                    new_segment_dst_x = graph_src_x;
                    new_segment_dst_y = graph_src_y;
                }
            }
        }
    }
    //
    else
    {
        is_vertical_new_segment = true;
        if (graph_src_y <= new_vector_y)
        {
            new_segment_src_x = graph_src_x;
            new_segment_src_y = graph_src_y;
            new_segment_dst_x = new_vector_x;
            new_segment_dst_y = new_vector_y;
        }
        else
        {
            new_segment_src_x = new_vector_x;
            new_segment_src_y = new_vector_y;
            new_segment_dst_x = graph_src_x;
            new_segment_dst_y = graph_src_y;
        }
    }

    int graph_existing_src_x;
    int graph_existing_src_y;
    int graph_existing_dst_x;
    int graph_existing_dst_y;
    std::string my_key = "";
    for (auto i = a1.myMultimap.begin(); i != a1.myMultimap.end(); ++i)
    {
        std::string currentKey = i->first;
        // auto next = std::next(i);
        if (currentKey != my_key)
        {
            my_key = currentKey;
            auto it = a1.myMultimap.equal_range(my_key);
            // std::cout << "Current key  " << it.first->first << std::endl;
            for (auto itr = it.first; itr != it.second; ++itr)
            {
                bool is_vertical_existing = false;
                if (std::next(itr) != it.second)
                {
                    auto next_it = std::next(itr);
                    int x0 = std::get<0>(itr->second);
                    int y0 = std::get<1>(itr->second);
                    int x1 = std::get<0>(next_it->second);
                    int y1 = std::get<1>(next_it->second);
                    if (x0 == x1)
                    {
                        is_vertical_existing = true;
                        if (y0 <= y1)
                        {
                            graph_existing_src_x = x0;
                            graph_existing_src_y = y0;
                            graph_existing_dst_x = x1;
                            graph_existing_dst_y = y1;
                        }
                        else
                        {
                            graph_existing_src_x = x1;
                            graph_existing_src_y = y1;
                            graph_existing_dst_x = x0;
                            graph_existing_dst_y = y0;
                        }
                        bool if_vertical_overlap = check_two_lines_parallel_and_overlap(graph_existing_src_x, graph_existing_src_y, graph_existing_dst_x, graph_existing_dst_y,
                                                                                        new_segment_src_x, new_segment_src_y, new_segment_dst_x, new_segment_dst_y, is_vertical_new_segment, is_vertical_existing);
                        if (if_vertical_overlap)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if (y0 == y1)
                        {
                            if (x0 <= x1)
                            {
                                graph_existing_src_x = x0;
                                graph_existing_src_y = y0;
                                graph_existing_dst_x = x1;
                                graph_existing_dst_y = y1;
                            }
                            else
                            {
                                graph_existing_src_x = x1;
                                graph_existing_src_y = y1;
                                graph_existing_dst_x = x0;
                                graph_existing_dst_y = y0;
                            }
                            bool if_horizontal_overlap = check_two_lines_parallel_and_overlap(graph_existing_src_x, graph_existing_src_y, graph_existing_dst_x, graph_existing_dst_y,
                                                                                              new_segment_src_x, new_segment_src_y, new_segment_dst_x, new_segment_dst_y, is_vertical_new_segment, is_vertical_existing);
                            if (if_horizontal_overlap)
                            {
                                return true;
                            }
                        }
                        else
                        {
                            if (x0 <= x1)
                            {
                                if (y0 <= y1)
                                {
                                    graph_existing_src_x = x0;
                                    graph_existing_src_y = y0;
                                    graph_existing_dst_x = x1;
                                    graph_existing_dst_y = y1;
                                }
                                else
                                {
                                    graph_existing_src_x = x1;
                                    graph_existing_src_y = y1;
                                    graph_existing_dst_x = x0;
                                    graph_existing_dst_y = y0;
                                }
                            }
                            else
                            {
                                if (y0 <= y1)
                                {
                                    graph_existing_src_x = x0;
                                    graph_existing_src_y = y0;
                                    graph_existing_dst_x = x1;
                                    graph_existing_dst_y = y1;
                                }
                                else
                                {
                                    graph_existing_src_x = x1;
                                    graph_existing_src_y = y1;
                                    graph_existing_dst_x = x0;
                                    graph_existing_dst_y = y0;
                                }
                            }
                            bool if_overlap = check_two_lines_parallel_and_overlap(graph_existing_src_x, graph_existing_src_y, graph_existing_dst_x, graph_existing_dst_y,
                                                                                   new_segment_src_x, new_segment_src_y, new_segment_dst_x, new_segment_dst_y, is_vertical_new_segment, is_vertical_existing);
                            if (if_overlap)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool generate_random_char(std::string &street_name, std::string &error_message)
{
    // std::string error_message;

    char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                         'h', 'i', 'j', 'k', 'l', 'm', 'n',
                         'o', 'p', 'q', 'r', 's', 't', 'u',
                         'v', 'w', 'x', 'y', 'z'};
    char s1;
    int returned_num;
    bool returned = genrate_random_num(&returned_num, 0, 25, error_message);
    if (returned == false)
    {
        return false;
    }
    s1 = alphabet[returned_num];
    street_name.append(1, s1);

    char s2;
    returned = genrate_random_num(&returned_num, 0, 25, error_message);
    if (returned == false)
    {
        return false;
    }
    s2 = alphabet[returned_num];
    street_name += s2;

    char s3;
    returned = genrate_random_num(&returned_num, 0, 25, error_message);
    if (returned == false)
    {
        return false;
    }
    s3 = alphabet[returned_num];
    street_name += s3;

    char s4;
    returned = genrate_random_num(&returned_num, 0, 25, error_message);
    if (returned == false)
    {
        return false;
    }
    s4 = alphabet[returned_num];
    street_name += s4;
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
    int num_streets;
    int num_line_segments;
    int num_wait;
    int x_y_range;
    while ((c = getopt(argc, argv, "s:n:l:c:")) != -1)
    {
        // std::cout << c << std::endl;
        // std::cout << (char)(c) << std::endl;
        if (c == 's')
        {
            svalue = optarg;
            sint_value = atoi(svalue.c_str());
        }
        else if (c == 'l')
        {
            lvalue = optarg;
            lint_value = atoi(lvalue.c_str());
        }
        else if (c == 'n')
        {
            nvalue = optarg;
            nint_value = atoi(nvalue.c_str());
        }
        else if (c == 'c')
        {
            cvalue = optarg;
            cint_value = atoi(cvalue.c_str());
        }
    }
    if (sint_value < 2)
    {
        std::cerr << "Error: s argument less than 2" << std::endl;
        exit(1);
    }
    if (nint_value < 1)
    {
        std::cerr << "Error: n argument less than 1" << std::endl;
        exit(1);
    }
    if (lint_value < 5)
    {
        std::cerr << "Error: l argument less than 5" << std::endl;
        exit(1);
    }
    if (cint_value < 1)
    {
        std::cerr << "Error: c argument less than 1" << std::endl;
        exit(1);
    }
    x_y_range = cint_value;
    // std::cout << "s   " << sint_value << "l   " << lint_value << "c   " << cint_value << "n   " << nint_value << std::endl;
    //  while (1)
    //{
    std::string failed_message;
    bool results = genrate_random_num(&num_streets, 2, sint_value, failed_message);
    if (!results)
    {
        std::cerr << "Error: " << failed_message << std::endl;
    }

    // std::cout << "My num streets " << num_streets << std::endl;
    //  for now for num of line segments is the same
    results = genrate_random_num(&num_line_segments, 1, nint_value, failed_message);
    if (!results)
    {
        std::cerr << "Error: " << failed_message << std::endl;
    }
    // std::cout << "My num line_segments " << num_line_segments << std::endl;
    //  generate wait seconds
    results = genrate_random_num(&num_wait, 5, lint_value, failed_message);
    if (!results)
    {
        std::cerr << "Error: " << failed_message << std::endl;
    }
    // missing code for generating x,y coordinates

    int curr_num_streets = 0;
    int failed_attempt = 0;
    new_graphs a1;
    int curr_num_line_segments = 0;
    // bool failed = false;
    while (1)
    {
        while (curr_num_streets < num_streets)
        {
            std::string street_name;
            std::string error_message;
            bool no_fail = generate_random_char(street_name, error_message);
            if (no_fail == false)
            {
                std::cerr << "Error: " << error_message << std::endl;

                goto end;
            }
            if (a1.myMultimap.empty())
            {
                // a1.myMultimap.push_back(std::string(1, c));
                while (curr_num_line_segments < (num_line_segments + 1))
                {
                    if (failed_attempt == 25)
                    {
                        std::cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << std::endl;
                        // failed = true;
                        exit(1);
                    }
                    int a;
                    int b;
                    results = genrate_random_num(&a, -1 * x_y_range, x_y_range, failed_message);
                    if (results == false)
                    {
                        std::cerr << "Error: " << error_message << std::endl;
                        continue;
                    }
                    results = genrate_random_num(&b, -1 * x_y_range, x_y_range, failed_message);
                    if (results == false)
                    {
                        std::cerr << "Error: " << error_message << std::endl;
                        // goto end_of_while;
                        continue;
                    }
                    std::tuple<int, int> my_tuple = std::make_tuple(a, b);
                    bool if_same_pts = check_if_same_pts(a1, my_tuple, street_name, curr_num_line_segments);
                    if (if_same_pts)
                    {
                        // You should include some code here before using 'goto'
                        failed_attempt++;
                        // goto end_of_while;
                        continue;
                    }
                    if (curr_num_line_segments <= 1)
                    {
                        a1.myMultimap.insert(std::make_pair(street_name, my_tuple));
                        curr_num_line_segments++;
                        continue;
                    }
                    bool if_overlap = check_if_overlap(curr_num_line_segments, a1, my_tuple, street_name);
                    if (if_overlap)
                    {
                        failed_attempt++;
                        goto end_of_while;
                    }
                    // a1.myStreets_Segments.push_back(my_tuple);
                    // bool check_if_overlap(a1, my_tuple);
                    a1.myMultimap.insert(std::make_pair(street_name, my_tuple));
                    curr_num_line_segments++;

                end_of_while:;
                }
                //}

                curr_num_line_segments = 0;
                curr_num_streets++;
            }
            else
            {
                bool repeated = true;
                while (repeated)
                {
                    if (failed_attempt == 25)
                    {
                        std::cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << std::endl;
                        exit(1);
                    }
                    for (auto keyIt = a1.myMultimap.begin(); keyIt != a1.myMultimap.end(); keyIt++)
                    {
                        const std::string key = keyIt->first;
                        if (street_name == key)
                        {
                            repeated = true;
                            generate_random_char(street_name, error_message);
                            failed_attempt++;
                            goto end_of_repeated;
                        }
                    }
                    repeated = false;
                    if (!repeated)
                    {
                        while (curr_num_line_segments < num_line_segments + 1)
                        {
                            if (failed_attempt == 25)
                            {
                                std::cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << std::endl;
                                exit(1);
                            }
                            int a;
                            int b;
                            results = genrate_random_num(&a, -1 * x_y_range, x_y_range, failed_message);
                            if (results == false)
                            {
                                std::cerr << "Error: " << error_message << std::endl;
                                goto end_of_repeated;
                            }
                            results = genrate_random_num(&b, -1 * x_y_range, x_y_range, failed_message);
                            if (results == false)
                            {
                                std::cerr << "Error: " << error_message << std::endl;
                                goto end_of_repeated;
                            }
                            std::tuple<int, int> my_tuple = std::make_tuple(a, b);
                            // a1.myStreets_Segments.push_back(my_tuple);
                            bool if_same_pts = check_if_same_pts(a1, my_tuple, street_name, curr_num_line_segments);
                            if (if_same_pts)
                            {
                                // You should include some code here before using 'goto'
                                failed_attempt++;
                                // goto end_of_repeated;
                                continue;
                            }
                            if (curr_num_line_segments < 1)
                            {
                                a1.myMultimap.insert(std::make_pair(street_name, my_tuple));
                                curr_num_line_segments++;
                                continue;
                            }
                            bool if_overlap = check_if_overlap(curr_num_line_segments, a1, my_tuple, street_name);
                            if (if_overlap)
                            {
                                failed_attempt++;
                                // goto end_of_repeated;
                                continue;
                            }
                            a1.myMultimap.insert(std::make_pair(street_name, my_tuple));
                            curr_num_line_segments++;
                        }
                        curr_num_line_segments = 0;
                    }
                end_of_repeated:;
                }
                curr_num_streets++;
            }
            // curr_num_streets++;
        end:;
        }
        // for ()
        std::string my_key = "";
        for (auto i = a1.myMultimap.begin(); i != a1.myMultimap.end(); ++i)
        {
            std::string currentKey = i->first;
            if (currentKey != my_key)
            {
                my_key = currentKey;
                std::cout << "add "
                          << "\"" << my_key << "\""
                          << " ";
                // std::cerr << "add "
                //           << "\"" << my_key << "\""
                //           << " ";
                auto range = a1.myMultimap.equal_range(my_key);
                for (auto itr = range.first; itr != range.second; ++itr)
                {
                    std::cout << "(" << std::get<0>(itr->second);
                    // std::cerr << "(" << std::get<0>(itr->second);
                    std::cout << "," << std::get<1>(itr->second) << ") ";
                    // std::cerr << "," << std::get<1>(itr->second) << ") ";
                }
                std::cout << std::endl;
                // std::cerr << std::endl;
                // fflush(stdout);
            }
        }
        std::cout << "gg"
                  << std::endl;
        // std::cerr << "gg"
        //           << std::endl;
        // fflush(stdout);
        sleep(num_wait);
        for (auto i = a1.myMultimap.begin(); i != a1.myMultimap.end(); ++i)
        {
            std::string currentKey = i->first;
            if (currentKey != my_key)
            {
                my_key = currentKey;
                std::cout << "rm "
                          << "\"" << my_key << "\""
                          << std::endl;
                // fflush(stdout);
            }
        }
        a1.myMultimap.clear();
        failed_attempt = 0;
        curr_num_streets = 0;
    }
    // a1.print();
    //  }
    // end:
    return 0;
}
