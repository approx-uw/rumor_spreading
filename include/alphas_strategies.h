#ifndef ALPHAS_STRATEGIES_H
#define ALPHAS_STRATEGIES_H

#include <cassert>

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

#endif /* ALPHAS_STRATEGIES_H */
