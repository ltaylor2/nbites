#include "AdjustParams.h"

namespace man {
namespace vision {

// Currently, constructor just hardcoded with various
// threshold mins/maxs
AdjustParams::AdjustParams() 
	: lineEndWeight(2.0), angleMin(0.15),
	  distMin(2.00), distMax(5.00),
	  magMin(4), magMax(8),
	  angleThr(angleMin, angleMax),
	  distThr(distMin, distMax),
	  magThr(magMin, magMax)
{}

}
}

