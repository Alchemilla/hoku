/// @file trio.cpp
/// @author Glenn Galvizo
///
/// Source file for Trio class, which retrieves attributes based off of three Stars.

#include <cmath>

#include "math/trio.h"

/// Private constructor. Sets the individual stars.
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param b_3 Star B_3 of the trio.
Trio::Trio (const Star &b_1, const Star &b_2, const Star &b_3) {
    this->b_1 = b_1, this->b_2 = b_2, this->b_3 = b_3;
}

/// Compute the planar side lengths of the triangle.
///
/// @return Side lengths in order a, b, c.
Trio::side_lengths Trio::planar_lengths () const {
    return {(this->b_1 - this->b_2).norm(), (this->b_2 - this->b_3).norm(), (this->b_3 - this->b_1).norm()};
}

/// Compute the spherical side lengths of the triangle.
///
/// @return Side lengths in order a, b, c in **radians**.
Trio::side_lengths Trio::spherical_lengths () const {
    auto compute_length = [] (const Star &beta_1, const Star &beta_2) -> double {
        return acos(Star::dot(beta_1, beta_2) / (beta_1.norm() * beta_2.norm()));
    };
    return {compute_length(b_1, b_2), compute_length(b_2, b_3), compute_length(b_3, b_1)};
}

/// Find the triangle's semi perimeter (perimeter * 0.5) given the side lengths.
///
/// @param a Length from star B_1 to B_2.
/// @param b Length from star B_2 to B_3.
/// @param c Length from star B_3 to B_1.
/// @return The semi perimeter of {B_1, B_2, B_3}.
double Trio::semi_perimeter (const double a, const double b, const double c) {
    return 0.5 * (a + b + c);
}

/// Treat the space between the trio as a planar triangle. Find the area of this triangle using Heron's formula.
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param b_3 Star B_3 of the trio.
/// @return The planar area of {B_1, B_2, B_3}.
double Trio::planar_area (const Star &b_1, const Star &b_2, const Star &b_3) {
    side_lengths ell = Trio(b_1, b_2, b_3).planar_lengths();
    double s = semi_perimeter(ell[0], ell[1], ell[2]);
    
    return sqrt(s * (s - ell[0]) * (s - ell[1]) * (s - ell[2]));
}

/// Treat the space between the trio as a planar triangle. Find the polar moment of this triangle.
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param b_3 Star B_3 of the trio.
/// @return The planar polar moment of {B_1, B_2, B_3}.
double Trio::planar_moment (const Star &b_1, const Star &b_2, const Star &b_3) {
    side_lengths ell = Trio(b_1, b_2, b_3).planar_lengths();
    return planar_area(b_1, b_2, b_3) * (pow(ell[0], 2) + pow(ell[1], 2) + pow(ell[2], 2)) / 36.0;
}

/// The three stars are connected with great arcs facing inward (modeling extended sphere of Earth), forming a
/// spherical triangle. Find the surface area of this. Based on L'Huilier's Formula and using the excess formula
/// defined in terms of the semi-perimeter.
/// https://en.wikipedia.org/wiki/Spherical_trigonometry#Area_and_spherical_excess
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param b_3 Star B_3 of the trio.
/// @return INVALID_TRIO_NEGATIVE_F if f is negative. DUPLICATE_STARS_IN_TRIO if two stars are the same. The spherical 
/// area of {B_1, B_2, B_3} otherwise.
double Trio::spherical_area (const Star &b_1, const Star &b_2, const Star &b_3) {
    side_lengths ell = Trio(b_1, b_2, b_3).spherical_lengths();
    double s = semi_perimeter(ell[0], ell[1], ell[2]);
    
    // If any of the stars are positioned in the same spot, this is a line. There exists no area.
    if (b_1 == b_2 || b_2 == b_3 || b_3 == b_1) {
        return DUPLICATE_STARS_IN_TRIO;
    }
    
    // Determine the inner component of the square root.
    double f = tan(0.5 * s) * tan(0.5 * (s - ell[0]));
    f *= tan(0.5 * (s - ell[1])) * tan(0.5 * (s - ell[2]));
    
    // F should not be negative. If this is the case, then we don't proceed. Return zero.
    if (f < 0) {
        return INVALID_TRIO_NEGATIVE_F;
    }
    
    // Find and return the excess.
    return 4.0 * atan(sqrt(f));
}

