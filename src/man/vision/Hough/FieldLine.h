#pragma once

#include "Line.h"

namespace man {
namespace vision {

// **************************
// *                        *
// *       FieldLine        *
// *                        *
// **************************
// Fieldlines stores pairs of lines found from Hough line-finder
// and then determined to be matching pairs on the field.
// In HoughSpace, fieldlines are the final step, going:
// Image-->Gradient-->Edges-->Lines-->Fieldlines, with
// findFieldLines() in the HoughSpace

// TODO implement title, camera angle, etc.
class FieldLine {
	
public:
	FieldLine(Line line1_, Line line2_);
	double getSeparation();

	// Getters for each of the lines, which store their
	// Line info separately
	Line getLine1() { return line1; }
	Line getLine2() { return line2; }

private:
	Line line1;
	Line line2;
};

}
}
