#include "include/graph.h"

#include <boost/program_options.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <iostream>
#include <cassert>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <random>
#include <unordered_map>
#include <unordered_set>

using namespace boost::interprocess;

//Remove shared memory on construction and destruction
struct shm_remove
{   std::string shm_name;
    shm_remove(std::string shm_name_) : shm_name(std::move(shm_name_)) {shared_memory_object::remove(shm_name.c_str());}
    ~shm_remove(){ shared_memory_object::remove(shm_name.c_str()); }
};

int main(int const argc, char const * const * const argv)
{
    namespace po = boost::program_options;
    po::options_description desc("reads graph");
    std::string graph_fname, shm_name;

    desc.add_options()
        ("help,h", "help message")
        ("shm_name,s", po::value<std::string>(&shm_name)->default_value("SimulationGraphShm"), "shared memory buffer name (the same must be used in the client)")
        ("graph,g", po::value<std::string>(&graph_fname), "graph format ( two ints in a row: followers -> followed )");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if (!vm.count("graph")) {
        std::cerr << "Error: graph parameter is mandatory!\n:" << desc << std::endl;
        return EXIT_FAILURE;
    }

    auto start = std::chrono::system_clock::now();
    shm_remove remover(shm_name);
    //Create shared memory
    auto memory_size = 30ll * 1024ll * 1024ll * 1024ll; // 30GB
    permissions perm;
    perm.set_unrestricted();
    managed_shared_memory segment(create_only, shm_name.c_str(), memory_size, nullptr, perm);

    //Construct a shared memory hash map.
    //Note that the first parameter is the initial bucket count and
    //after that, the hash function, the equality function and the allocator
    auto *graph= segment.construct<Graph>("Graph")  //object name
        ( 100, boost::hash<int>(), std::equal_to<int>()                  //
          , segment.get_allocator<ValueType>());                         //allocator instance

    std::ifstream input(graph_fname);
    VectorShmemAllocator vec_alloc(segment.get_segment_manager());
    std::unordered_set<int> vertices_set;
    while(input.good()) {
        int a,b;
        input >> a >> b;
        vertices_set.insert(a);
        vertices_set.insert(b);
        auto iter = graph->find(a);
        if(iter == graph->end()) {
            MyVector myvec(1, b, vec_alloc);
            graph->insert(ValueType(a, std::move(myvec)));
        } else {
            iter->second.push_back(b);
        }
    }
    for(auto & a : *graph) a.second.shrink_to_fit();

    std::cout << "Constructing vertices " << vertices_set.size() << std::endl;

    auto *vertices= segment.construct<MyVector>("Vertices")  //object name
        (vertices_set.size(), 0, vec_alloc);

    boost::copy(vertices_set, vertices->begin());

    vertices_set.clear();

    auto after_read = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(after_read  - start);
    std::cout << "Read time " << duration.count() << std::endl;

    std::cerr << "Data read" << std::endl;
    sleep(3600 * 24 * 7 * 8); // eight weeks

    return EXIT_SUCCESS;
}