/// Determine the centroid of a **planar** triangle formed by the given three stars. It's use is appropriate for the
/// spherical triangles as we only require the angle between these calculated centroids.
///
/// @return Star with the components of {B_1, B_2, B_3} centroid.
Star Trio::planar_centroid () const {
    return {(1 / 3.0) * (this->b_1[0] + this->b_2[0] + this->b_3[0]),
        (1 / 3.0) * (this->b_1[1] + this->b_2[1] + this->b_3[1]),
        (1 / 3.0) * (this->b_1[2] + this->b_2[2] + this->b_3[2])};
}

/// Cut the current triangle into one smaller, retaining the focus coordinate and half the distance from star C_1 &
/// star C_3. If focus is equal to {0, 0, 0}, then find the middle fourth of the triangle.
///
/// @param c_1 New trio has midpoint between keep-c_1 edge.
/// @param c_2 New trio has midpoint between keep-c_2 edge.
/// @param c_3 New trio has midpoint between c_1-c_2 edge, if focus = {0, 0, 0}.
/// @param keep Star to retain with cut. Vector with components {0, 0, 0} by default.
/// @return Cut trio of stars {c_12, c_23, c_31}.
Trio Trio::cut_triangle (const Star &c_1, const Star &c_2, const Star &c_3, const Star &keep) {
    // Keep desired stars. Only include those who are directly related to the midpoint.
    return {(keep == c_3) ? c_3 : (c_1 + c_2).normalize(), (keep == c_2) ? c_2 : (c_1 + c_3).normalize(),
        (keep == c_1) ? c_1 : (c_2 + c_3).normalize()};
}

/// Recursively determine the spherical moment of a trio. This is a divide-and-conquer approach, creating four
/// smaller triangles with every depth increase. Upon reaching our base case, we return the area of this new
/// triangle 'dA' multiplied with the square of the angle between the root trio's centroid and the current
/// centroid = theta^2.
///
/// @param c Centroid of the td_h=0 trio.
/// @param td_n Maximum depth of moment calculation process. Larger td_n = more accurate, slower.
/// @param td_i Current depth of moment calculation.
/// @return The spherical polar moment.
double Trio::recurse_spherical_moment (const Star &c, const int td_n, const int td_i) {
    if (td_n == td_i) {
        double theta = Star::angle_between(c, this->planar_centroid());
        return spherical_area(this->b_1, this->b_2, this->b_3) * pow(theta, 2);
    }
    else {
        // Divide the triangle into four equal parts. Recurse for each of these new triangles.
        Trio t_1 = cut_triangle(this->b_1, this->b_2, this->b_3, this->b_1);
        Trio t_2 = cut_triangle(this->b_1, this->b_2, this->b_3, this->b_2);
        Trio t_3 = cut_triangle(this->b_1, this->b_2, this->b_3, this->b_3);
        Trio t_4 = cut_triangle(this->b_1, this->b_2, this->b_3);
        
        return t_1.recurse_spherical_moment(c, td_n, td_i + 1) + t_2.recurse_spherical_moment(c, td_n, td_i + 1)
               + t_3.recurse_spherical_moment(c, td_n, td_i + 1) + t_4.recurse_spherical_moment(c, td_n, td_i + 1);
    }
}

/// The three stars are connected with great arcs facing inward (modeling extended sphere of Earth), forming a
/// spherical triangle. Find the polar moment of this. This is a wrapper for the recurse_spherical_moment method.
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param b_3 Star B_3 of the trio.
/// @param td_h Maximum depth of moment calculation process. Larger td_h = more accurate, slower.
/// @return The spherical polar moment of {B_1, B_2, B_3}.
double Trio::spherical_moment (const Star &b_1, const Star &b_2, const Star &b_3, const int td_h) {
    Trio t(b_1, b_2, b_3);
    
    // We star at the root, where the current tree depth td_i = 0.
    double t_i = t.recurse_spherical_moment(t.planar_centroid(), td_h, 0);
    
    // Don't return NaN values.
    return (std::isnan(t_i)) ? 0 : t_i;
}

/// Find the angle between two stars b_1 and b_2, using the central star as the new origin.
///
/// @param b_1 Star B_1 of the trio.
/// @param b_2 Star B_2 of the trio.
/// @param central Central star of the trio.
/// @return Angle between b_1 and b_2, with central as the origin.
double Trio::dot_angle (const Star &b_1, const Star &b_2, const Star &central) {
    // Move the origin of b_1 and b_2 to the newly defined center. Normalize these.
    Star b_1c = (b_1 - central).normalize(), b_2c = (b_2 - central).normalize();
    return Star::angle_between(b_1c, b_2c);
}
