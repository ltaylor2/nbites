#pragma once

#include "FuzzyLogic.cpp"

namespace man {
namespace vision {

class AdjustParams {

public:
	AdjustParams();
	FuzzyThr getAngleThreshold() { return angleThr; }
	FuzzyThr getDistanceThreshold() { return distThr; }
	FuzzyThr getMagnitudeThreshold() { return magThr; }
	double getLineEndWeight() { return lineEndWeight; }

private:
	double lineEndWeight;
	
	double angleMin, angleMax;
	double distMin, distMax;
	double magMin, magMax;

	FuzzyThr angleThr, distThr, magThr;
};

}
}