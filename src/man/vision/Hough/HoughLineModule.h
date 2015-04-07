#include "image/ImageConverterModule.h"
#include "HoughSpace.h"
#include "AdjustParams.h"
#include "VisionDef.h"
#include "Profiler.h"
#include "VisionField.pb.h"

#pragma once

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

class HoughLineModule : public portals::Module {

public:
	HoughLineModule();

	// get an image from the camera (Transcriber)
	portals::InPortal<messages::YUVImage> imageIn;

	// output the image with the marked hough lines
	portals::OutPortal<messages::PackedImage16> houghImage;
	portals::OutPortal<messages::Lines> houghLineList;

	void run_();

private:
	HoughSpace hs;
	AdjustParams params;
};

}
}