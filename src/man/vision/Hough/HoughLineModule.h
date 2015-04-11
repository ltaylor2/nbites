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
// **************************
// *                        *
// *     HoughLineModule    *
// *                        *
// **************************
// Used to take an input log from nbtool, call from nbcross
// and then:
// 1. get the output y-image from ImageConverter module (src/image)
// 2. get the gradient, and therefore edge information, from that image
// 3. run a hough line finder (in HoughSpace) on that gradient
// ends with a list of FieldLines, if all goes well.
class HoughLineModule : public portals::Module {

public:
	HoughLineModule();

	// get an image from the camera (Transcriber)
	portals::InPortal<messages::YUVImage> imageIn;

	// output the image, just the y-image that was put in for now
	portals::OutPortal<messages::PackedImage16> houghImage;

	// sends out the line information to draw the lines back in
	// nbtool view
	portals::OutPortal<messages::Lines> houghLineList;

	void run_();

private:
	HoughSpace hs;	// hough accumulator storing [r][t] accumulation information
	AdjustParams params; // constructs parameters for use in finding field lines
};

}
}