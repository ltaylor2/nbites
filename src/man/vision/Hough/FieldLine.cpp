#include "FieldLine.h"

namespace man {
namespace vision {

// A fieldline is just a pair of lines, with a recorded
// separation
FieldLine::FieldLine(Line line1_, Line line2_)
	:line1(line1_), line2(line2_) {}

// calculate the separation on the fly
double FieldLine::getSeparation() {
	return -(line1.getR() + line2.getR());
}

}
}