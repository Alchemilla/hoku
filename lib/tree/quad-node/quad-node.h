/// @file quad-node.cpp
/// @author Glenn Galvizo
///
/// Header file for QuadNode class, which represents a node and associated functions for the Mercator quadtree.

#ifndef HOKU_QUAD_NODE_H
#define HOKU_QUAD_NODE_H

#include "mercator.h"
#include "nibble.h"
#include <memory>

/// The QuadNode represents a node for the Mercator quadtree, a structure for spatial indexing. This structure is
/// currently being used by the Planar and Spherical Triangle identification methods.
///
/// @example
/// @code{.cpp}
/// // Construct the quadtree. Project every star in BSC5 to a square of 1000x1000 size.
/// QuadNode q_root = QuadNode::load_tree(1000);
///
/// // Find all nearby stars that are within 15 degrees of a random star (Star::chance()). Expecting 90 stars.
/// for (const Star &s : q_root.nearby_stars(Star::chance(), 15, 90)) {
///     printf("%s", s.str().c_str());
/// }
/// @endcode
class QuadNode : public Mercator {
  private:
    friend class TestQuadNode;
    friend class BaseTest; // Friend to BaseTest for access to '==' operator.
  
  public:
    Star::list nearby_stars (const Star &, const double, const unsigned int);
    
    static QuadNode load_tree (const double);
  
  private:
    /// Precision default for '==' method.
    constexpr static double QUADNODE_EQUALITY_PRECISION_DEFAULT = 0.000000000001;
    
    /// Alias for a list of QuadNodes (STL vector of QuadNodes).
    using list = std::vector<QuadNode>;
    
    /// Alias for edges to QuadNode's children (4-element STL array of shared pointers).
    using child_edges = std::array<std::shared_ptr<QuadNode>, 4>;
  
  private:
    QuadNode (const Star &, const double, const double);
    QuadNode (const double, const double, const double);
    static QuadNode root (const double);
    
    std::string str () const;
    
    bool operator== (const QuadNode &) const;
    
    bool is_terminal_branch ();
    bool is_dead_child (const int) const;
    static child_edges no_children ();
    
    double width_given_angle (const double);
    bool quadrant_intersects_quadrant (const QuadNode &) const;
    
    static QuadNode branch (const QuadNode &, const child_edges & = no_children());
    QuadNode to_child (const int c) const;
    
    QuadNode::list reduce_to_quadrant (const QuadNode::list &, const double);
    bool within_quadrant (const QuadNode &) const;
    child_edges find_quadrant_centers () const;
    
    QuadNode find_quad_leaves (const QuadNode &, const double, const QuadNode::list &);
    Star::list query_quadtree (Nibble &, const QuadNode &, const QuadNode &, Star::list &);
  
  private:
    /// Children of the node itself. Defaults to having no children.
    QuadNode::child_edges children = no_children();
    
    /// Signals if leaf node or not. Defaults to false.
    bool is_green = false;
    
    /// Width of the quadrant represented.
    double w_i = 1;
};

#endif /* HOKU_QUAD_NODE_H */