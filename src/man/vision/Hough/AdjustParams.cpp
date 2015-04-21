#include "AdjustParams.h"

namespace man {
namespace vision {

// Currently, constructor just hardcoded with various
// threshold mins/maxs
AdjustParams::AdjustParams() 
	: lineEndWeight(2.0),
	  angleThr(0.24, 0.24),
	  distThr(0.7, 2.00),
	  magThr(4, 8)
{}

}
}

