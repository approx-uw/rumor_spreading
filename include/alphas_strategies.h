#ifndef PAAL_ALPHAS_STRATEGIES_H
#define PAAL_ALPHAS_STRATEGIES_H

#include <boost/range/irange.hpp>
#include <boost/range/numeric.hpp>

#include <string>

struct GetAlpha {
    virtual float operator()(int /*iteration_nr*/) const = 0;
};

struct GetConstAlpha : public GetAlpha {
    GetConstAlpha(float alpha) : m_alpha(alpha) {}
    float m_alpha;
    float operator()(int /*iteration_nr*/) const override {return m_alpha;}
};

struct GetExponentAlpha: public GetAlpha {
    GetExponentAlpha(float alpha) : m_alpha(alpha), m_current_alpha(1.), m_iteration_of_last_query(-1) {}
    float operator()(int iteration_nr) const override {
        if( iteration_nr == m_iteration_of_last_query + 1) {
            ++m_iteration_of_last_query;
            return (m_current_alpha *= m_alpha);
        } else {
            assert(iteration_nr == 0);
            m_iteration_of_last_query = 0;
            return (m_current_alpha = m_alpha);
        }
    }

    float m_alpha;
    mutable float m_current_alpha;
    mutable int m_iteration_of_last_query;
};

struct GetPowerLawAlpha : public GetAlpha {
    GetPowerLawAlpha(float alpha) : m_alphas(MAX_ITER) {
        for (int i : boost::irange(0, MAX_ITER)) {
            m_alphas[i] = std::pow(i+1, -alpha);
        }

        auto const sum = boost::accumulate(m_alphas, 0.);

        for (auto & alpha : m_alphas) alpha /= sum;
    }

    float operator()(int i) const override {
        //AAAAAAAAAAAAAAAAAAAA!!!!!!!!!ale nahakowane !!!!!!!!!!!!
        assert(i + 1 < MAX_ITER);
        return m_alphas[i + 1];
    }

    int const MAX_ITER = 1000;
    std::vector<double> m_alphas;
};

struct GetLinearAlpha : public GetAlpha {
    GetLinearAlpha(float alpha) : m_alpha(alpha) {}

    float operator()(int const iteration_nr) const override {
        return m_alpha / (iteration_nr + 1); //iteration starts with 0
    }

    float const m_alpha;
};

#endif /* PAAL_ALPHAS_STRATEGIES_H */
