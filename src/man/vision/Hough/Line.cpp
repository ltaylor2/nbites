#include "Line.h"
#include "LineFit.h"
#include "FuzzyLogic.cpp"
#include "Util.cpp"

#include <math.h>
#include <iostream>

namespace man {
namespace vision {

// Constructor, also sets t
Line::Line(int rIndex_, int tIndex_, double r_, double t_, double score_)
	: rIndex(rIndex_), tIndex(tIndex_ & 0xFF), r(r_), score(score_), 
	  fieldLineIndex(-1), end0(-200), end1(200)
{
	double n = floor(t_ / (2.0 * M_PI));
	t = t_ - n * 2.0 * M_PI;
}

// Cos of binary angle
double Line::getCosT() {
	return cos(t);
}

// Sin of binary angle
double Line::getSinT() {
	return sin(t);
}

int Line::getBinaryAngle() {
	return (int)(t / M_PI * 128.0) & 0xFF;
}

// Does this line intersect another line? True if they do intersect
bool Line::intersect(Line otherLine, double &px, double &py) {
	double g;

	double cs1 = cos(t);
	double sn1 = sin(t);
	double cs2 = cos(otherLine.getT());
	double sn2 = sin(otherLine.getT());

	px = sn2 * r - sn1 * otherLine.getR();
	py = -cs2 * r + cs1 * otherLine.getR();
	g = cs1 * sn2 - sn1 * cs2;

	if (g == 0) {
		px = 0;
		py = 0;
		return false;
	}

	px = px / g;
	py = py / g;
	return true;
}

// run adjustment algorithm with edges at an angle similar to the line
void Line::adjust(std::vector<Edge> &edges, AdjustParams p)
{
	LineFit fit = LineFit();

	// init weight histrogram
	for (int i = 0; i < uHistSize; i++) {
		uHistogram[i] = 0;
	}

	int lineAngle = getBinaryAngle();

	// angle range from fuzzy threshold
	int angleLimit = (int)ceil(p.getAngleThreshold().getT1() * 128 / M_PI);
	// consider every edge (double yikes, need edge bins)
	for (unsigned int i = 0; i < edges.size(); i++) {
		Edge e = edges[i];

		if (e.getDir8() >= lineAngle - angleLimit
			&& e.getDir8() <= lineAngle + angleLimit) {

			double angle = Util::diffRadians(e.getDir(), t);
			double x = e.getX();
			double y = e.getY();

			double distance = abs(x * getCosT() + y * getSinT() - r);
			
			// Fuzzy threshold operators
			double w = ((angle < p.getAngleThreshold())
						& (distance < p.getDistanceThreshold())
						& (e.getMag() > p.getMagnitudeThreshold())).get();
			
			// if e passes all the threshold tests
			if (w > 0) {
				// add the correct weight to the line's histogram
				double q = x * getSinT() - y * getCosT();
				uAdd(q, w);
				fit.add(w, x, y);
			}
		}
	}

	// now find the first endpoint with the weights
	double tw = 0;
	for (int i = 0; i < uHistSize; i++) {
		tw += uHistogram[i];
		if (tw >= p.getLineEndWeight()) {
			end0 = uBin(i);
			break;
		}
	}

	// and the second endpoint
	tw = 0;
	for (int i = uHistSize - 1; i >= 0; i--) {
		tw += uHistogram[i];
		if (tw >= p.getLineEndWeight()) {
			end1 = uBin(i);
			break;
		}
	}

	// now adjust R and T w/ line fit
	// TODO understand unit vectors better
	double r = fit.getCenterX() * fit.getSecondPrincipalAxisU() + fit.getCenterY() * fit.getSecondPrincipalAxisV();
	if (getCosT() * fit.getSecondPrincipalAxisU() + getSinT() * fit.getSecondPrincipalAxisV() >= 0) {
		// setUnitVect(fit.getSecondPrincipalAxisU(), fit.getSecondPrincipalAxisV());
	}
	else {
		// setUnitVect(-fit.getSecondPrincipalAxisU(), -fit.getSecondPrincipalAxisV());
		r = -r;
	}

	score = fit.getArea();
}

// add to the ubin with the appropriate weights
void Line::uAdd(double u, double w) 
{
	int i = std::min(std::max((int)floor(u + uHistSize / 2.0), 0), uHistSize - 1);
	uHistogram[i] += w;
}

// get the uHist bin that corresponds to the correct spot along the line
double Line::uBin(int index)
{
	return index - (uHistSize - 1) / 2.0;
}

}
}