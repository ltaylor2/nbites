#include "HoughLineModule.h"
#include "Gradient.h"
#include <iostream>

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

// Constructor for the module
HoughLineModule::HoughLineModule()
	: Module(),
	  houghImage(base()),
	  houghLineList(base()),
	  magImage(base()),
	  edgeImage(base()),
	  hs()
{}


// Run the module, getting the y-image from the YUV image
// of the log, building the gradient, and then finding the fieldlines.
// Sends image and line info as OutPortal messages to view back in
// Java and nbtool
void HoughLineModule::run_() {
	imageIn.latch();

	// get the yuv image from the message from the tool
	const YUVImage yuv = imageIn.message();
	portals::Message<messages::YUVImage> message(&yuv);

	// get the y image
	man::image::ImageConverterModule module;
	module.imageIn.setMessage(message);
	module.run();

	// y-image is a pointer to 16-bit ints
	const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();

	std::cout << "Building Gradient" << std::endl;

	// build the gradient from the image
	Gradient gradient(yImage->pixelAddress(0,0), yImage->width(), yImage->height());

	std::cout << "Gradient Built" << std::endl;

	// get the edges with (here) a hardcoded noise threshold
	int noiseThr = 20;
	std::vector<Edge> edges = gradient.getEdges(noiseThr);

	// get the edges for the edge image
	bool *edgeMarkers = gradient.getEdgeMarkers();

	// get the magnitudes for the magnitude image
	int *magnitudes = gradient.getMagnitudes();

	// set the mag image, where brighter pixels have greater magnitude
	HeapPixelBuffer *tempBuffer = new HeapPixelBuffer(320*240*2);
	PackedImage16 magOutput(tempBuffer, 320, 240, 320);

	// first find the highest magnitude, to normalize
	double maxMag = 0;
	for (int y = 0; y < magOutput.height(); y++) {
		for (int x = 0; x < magOutput.width(); x++) {
			if ((double)magnitudes[x + magOutput.width() * y] > maxMag)
				maxMag = magnitudes[x + magOutput.width() * y];
		}
	}

	// now set the pixels in the mag image normalized for the maximum
	for (int y = 0; y < magOutput.height(); y++) {
		for (int x = 0; x < magOutput.width(); x++) {
			*magOutput.pixelAddress(x,y) = (double)magnitudes[x + magOutput.width() * y] / maxMag * 255;
		}
	}
	
	// edge at point ? white : black
	HeapPixelBuffer *newTempBuffer = new HeapPixelBuffer(320*240*2);
    PackedImage16 edgeOutput(newTempBuffer, 320, 240, 320);
    for (int y = 0; y < edgeOutput.height(); y++) {
    	for (int x = 0; x < edgeOutput.width(); x++) {
    		if (edgeMarkers[x + edgeOutput.width() * y]) {
    			*edgeOutput.pixelAddress(x,y) = 255;
    		}
    		else 
    			*edgeOutput.pixelAddress(x,y) = 0;
    	}
    }

	std::cout << "Got Edges" << std::endl;
	std::cout << "Edges found: " << edges.size() << std::endl;
	
	// run the houghspace with the edges from the gradient,
	// getting lines
	hs.run(edges, params);

	std::cout << "Ran HoughSpace" << std::endl;

	// for current testing, just working on getting the lines
	// (rawlines) themselves, not yet succesfully finding fieldlines
	std::vector<FieldLine> fieldLines = hs.getFieldLines();
	std::vector<Line> rawLines = hs.getAllLines();

	messages::Lines* lines = new messages::Lines;

	// fill the out message with lines
	for (int i = 0; i < rawLines.size(); i++) {
		messages::Lines_Line* aLine;

		aLine = lines->add_line();
		aLine->set_radius(rawLines[i].getR());
		aLine->set_angle(rawLines[i].getT());

		// or, when you become smarter, with fieldlines
		// aLine = lines->add_line();
		// aLine->set_radius(fieldLines[i].getLine1().getR());
		// aLine->set_angle(fieldLines[i].getLine1().getT());

		// std::cout << "R:" << fieldLines[i].getLine1().getR() << " T: " << fieldLines[i].getLine1().getT() << std::endl;

		// messages::Lines_Line* bLine;
		// bLine = lines->add_line();
		// bLine->set_radius(fieldLines[i].getLine2().getR());
		// bLine->set_angle(fieldLines[i].getLine2().getT());

		// std::cout << "R:" << fieldLines[i].getLine2().getR() << " T: " << fieldLines[i].getLine2().getT() << std::endl;
	}

	// avoid errors from empty lines
	if (rawLines.size() == 0) {
		messages::Lines_Line* aLine;
		aLine = lines->add_line();
		aLine->set_radius(400);
		aLine->set_angle(0);
	}
		
	std::cout << "Found Field Lines" << std::endl;
	std::cout << "Field Lines num: " << fieldLines.size() << std::endl;

	// set the out messages and end
	houghImage.setMessage(Message<PackedImage16>(yImage));
	houghLineList.setMessage(Message<messages::Lines>(lines));
	magImage.setMessage(Message<PackedImage16>(&magOutput));
	edgeImage.setMessage(Message<PackedImage16>(&edgeOutput));
}

}
}