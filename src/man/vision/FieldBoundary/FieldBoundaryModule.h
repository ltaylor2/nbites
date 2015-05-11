#pragma once

#include "image/ImageConverterModule.h"
#include "VisionDef.h"
#include "VisionField.pb.h"
#include "Ransac.h"

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

class FieldBoundaryModule : public portals::Module {

public:
	FieldBoundaryModule();

	// the input YUV image to the module
	portals::InPortal<messages::YUVImage> imageIn;

	// the green image obtained from ImageConverterModule, or otherwise
	portals::OutPortal<messages::PackedImage8> gImage;

	// the 8-bit image marking the left, right, and top points find in thresholding
	portals::OutPortal<messages::PackedImage8> fImage;

	// the list of boundary lines that mark the left, right, and top boundaries of the field.
	portals::OutPortal<messages::BoundaryLines> bLineList;

	void run_();

private:
	static const int FIELD_EDGE_THRESHOLD = 1000;
	static const int RANSAC_ITERATIONS = 5000;
	static const int RANSAC_ERROR_THRESH = 4;

	// find the points at which a row (from left or from right) or a column (from above) passes
	// a green value threshold in the 8-bit green image. Fills argument vectors for those three
	// separate sets of points, top, left, and above
	void findFieldEdges(uint8_t* pixels,
						int width,
						int height,
						std::vector<bool> &top,
						std::vector<bool> &left,
						std::vector<bool> &right);

	// runs RANSAC on the sets of boundary points found in findFieldEdges. Fills BoundaryLine
	// argument vector with the line-fit results of RANSAC
	void findBoundaryLines(std::vector<BoundaryLine> &lines,
						   int width,
						   int height,
						   std::vector<bool> &top,
						   std::vector<bool> &left,
						   std::vector<bool> &right);

	// computation to get the intersection of two lines
	// returns the point of intersection
	Point getIntersection(BoundaryLine line1, BoundaryLine line2);
};

}
}
