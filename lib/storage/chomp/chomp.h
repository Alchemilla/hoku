/*
 * @file: chomp.h
 *
 * @brief: Header file for Chomp namespace, which exists to supplement the Nibble namespace and
 * provide more specific functions that facilitate the retrieval and storage of various lookup
 * tables.
 */

#ifndef HOKU_CHOMP_H
#define HOKU_CHOMP_H

#include <iomanip>
#include <iostream>
#include "nibble.h"

// enable exception messages if desired
//#define CHOMP_DISPLAY_EXCEPTION_MESSAGES 1

/*
 * @brief Chomp namespace, which exists to supplement the Nibble namespace and provide more specific
 * functions that facilitate the retrieval and storage of various lookup tables.
 *
 * The chomp namespace is used with nearly all identification implementations. Like Nibble, a group
 * of stars are linked with certain attributes. Unlike Nibble, the data structure and methods are
 * extended beyond that of a basic lookup table.
 */
namespace Chomp {

    // k-vector table name, build k-vector equation, query a table using k-vector method
    int build_k_vector_table(SQLite::Database &, const std::string &, const std::string &,
                             const double, const double);
    int create_k_vector(const std::string &, const std::string &);
    std::vector<double> k_vector_query(SQLite::Database &, const std::string &, const std::string &,
                                       const std::string &, const double, const double,
                                       const unsigned int);

    // standard machine epsilon for doubles, smallest possible change in precision
    const double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon();

    // parse catalog, generate BSC5
    void parse_catalog(SQLite::Database &, std::ifstream &);
    int generate_bsc5_table();

    // generic table insertion method, limit by fov if desired
    int insert_into_table(SQLite::Database &, const std::string &, const std::string &,
                          const std::vector<double> &);

    // load all stars in catalog to array
    std::array<Star, 5029> all_bsc5_stars();

    // find all stars near a given focus
    std::vector<Star> nearby_stars(SQLite::Database &, const Star &, const double,
                                   const unsigned int);
    std::vector<Star> nearby_stars(const Star &, const double, const unsigned int);

    // query BSC5 for i, j, k fields given BSC ID
    Star query_bsc5(SQLite::Database &, const int);
    Star query_bsc5(const int);


    // query a table for specified fields given a constraint, limit results by certain number
    std::vector<double> search_table(const std::string &, const std::string &, const std::string &,
                                     const unsigned int, const int = -1);
    std::vector<double> table_results_at(const std::vector<double> &, const unsigned int,
                                         const int);

    // sort table by specified column and create index
    int polish_table(const std::string &, const std::string &, const std::string &,
                     const std::string &);

    // location of catalog and database, requires definition of HOKU_PROJECT_PATH
    static std::string catalog_location(std::string(std::getenv("HOKU_PROJECT_PATH")) +
                                        "/data/bsc5.dat");
    static std::string database_location = std::string(std::getenv("HOKU_PROJECT_PATH")) +
                                           "/data/nibble.db";

    // read and calculate star components from line
    std::array<double, 6> components_from_line(const std::string &);
}

#endif /* HOKU_CHOMP_H */
