#include "Edge.h"

#include <math.h>

namespace man {
namespace vision {

Edge::Edge(int x_, int y_, int gx_, int gy_)
	: x(x_), y(y_), gx(gx_), gy(gy_)
{
	dir = atan2(gy, gx);
	magSquared = (gx * gx + gy * gy);
	mag = (int)sqrt(magSquared) >> 2;
}

int Edge::getDir8()
{
	return (int)(dir / M_PI * 128.0) & 0xFF;
}

}
}