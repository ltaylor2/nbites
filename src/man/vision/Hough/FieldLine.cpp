#include "FieldLine.h"

namespace man {
namespace vision {

FieldLine::FieldLine(Line line1_, Line line2_)
	:line1(line1_), line2(line2_) {}

double FieldLine::getSeparation() {
	return -(line1.getR() + line2.getR());
}

}
}