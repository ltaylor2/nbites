#pragma once

#include "FieldLine.h"
#include "Line.h"
#include "AdjustParams.h"

#include <vector>
#include <math.h>
#include <stdlib.h>

namespace man {
namespace vision {


// **************************
// *                        *
// *       HoughSpace       *
// *                        *
// **************************
// accumulator for performing hough transform from a set of edges to
// lines to fieldline. -128 <= R < 128 pixels, with 8-bit binary angle t
// run() gets you Edges-->Lines-->FieldLines, with smoothing/adjustment
class HoughSpace {

public:
	HoughSpace();
	void run(std::vector<Edge>& edges, AdjustParams p);
	int getR(int x, int y, int t);
	void processEdge(int x, int y, int t0, int t1);

	void smooth();
	void getPeaks();
	void suppress(int rx, int ry);
	void adjust(std::vector<Edge>& edges, AdjustParams p);

	void findFieldLines();
	std::vector<FieldLine> getFieldLines() { return fieldLines; }
	std::vector<Line> getAllLines() { return lines; }
	void clear();

private:
	// quantities for the hough accumulator
	static const int R_SPAN = 320;
	static const int T_SPAN = 256;

	// operating params
	// TODO why 32?
	static const int ACCEPT_THRESHOLD = 24;
	static const int ANGLE_SPREAD  = 3;
	static const int REFINE_STEPS = 2;

	// ignoring these specifications for now
	static constexpr double MIN_HEIGHT = 6.0;
	static constexpr double MAX_HEIGHT = 20.0;
	static constexpr double MIN_TILT = 0.2 * M_PI;
	static constexpr double MAX_TILT = 0.6 * M_PI;
	static const bool HEIGHT_ENABLE = false;
	static const bool TILT_ENABLE = false;

	// the actual vectors of lines and fieldlines
	std::vector<Line> lines;
	std::vector<FieldLine> fieldLines;

	// the hough accumulator,
	// and keep track of peaks for other reasons if we 
	// need to look at them
	int hs[R_SPAN][T_SPAN +1];
	bool peaks[R_SPAN][T_SPAN];

};

}
}