#pragma once

#include <boost/circular_buffer.hpp>
#include <boost/thread/synchronized_value.hpp>
#include <chrono>
#include <cstddef>

namespace prof {

using Duration = std::chrono::duration<double, std::milli>;
using TimePoint = std::chrono::high_resolution_clock::time_point;

/// Returns the current time.
TimePoint now();

/// Returns a sub-millisecond resolution duration between start and end.
Duration duration(const TimePoint& start, const TimePoint& end);

struct Stats {
    double mean;
    double stddev;
    double min;
    double max;
    size_t n;
    size_t total_calls;
};

/// Calculates and stores the moving average over K samples of execution time data
/// for some single unit of code. Threadsafe.
struct UnitExecutionTime {
    UnitExecutionTime();

    /// Adds a sample to the collection, overriding the oldest sample if needed.
    void add_sample(const Duration& dur);

    /// Calculates the mean duration over the `measurement_count()` measurements,
    /// as well as the standard deviation.
    Stats stats() const;

    /// Returns the number of elements the moving average is calculated over.
    size_t measurement_count();

private:
    boost::synchronized_value<boost::circular_buffer<Duration>> m_measurements;
    size_t m_total_calls {};
};

/// Holds profiles for multiple units by name. Threadsafe.
struct UnitProfileCollection {
    /// Adds a sample to the collection, overriding the oldest sample if needed.
    void add_sample(const std::string& unit, const Duration& duration);

    /// Calculates the mean duration over the `measurement_count()` measurements,
    /// as well as the standard deviation.
    Stats stats(const std::string& unit);

    /// Returns the number of elements the moving average is calculated over.
    size_t measurement_count(const std::string& unit);

    /// Returns the stats for all stored units.
    std::unordered_map<std::string, Stats> all_stats();

private:
    boost::synchronized_value<std::unordered_map<std::string, UnitExecutionTime>> m_map;
};

}
