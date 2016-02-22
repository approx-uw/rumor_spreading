#ifndef MODELS_H
#define MODELS_H

#include "common.h"
#include "alphas_strategies.h"

#include <random>
#include <unordered_set>

struct Model {
    virtual size_t simulate(Graph const & graph, GetAlpha const & get_alpha, int start, RNG & rng, size_t THRESHOLD) = 0;
};

struct ModelOneDecision : public Model {

    void spread(Graph const & graph, int node,
            std::unordered_set<int> & informed,
            std::unordered_set<int> & currently_informed) {
        auto iter = graph.find(node);
        if (iter != graph.end()) {
            for (auto nb : iter->second) {
                //cout << "nb " << nb << endl;
                if(!informed.count(nb)) {
                    currently_informed.insert(nb);
                    informed.insert(nb);
                }
            }
        }
    }

    size_t simulate(Graph const & graph, GetAlpha const & get_alpha, int start, RNG & rng, size_t THRESHOLD) override {
        std::unordered_set<int> newly_informed;
        std::unordered_set<int> informed({start});
        std::unordered_set<int> currently_informed;
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        size_t spreaders_cnt{1};
        spread(graph, start, informed, newly_informed);

        int iter_cnt{};
        while(!newly_informed.empty()) {
            auto current_alpha = get_alpha(iter_cnt);
            iter_cnt++;

            for (auto node : newly_informed) {
                if ( dist(rng) < current_alpha) {
                    ++spreaders_cnt;
                    spread(graph, node, informed, currently_informed);
                }
                if (spreaders_cnt > THRESHOLD) return spreaders_cnt;
                //cout << "spreading from " << node << endl;
            }
            //        std::cout << "CA " << current_alpha << std::endl;
            newly_informed.swap(currently_informed);
            currently_informed.clear();
        }

        return spreaders_cnt;
    }
};

struct ModelManyDecisions : public Model {

    void spread(Graph const & graph, int node,
            std::unordered_set<int> & informed,
            std::unordered_set<int> & currently_informed,
            float alpha, RNG & rng) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        auto iter = graph.find(node);
        if (iter != graph.end()) {
            for (auto nb : iter->second) {
                //cout << "nb " << nb << endl;
                if(!informed.count(nb)) {
                    if ( dist(rng) < alpha) {
                        currently_informed.insert(nb);
                        informed.insert(nb);
                    }
                }
            }
        }
    }

    size_t simulate(Graph const & graph, GetAlpha const & get_alpha, int start, RNG & rng, size_t THRESHOLD) override {
        std::unordered_set<int> newly_informed({start});
        std::unordered_set<int> informed({start});
        std::unordered_set<int> currently_informed;
        size_t spreaders_cnt{};

        int iter_cnt{};
        while(!newly_informed.empty()) {
            auto current_alpha = get_alpha(iter_cnt);
            iter_cnt++;

            spreaders_cnt += newly_informed.size();
            if (spreaders_cnt > THRESHOLD) return spreaders_cnt;
            for (auto node : newly_informed) {
                spread(graph, node, informed, currently_informed, current_alpha, rng);
                //cout << "spreading from " << node << endl;
            }
            //        std::cout << "CA " << current_alpha << std::endl;
            newly_informed.swap(currently_informed);
            currently_informed.clear();
        }

        return spreaders_cnt;
    }
};
#endif /* MODELS_H */
