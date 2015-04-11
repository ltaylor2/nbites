#include "Edge.h"

#include <math.h>

namespace man {
namespace vision {

// Constructor for an edge
// Initializes variable and calculates direction, mag^2, and mag
Edge::Edge(int x_, int y_, int gx_, int gy_)
	: x(x_), y(y_), gx(gx_), gy(gy_)
{
	dir = atan2(gy, gx); // gradient direction in radians
	magSquared = (gx * gx + gy * gy);
	mag = (int)sqrt(magSquared) >> 2;
}

// 8-bit binary gradient direction
int Edge::getDir8()
{

	// & 0xFF ANDS with 8-bit 256 (11111111) leaving
	// last 8 bits of the int
	return (int)(dir / M_PI * 128.0) & 0xFF;
}

}
}