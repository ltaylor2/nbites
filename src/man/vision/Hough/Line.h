#pragma once

#include "AdjustParams.h"
#include "Edge.h"
#include <vector>

namespace man {
namespace vision {

// **************************
// *                        *
// *         Line           *
// *                        *
// **************************
// a line in polar coords (for 2 degrees of freedom)
class Line {

public:
	Line(int rIndex_, int tIndex_, double r_, double t_, double score_);

	// getters for everything
	double getR() { return r; }
	double getT() { return t; }

	int getRIndex() { return rIndex; }
	int getTIndex() { return tIndex; }

	double getCosT();
	double getSinT();

	double getEnd0() { return end0; }
	double getEnd1() { return end1; }

	int getFieldLineIndex() { return fieldLineIndex; }
	void setFieldLineIndex(int fi) { fieldLineIndex = fi; }

	double getScore() { return score; }
	int getBinaryAngle();

	// check if this line intersects with another
	bool intersect(Line otherLine, double &px, double &py);
	
	// adjust a line to find its line-segment endpoints
	void adjust(std::vector<Edge> &edges, AdjustParams p);

private:
	// the size of the histogram that will be used
	// to find the endpoints of the line
	static const int uHistSize = 401;

	int rIndex, tIndex;		// hs indeces
	double r, t;			// r and t values
	double score;			// line's score used in adjusting
	int fieldLineIndex;		// part of a fieldline? which?

	double end0, end1;		// line segment endpoints
	double uHistogram[uHistSize];	// used to calculate endpoints

	// histogram manipulation
	void uAdd(double u, double w);
	double uBin(int index);
};

}
}