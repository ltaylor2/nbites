#include "HoughLineModule.h"
#include "Gradient.h"
#include "vision/FieldBoundary/FieldBoundaryModule.h"

#include <iostream>
#include <fstream>
#include <dirent.h>

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

// Constructor for the module
HoughLineModule::HoughLineModule()
	: Module(),
	  houghImage(base()),
	  houghLineList(base()),
	  edgeImage(base()),
	  fieldEdgeImage(base()),
	  greenImage(base()),
	  fieldPointImage(base()),
	  bLineList(base()),
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
	const messages::PackedImage<unsigned char>* gImage = module.greenImage.getMessage(true).get();

	std::cout << "Building Gradient" << std::endl;

	// build the gradient from the image
	Gradient gradient(yImage->pixelAddress(0,0), yImage->width(), yImage->height());

	std::cout << "Gradient Built" << std::endl;

	// get the edges with (here) a hardcoded noise threshold
	int noiseThr = 17;
	std::vector<Edge> edges = gradient.getEdges(noiseThr, gImage->pixelAddress(0,0));

	// get the magnitudes for the magnitude image
	// int *magnitudes = gradient.getMagnitudes();

	// // set the mag image, where brighter pixels have greater magnitude
	// HeapPixelBuffer *tempBuffer = new HeapPixelBuffer(320*240*2);
	// PackedImage16 magOutput(tempBuffer, 320, 240, 320);

	// // first find the highest magnitude, to normalize
	// int maxMag = 0;
	// for (int y = 0; y < magOutput.height() - 1; y++) {
	// 	for (int x = 0; x < magOutput.width() - 1; x++) {
	// 		if (magnitudes[x + magOutput.width() * y] > maxMag)
	// 			maxMag = magnitudes[x + magOutput.width() * y];
	// 	}
	// }

	// // now set the pixels in the mag image normalized for the maximum
	// // modded for color error?
	// for (int y = 0; y < magOutput.height(); y++) {
	// 	for (int x = 0; x < magOutput.width(); x++) {
	// 		*magOutput.pixelAddress(x,y) = (int)((double)magnitudes[x + magOutput.width() * y] / maxMag * 255) % 255;
	// 	}
	// }
	
	// get the edges for the edge image
	bool *edgeMarkers = gradient.getEdgeMarkers();
	// edge at point ? white : black
	HeapPixelBuffer *tempBuffer = new HeapPixelBuffer(320*240*2);
    PackedImage16 edgeOutput(tempBuffer, 320, 240, 320);
    for (int y = 0; y < edgeOutput.height(); y++) {
    	for (int x = 0; x < edgeOutput.width(); x++) {
    		if (edgeMarkers[x + edgeOutput.width() * y]) {
    			*edgeOutput.pixelAddress(x,y) = 511;
    		}
    		else 
    			*edgeOutput.pixelAddress(x,y) = 0;
    	}
    }

    bool *fieldEdgeMarkers = gradient.getFieldEdgeMarkers();
    // get the edges on the field
    HeapPixelBuffer *newTempBuffer = new HeapPixelBuffer(320*240*2);
    PackedImage16 fieldEdgeOutput(newTempBuffer, 320, 240, 320);
    for (int y = 0; y < fieldEdgeOutput.height(); y++) {
    	for (int x = 0; x < fieldEdgeOutput.width(); x++) {
    		if (fieldEdgeMarkers[x + fieldEdgeOutput.width() * y]) {
    			*fieldEdgeOutput.pixelAddress(x,y) = 511;
    		}
    		else 
    			*fieldEdgeOutput.pixelAddress(x,y) = 0;
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
		aLine->set_end0(rawLines[i].getEnd0());
		aLine->set_end1(rawLines[i].getEnd1());

		// std::cout << "Line " << i << " 1: " << rawLines[i].getEnd0() << " 2: " << rawLines[i].getEnd1() << std::endl;
		// aLine = lines->add_line();
		// aLine->set_radius(fieldLines[i].getLine1().getR());
		// aLine->set_angle(fieldLines[i].getLine1().getT());
		// aLine->set_end0(fieldLines[i].getLine1().getEnd0());
		// aLine->set_end1(fieldLines[i].getLine1().getEnd1());

		// std::cout << "Pair " << i << " Line 1 with R:" << fieldLines[i].getLine1().getR() << " T: " << fieldLines[i].getLine1().getT() << std::endl;

		// messages::Lines_Line* bLine;
		// bLine = lines->add_line();
		// bLine->set_radius(fieldLines[i].getLine2().getR());
		// bLine->set_angle(fieldLines[i].getLine2().getT());
		// bLine->set_end0(fieldLines[i].getLine2().getEnd0());
		// bLine->set_end1(fieldLines[i].getLine2().getEnd1());

		// std::cout << "Pair " << i << " Line 2 with R:" << fieldLines[i].getLine2().getR() << " T: " << fieldLines[i].getLine2().getT() << std::endl;
	}

	// avoid errors from empty lines
	if (rawLines.size() == 0) {
		messages::Lines_Line* aLine;
		aLine = lines->add_line();
		aLine->set_radius(0);
		aLine->set_angle(0);
		aLine->set_end0(-50);
		aLine->set_end1(50);
	}
		
	std::cout << "Found Field Lines" << std::endl;
	std::cout << "Field Lines num: " << fieldLines.size() << std::endl;

	// set the final out messages and end
	houghImage.setMessage(Message<PackedImage16>(yImage));
	houghLineList.setMessage(Message<messages::Lines>(lines));
	edgeImage.setMessage(Message<PackedImage16>(&edgeOutput));
	fieldEdgeImage.setMessage(Message<PackedImage16>(&fieldEdgeOutput));

	// now, for easy display and comparison to edges fetched directly from the green image
	// NOTE using the FieldBoundaryModule runs some stuff twice, like image aquisition and color
	// level stuff, but this shouldn't be being used for anything that takes speed into consideration anyways
	// this is just for independent study display purposes, basically.
	FieldBoundaryModule fModule;
	fModule.imageIn.setMessage(message);
	fModule.run();

	greenImage.setMessage(Message<PackedImage8>(fModule.gImage.getMessage(true).get()));
	fieldPointImage.setMessage(Message<PackedImage8>(fModule.fImage.getMessage(true).get()));
	bLineList.setMessage(Message<BoundaryLines>(fModule.bLineList.getMessage(true).get()));
}

}
}