#include <math.h>
#include <cmath>

namespace man {
namespace vision {

// **************************
// *                        *
// *         Util           *
// *                        *
// **************************
// Helper methods
class Util {
	
public:
	static double diffRadians(double a, double b)
	{
		double d = a-b;
		double n = (int) floor(d / (2 * M_PI) + 0.5);
		return std::abs(d - n * 2 * M_PI);
	}

	static void unitVec(double x, double y, double &u, double &v)
	{
		double g = sqrt(x * x + y * y);
		u = x /g;
		v = y /g;
	}
};

}
}