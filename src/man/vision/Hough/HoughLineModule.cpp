#include "HoughLineModule.h"
#include "Gradient.h"
#include <iostream>

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

HoughLineModule::HoughLineModule()
	: Module(),
	  houghImage(base()),
	  houghLineList(base()),
	  hs()
{}

void HoughLineModule::run_() {
	imageIn.latch();

	// get the yuv image from the message from the tool
	const YUVImage yuv = imageIn.message();
	portals::Message<messages::YUVImage> message(&yuv);

	// get the y image
	man::image::ImageConverterModule module;

	module.imageIn.setMessage(message);

	module.run();

	const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();

	std::cout << "Building Gradient" << std::endl;

	Gradient gradient(yImage->pixelAddress(0,0), yImage->width(), yImage->height());

	std::cout << "Gradient Built" << std::endl;

	int noiseThr = 20;
	std::vector<Edge> edges = gradient.getEdges(noiseThr);

	std::cout << "Got Edges" << std::endl;
	std::cout << "Edges found: " << edges.size() << std::endl;
	
	hs.run(edges, params);

	std::cout << "Ran HoughSpace" << std::endl;

	std::vector<FieldLine> fieldLines = hs.getFieldLines();

	messages::Lines* lines = new messages::Lines;

	for (int i = 0; i < fieldLines.size(); i++) {
		messages::Lines_Line* aLine;
		aLine = lines->add_line();
		aLine->set_radius(fieldLines[i].getLine1().getR());
		aLine->set_angle(fieldLines[i].getLine1().getT());

		messages::Lines_Line* bLine;
		bLine = lines->add_line();
		bLine->set_radius(fieldLines[i].getLine2().getR());
		bLine->set_angle(fieldLines[i].getLine2().getT());
	}
	std::cout << "Found Field Lines" << std::endl;
	std::cout << "Field Lines num: " << fieldLines.size() << std::endl;

	houghImage.setMessage(Message<PackedImage16>(yImage));
	houghLineList.setMessage(Message<messages::Lines>(lines));
}

}
}