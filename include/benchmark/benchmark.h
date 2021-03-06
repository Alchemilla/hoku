#include <utility>

/// @file benchmark.h
/// @author Glenn Galvizo
///
/// Header file for Benchmark class, which generates the input data for star identification testing.

#ifndef HOKU_BENCHMARK_H
#define HOKU_BENCHMARK_H

#include "math/star.h"
#include "math/rotation.h"
#include "storage/chomp.h"

/// @brief Class to represent an image of stars.
class Benchmark {
public:
    class Builder;

    static const double NO_M_BAR;
    static const double NO_FOV;
    static const int NO_N;

    Star operator[] (unsigned int n) const;

    void generate_stars (const std::shared_ptr<Chomp> &ch, int n = NO_N, double m_bar = NO_M_BAR);
    void add_extra_light (unsigned int n);
    void shift_light (unsigned int n, double sigma);
    void remove_light (unsigned int n, double psi);

    std::shared_ptr<Star::list> get_image ();
    std::shared_ptr<Star::list> get_answers ();
    std::shared_ptr<Star::list> get_inertial ();
    Vector3 get_center ();
    double get_fov ();


private:
    void shuffle ();

    Benchmark (const std::shared_ptr<Chomp> &ch, double fov, int n = NO_N, double m_bar = NO_M_BAR);
    Benchmark (const std::shared_ptr<Chomp> &ch, const std::shared_ptr<Star::list> &b, double fov);

private:
    Rotation q_rb = Rotation(0, 0, 0, 0);
    std::shared_ptr<Star::list> b_answers;
    std::shared_ptr<Star::list> b;
    std::shared_ptr<Star::list> r;
    Vector3 center;
    double fov;
};

class Benchmark::Builder {
public:
    Builder &using_chomp (const std::shared_ptr<Chomp> &cho) {
        this->ch = cho;
        return *this;
    }
    Builder &limited_by_n_stars (int num) {
        this->n = num;
        return *this;
    }
    Builder &limited_by_m (double m) {
        this->m_bar = m;
        return *this;
    }
    Builder &limited_by_fov (double num) {
        this->fov = num;
        return *this;
    }
    Builder &using_stars (const std::shared_ptr<Star::list> &b_stars) {
        this->b = b_stars;
        return *this;
    }
    Benchmark build () { return (b == nullptr) ? Benchmark(ch, fov, n, m_bar) : Benchmark(ch, b, fov); }

private:
    std::shared_ptr<Star::list> b = nullptr;
    std::shared_ptr<Chomp> ch;
    double m_bar = NO_M_BAR;
    double fov = NO_FOV;
    int n = NO_N;
};

#endif /* HOKU_BENCHMARK_H */