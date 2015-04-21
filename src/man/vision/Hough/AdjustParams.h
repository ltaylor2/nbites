#pragma once

#include "FuzzyLogic.cpp"

namespace man {
namespace vision {

// **************************
// *                        *
// *    AdjustParameters    *
// *                        *
// **************************
// Adjustment parameters used in making 
// fuzzy thresholds, in turn used to find field lines in
// the HoughSpace class. Currently everything's just hardcoded
// TODO make this a struct in HoughSpace.
class AdjustParams {

public:
	AdjustParams();
	// Getters that return fuzzyThr objects
	FuzzyThr getAngleThreshold() { return angleThr; }
	FuzzyThr getDistanceThreshold() { return distThr; }
	FuzzyThr getMagnitudeThreshold() { return magThr; }
	double getLineEndWeight() { return lineEndWeight; }

private:
	double lineEndWeight;

	// fuzzies themselves
	FuzzyThr angleThr, distThr, magThr;
};

}
}