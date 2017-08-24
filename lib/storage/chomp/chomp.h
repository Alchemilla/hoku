/// @file chomp.h
/// @author Glenn Galvizo
///
/// Header file for Chomp class, another class to facilitate the retrieval and storage of various data. The Chomp
/// class is meant to provide more specific functions than Nibble, and also deals with data that does not reside
/// inside a database.

#ifndef HOKU_CHOMP_H
#define HOKU_CHOMP_H

#include <iomanip>
#include <iostream>
#include "nibble.h"

/// The chomp class is used with nearly all identification implementations. Like Nibble, a group of stars are linked
/// with certain attributes. Unlike Nibble, the data structure and methods are extended beyond that of a basic lookup
/// table.
///
/// Like Nibble, the following must hold true:
/// The environment variable HOKU_PROJECT_PATH must point to top level of this project.
/// The file %HOKU_PROJECT_PATH%/data/bsc5.dat must exist.
///
/// @example
/// @code{.cpp}
/// Chomp ch;
///
/// // Create a K-Vector for the "BSC5" table, with future queries focused on the "m" column.
/// ch.select_table("BSC5");
/// ch.create_k_vector("M");
///
/// // Search the "BSC5" table for I and J components of all stars with 2.2 < m <2.3. Expecting 20 floats total.
/// sql_row a = ch.k_vector_query("I, J", 2.2, 2.3, 20);
///
/// // Print the results of the search. a[0][0] = first result's I component. a[1][1] = second result's J component.
/// printf("%f, %f", ch.table_results_at(a, 0, 0), ch.table_results_at(a, 1, 1));
/// @endcode
class Chomp : private Nibble {
    friend class TestChomp;
  
  public:
    // Use Nibble's constructor and several other methods. Treat Chomp like an 'upgrade' to Nibble.
    using Nibble::Nibble;
    using Nibble::table_results_at;
    using Nibble::select_table;
    using Nibble::all_bsc5_stars;
    using Nibble::nearby_stars;
    using Nibble::query_bsc5;
    using Nibble::sql_row;
    
    int create_k_vector (const std::string &);
    sql_row k_vector_query (const std::string &, const std::string &, const double, const double, const unsigned int);
  
  private:
    
    /// Standard machine epsilon for doubles. This represents the smallest possible change in precision.
    const double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon();
  
  private:
    int build_k_vector_table (const std::string &, const double, const double);
};

#endif /* HOKU_CHOMP_H */