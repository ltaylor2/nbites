#pragma once

#include "AdjustParams.h"
#include "Edge.h"
#include <vector>

namespace man {
namespace vision {

// a line in polar coords (for 2 degrees of freedom)
class Line {
public:
	Line(int rIndex_, int tIndex_, double r_, double t_, double score_);

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

	bool intersect(Line otherLine, double &px, double &py);
	void adjust(std::vector<Edge> &edges, AdjustParams p);

private:
	static const int uHistSize = 401;
	int rIndex, tIndex;
	double r, t;
	double score;
	int fieldLineIndex;

	double end0, end1;
	double uHistogram[uHistSize];

	void uAdd(double u, double w);
	double uBin(int index);
};

}
}