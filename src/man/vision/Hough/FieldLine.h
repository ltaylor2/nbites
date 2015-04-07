#pragma once

#include "Line.h"

namespace man {
namespace vision {

// a pair of lines that appear to be 
// TODO implement heights, tilts, etc. for camera params
class FieldLine {
public:
	FieldLine(Line line1_, Line line2_);
	double getSeparation();
	Line getLine1() { return line1; }
	Line getLine2() { return line2; }

private:
	Line line1;
	Line line2;
};

}
}
