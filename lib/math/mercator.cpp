/// @file mercator.h
/// @author Glenn Galvizo
///
/// Source file for Mercator class, which represents two-dimensional projections of three-dimensional unit vectors
/// (i.e. Stars).

#define _USE_MATH_DEFINES
#include <cmath>
#include <iomanip>

#include "math/mercator.h"

/// Constructor. Projects the input star by the given width and records the results.
///
/// @param s Star to project and store.
/// @param w_n Width to project with.
Mercator::Mercator (const Star &s, const double w_n) {
    project_star(s, w_n);
}

/// Overloaded constructor. Does not project the given points, but stores the values as-is.
///
/// @param x X coordinate to store.
/// @param y Y coordinate to store.
/// @param w_n Width used to project the given X, Y coordinates.
/// @param label Catalog ID to store.
Mercator::Mercator (const double x, const double y, const double w_n, const int label) {
    this->x = x, this->y = y, this->w_n = w_n, this->label = label;
}

/// Return a point with coordinates (0, 0) and w_n = 0.
///
/// @return (0, 0) point with w_n = 0.
Mercator Mercator::zero () {
    return {0, 0, 0};
}

/// Access method for the x and y components of the star. Overloads the [] operator.
///
/// @param n Index of {x, y to return.
/// @return INVALID_ELEMENT_ACCESSED if n > 1. Otherwise component at index n of {x, y}.
double Mercator::operator[] (const unsigned int n) const {
    return n > 1 ? INVALID_ELEMENT_ACCESSED : std::array<double, 2> {x, y}[n];
}

/// Determine the length of the line that connects points m_1 and m_2.
///
/// @param m_1 Point one to determine the distance from.
/// @param m_2 Point two to determine the distance from.
/// @return Distance between points m_1 and m_2.
double Mercator::distance_between (const Mercator &m_1, const Mercator &m_2) {
    return sqrt(pow(m_2.y - m_1.y, 2) + pow(m_2.x - m_1.x, 2));
}

/// Return all components in the current point as a string object.
///
/// @return String of components in form of (x:y:w_n:hr).
std::string Mercator::str () const {
    std::stringstream components;
    
    // Need to use stream here to set precision.
    components << std::setprecision(std::numeric_limits<double>::digits10 + 1) << std::fixed << "(" << x << ":" << y
               << ":" << w_n << ":" << label << ")";
    return components.str();
}

/// Check if the current point is within the corner defined boundary quadrilateral. Corners are defined as such:
///
/// @code{.cpp}
/// corners[0] ----------------------- corners[1]
///  |-----------------------------------------|
///  |-----------------------------------------|
///  |-----------------------------------------|
///  |-----------------------------------------|
///  |-----------------------------------------|
/// corners[2] ----------------------- corners[3]
/// @endcode
///
/// @param corners Corners of the boundary quadrilateral.
/// @return True if within the box. False otherwise.
bool Mercator::is_within_bounds (const quad &corners) const {
    bool within_left_x = this->x > corners[0].x && this->x > corners[2].x;
    bool within_right_x = this->x < corners[1].x && this->x < corners[3].x;
    bool within_top_y = this->y < corners[0].y && this->y < corners[1].y;
    bool within_bottom_y = this->y > corners[2].y && this->y > corners[3].y;
    
    return within_left_x && within_right_x && within_top_y && within_bottom_y;
}

/// Project the input star by the given width. Converts the star into spherical coordinates, and then projects this
/// point into a square of size w_n*w_n with the center being (0, 0). Original conversion found here:
/// https://stackoverflow.com/a/14457180
///
/// @param s Star to project.
/// @param w_n Width to project with.
void Mercator::project_star (const Star &s, const double w_n) {
    double theta, phi, r = s.norm();
    
    // Determine longitude (theta) and latitude (phi). Convert to degrees.
    theta = asin(s[2] / r) * 180.0 / M_PI;
    phi = atan2(s[1], s[0]) * 180.0 / M_PI;
    
    // Project star onto cylinder. Unravel onto square.
    this->x = ((phi + 180.0) * w_n / 360.0) - w_n / 2;
    this->y = ((0.5 * w_n) - (w_n * log(tan((M_PI / 4) + ((theta * M_PI / 180.0) / 2.0))) / (2 * M_PI))) - w_n / 2;
    
    // Save projection width. Use star's catalog ID.
    this->w_n = w_n, this->label = s.get_label();
}

/// Access method for the catalog ID.
///
/// @return Catalog ID for this point.
int Mercator::get_label () const {
    return label;
}

/// Find the corners of a box using the current point as the center and 'a' as the width.
///
/// @param a Width of the box.
/// @return Quad of corners, in order of top-left, top-right, bottom-left, bottom-right.
Mercator::quad Mercator::find_corners (const double a) const {
    return {Mercator(x - a / 2.0, y + a / 2.0, w_n), Mercator(x + a / 2.0, y + a / 2.0, w_n),
        Mercator(x - a / 2.0, y - a / 2.0, w_n), Mercator(x + a / 2.0, y - a / 2.0, w_n)};
}

/// Given a pixel point (X, Y) in an image and the ratio of pixels to degrees, project the star into 3D
/// Cartesian space. The ratio of pixels to degrees must be passed, and it is assumed that the image itself is a
/// square. Solution found here:
/// https://stackoverflow.com/a/12734509
///
/// @param x X coordinat of the image point, using (0, 0) as the *image* center.
/// @param y Y coordinat of the image point, using (0, 0) as the *image* center.
/// @param dpp Degrees per pixel.
/// @return The given point (X, Y) as a normalized vector in a 3D Cartesian frame.
Star Mercator::transform_point (const double x, const double y, const double dpp) {
    double r = (1 / (dpp / 90));
    double alpha = x / r, delta = 2 * atan(exp(y/r)) - M_PI / 2;
    
    return Star(1.0 * cos(delta) * cos(alpha), 1.0 * cos(delta) * sin(alpha), 1.0 * sin(delta), 0, true);
}

/// Wrapper for 'project_star'. Projects the star with the given width.
///
/// @param s Star to project.
/// @param w_n Width to project with.
/// @return The star S, as a Mercator point in 2D space.
Mercator Mercator::transform_star (const Star &s, const double w_n) {
    return Mercator(s, w_n);
}