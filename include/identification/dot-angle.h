/// @file dot-angle.h
/// @author Glenn Galvizo
///
/// Header file for DotAngle class, which matches a set of body vectors (stars) to their inertial counter-parts in
/// the database.

#ifndef HOKU_DOT_ANGLE_H
#define HOKU_DOT_ANGLE_H

#include "benchmark/benchmark.h"
#include "identification.h"

/// @brief Star identification class using dot product angles.
///
/// The angle class is an implementation of Liebe's Dot Angle method for alignment determination. This is one of
/// the six star identification procedures being tested.
///
/// @example
/// @code{.cpp}
/// // Find all stars around a random star within 7.5 degrees of it.
/// // Rotate all stars by same random rotation.
/// Benchmark b(15, Star::chance(), Rotation::chance());
///
/// // Append 2 extra stars to the data-set above.
/// b.add_extra_light(2);
///
/// // Determine an identification. 'A' contains the body set with catalog label attached.
/// Star::list a = Dot(b, Dot::DEFAULT_PARAMETERS).identify();
/// for (const Star s : a) { std::cout << s.str(); << std::endl; }
///
/// // Extract an attitude instead of an identification.
/// Rotation q = Dot(b, Dot::DEFAULT_PARAMETERS).align();
/// @endcode
class DotAngle : public Identification {
public:
    explicit DotAngle (const Benchmark &input, const Parameters &p);

    std::vector<Identification::labels_list> query (const Star::list &s) override;

    stars_either reduce () override;

    stars_either identify () override;

    static int generate_table (INIReader &cf);

    static const int NO_CANDIDATE_TRIO_FOUND_EITHER;
    static const int NO_CANDIDATES_FOUND_EITHER;

    static const unsigned int QUERY_STAR_SET_SIZE;

#if !defined ENABLE_TESTING_ACCESS
    private:
#endif
    // For errors when querying for a star trio, we define an "either" struct.
    struct trios_either {
        Star::trio result; // Result associated with the computation.
        int error = 0; // Error associated with the computation.
    };

    Star::trio find_closest (const Star &b_i);

    labels_either query_for_trio (double theta_1, double theta_2, double phi);

    trios_either find_candidate_trio (const Star &b_i, const Star &b_j, const Star &b_c);
};

/// Alias for the DotAngle class. 'Dot' distinguishes the process I am testing here enough from the 5 other methods.
typedef DotAngle Dot;

#endif /* HOKU_DOT_ANGLE_H */