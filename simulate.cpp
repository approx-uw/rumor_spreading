#include "include/alphas_strategies.h"
#include "include/duration.h"
#include "include/graph.h"
#include "include/models.h"

#include <boost/program_options.hpp>

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

bool file_exists(const char *fileName) {
        std::ifstream infile(fileName);
            return infile.good();
}

auto read_graph(boost::interprocess::managed_shared_memory & segment) {
    //Open already created shared memory object.
    auto graph_and_size = segment.find<Graph>("Graph");
    assert(graph_and_size.first != nullptr);

    auto vertices_and_size = segment.find<MyVector>("Vertices");
    assert(vertices_and_size.first != nullptr);

    std::cout << "Number of vertices: "  << vertices_and_size.first->size()
              << ". Vertices with non zero degree: " << graph_and_size.first->size() << std::endl;

    return std::make_pair(graph_and_size.first, vertices_and_size.first);
}

auto run(GetAlpha const & get_alpha, Model & model, size_t iterations, Graph const & graph, MyVector const & vertices, RNG & rng, size_t THRESH) {
    uniform_int_distribution<> vertices_dist(0, vertices.size());
    vector<int> counts(100);
    double res{};
    for (size_t it_cnt = 0; it_cnt < iterations; ++it_cnt) {
        if (it_cnt % 100 == 0) {
            cout << "\r" << it_cnt << flush;
        }
        auto vertex = vertices[vertices_dist(rng)];
        auto val = model.simulate(graph, get_alpha, vertex, rng, THRESH);
        assert(val > 0);
        if (val  > counts.size() - 1) {
            counts.resize(val+1);
        }
        ++counts[val];
        res += val;
    }
    cout << std::endl;

    cout << res / iterations << endl;
    return counts;
}


int main(int const argc, char const * const * const argv) {
    namespace po = boost::program_options;
    po::options_description desc("Simulates SIR in a graph");
    string graph_fname, output, shm_name;
    double alpha{};
    size_t iterations{};
    size_t seed{}, THRESH{};
    string alpha_strategy, model_name;

    desc.add_options()
        ("help,h", "help message")
        ("iterations,n", po::value<size_t>(&iterations)->default_value(100000), "nr of iterations (def 100000)")
        ("shm_name,s", po::value<std::string>(&shm_name)->default_value("SimulationGraphShm"), "shared memory buffer name (the same must be used in the server)")
        ("seed,r", po::value<size_t>(&seed)->default_value(1545354), "def = 1545354")
        ("alpha,a", po::value<double>(&alpha)->default_value(0.001), "def = 0.001")
        ("alpha_strategy,v", po::value<string>(&alpha_strategy)->default_value("const"), "def = const, possible(const, exp, pld, lin)")
        ("model,m", po::value<string>(&model_name)->default_value("one_decision"), "def = one_decision, possible(one_decision, many_decisions)")
        ("threshold,t", po::value<size_t>(&THRESH)->default_value(1e4), "def = 1e4")
        ("output,o", po::value<string>(&output), "output file name");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }
    if (!vm.count("output")) {
        output = "simulation_" + to_string(alpha) + "_" + to_string(iterations)
            + "_" + shm_name + "_seed_" + to_string(seed) + "_" + alpha_strategy + "_" + model_name + "_all_vertices" + ".data";
    }
    if (file_exists(output.c_str())) {
        std::cout << "File '" << output << "' already exists!" << std::endl;
        return EXIT_SUCCESS;
    }


    std::unique_ptr<Model> model;
    std::unique_ptr<GetAlpha> get_alpha;

    if (model_name == "one_decision") {
        model = make_unique<ModelOneDecision>();
    } else if(model_name == "many_decisions") {
        model = make_unique<ModelManyDecisions>();
    } else {
        assert(false);
    }

    if (alpha_strategy == "const") {
        get_alpha = make_unique<GetConstAlpha>(alpha);
    } else if (alpha_strategy == "exp") {
        get_alpha = make_unique<GetExponentAlpha>(alpha);
    } else {
        assert(false);
    }



    RNG rng(seed);
    boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, shm_name.c_str());

    auto graph_and_vertices = measure_time("read_graph", read_graph, segment);
    auto counts = measure_time("simulation", run, *get_alpha, *model, iterations,
                                *graph_and_vertices.first, *graph_and_vertices.second, rng, THRESH);

    ofstream os(output);
    for (size_t i = 0; i < counts.size(); ++i) os << i << " " << counts[i] << "\n";
    std::cout << "File with simulation results:\n" << output << std::endl;


    return EXIT_SUCCESS;
}
