#ifndef PAAL_DURATION_H
#define PAAL_DURATION_H

#include <chrono>
#include <iostream>
#include <string>


template<typename F, typename ...Args>
auto measure_time(std::string const & name, F&& func, Args&&... args) {
    auto start = std::chrono::system_clock::now();
    auto ret = std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    auto duration = std::chrono::duration_cast<std::chrono::seconds>
        (std::chrono::system_clock::now() - start);
    std::cout << name << " time "  << duration.count() << std::endl;
    return std::move(ret);
}
#endif /* PAAL_DURATION_H */
