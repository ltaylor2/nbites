#include "AdjustParams.h"

namespace man {
namespace vision {

// Currently, constructor just hardcoded with various
// threshold mins/maxs
AdjustParams::AdjustParams() 
	: lineEndWeight(2.0),
	  angleThr(0.15, 0.3),
	  distThr(2.00, 5.00),
	  magThr(4, 8)
{}

}
}

