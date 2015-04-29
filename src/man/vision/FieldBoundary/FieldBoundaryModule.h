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

	portals::InPortal<messages::YUVImage> imageIn;

	portals::OutPortal<messages::PackedImage8> gImage;
	portals::OutPortal<messages::PackedImage8> fImage;
	portals::OutPortal<messages::BoundaryLines> bLineList;

	void run_();

private:
	static const int FIELD_EDGE_THRESHOLD = 2000;
	static const int RANSAC_ITERATIONS = 5000;
	static const int RANSAC_ERROR_THRESH = 4;

	void findFieldEdges(uint8_t* pixels,
						int width,
						int height,
						std::vector<bool> &top,
						std::vector<bool> &left,
						std::vector<bool> &right);

	void findBoundaryLines(std::vector<BoundaryLine> &lines,
						   int width,
						   int height,
						   std::vector<bool> &top,
						   std::vector<bool> &left,
						   std::vector<bool> &right);

	Point getIntersection(BoundaryLine line1, BoundaryLine line2);
};

}
}
