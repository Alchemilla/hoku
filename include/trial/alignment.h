/// @file alignment.h
/// @author Glenn Galvizo
///
/// Header file for the alignment trials. This holds the namespace of functions that allow us to test various
/// attitude determination methods and log the data.

#ifndef TRIAL_ALIGNMENT_H
#define TRIAL_ALIGNMENT_H

#include "identification/angle.h"
#include "identification/astrometry-net.h"
#include "identification/spherical-triangle.h"
#include "identification/planar-triangle.h"
#include "identification/pyramid.h"

/// Contains constants and functions to test various attitude determination functions.
///
/// @code{.cpp}
/// Testing Spaces:                     match_sigma exists in [epsilon, ..., epsilon + 3e-10 * 20]
///                                     shift_sigma exists in [epsilon, ..., epsilon + 3e-16 * 20]
///
/// Current number of alignment trials: 100 * 2 * (20 + 20*20) = 42000
/// @endcode
namespace Alignment {
    /// Attribute header that corresponds to the log file for all query trials.
    const char *const ATTRIBUTE = "IdentificationMethod,MatchSigma,ShiftSigma,ResultSetSize,SExistence\n";
    
    const double WORKING_FOV = 20; ///< Field of view that all our test stars must be within.
    const int QUERY_SAMPLES = 100; ///< Number of samples to retrieve for each individual trial.
    
    const double MS_MIN = std::numeric_limits<double>::epsilon(); ///< Minimum match sigma (machine epsilon).
    const double MS_STEP = 3e-10; ///< Step to increment match sigma with for each variation.
    const int MS_ITER = 20; ///< Number of match sigma variations.
    
    const double SS_MIN = std::numeric_limits<double>::epsilon(); ///< Minimum shift sigma (machine epsilon).
    const double SS_STEP = 3e-16; ///< Step to increment shift sigma with for each variation.
    const int SS_ITER = 20; ///< Number of shift sigma variations.
}



#endif /* TRIAL_ALIGNMENT_H */
