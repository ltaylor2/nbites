#include "LineFit.h"

#include <math.h>

namespace man {
namespace vision {

LineFit::LineFit()
	: sumW(0), sumX(0), sumY(0), sumXY(0), sumX2(0), sumY2(0), solved(false)
{}

void LineFit::solve()
{
	if (solved)
		return;
    double a = sumW * sumX2 - sumX * sumX;
    double b = sumW * sumY2 - sumY * sumY;
    double c = 2.0 * ((double)sumW * sumXY - (double)sumX * sumY);

    double d = sqrt(c * c + (a - b) * (a - b));

    // no principal moments
    if (d == 0.0) {
        uPAI = vPAI = 0.0;
    }

    else {
        uPAI = sqrt(((a - b) / d + 1) / 2);
        vPAI = sqrt((1 - (a - b) / d) / 2);
        if (c < 0)
            uPAI = -uPAI;
    }
    
    solved = true;
}

void LineFit::add(double w, double x, double y)
{
    sumW += w;
    sumX += w * x;
    sumY += w * y;
    sumX2 += w * x * x;
    sumY2 += w * y * y;
    sumXY += w * x * y;
    solved = false;
}

double LineFit::getSecondPrincipalAxisU() {
	solve();
	return -vPAI;
}

double LineFit::getSecondPrincipalAxisV() {
	solve();
	return uPAI;
}

}
}
