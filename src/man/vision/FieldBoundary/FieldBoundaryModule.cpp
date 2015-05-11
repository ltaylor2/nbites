#include "FieldBoundaryModule.h"

#include <vector>
#include <iostream>

namespace man {
namespace vision {

using namespace messages;
using namespace portals;

// Constructor for the module
FieldBoundaryModule::FieldBoundaryModule()
	: Module(),
	  gImage(base()),
	  fImage(base()),
	  bLineList(base())
{}

void FieldBoundaryModule::run_()
{
	imageIn.latch();

	// get the yuv image from the message from the tool
	const YUVImage yuv = imageIn.message();
	portals::Message<messages::YUVImage> message(&yuv);

	// get the green image
	image::ImageConverterModule module;
	module.imageIn.setMessage(message);
	module.run();

	// green image is a pointer to 8-bit ints
	const messages::PackedImage<unsigned char>* gIn = module.greenImage.getMessage(true).get();

	std::vector<bool> topPoints(gIn->width() * gIn->height(), false);
	std::vector<bool> leftPoints(gIn->width() * gIn->height(), false);
	std::vector<bool> rightPoints(gIn->width() * gIn->height(), false);


	// find the left, right, and top points in the green image where those rows/columns
	// have passed the threshold
	findFieldEdges(gIn->pixelAddress(0,0),
				   gIn->width(),
				   gIn->height(),
				   topPoints,
				   leftPoints,
				   rightPoints);

	// mark the edges for display
	HeapPixelBuffer *newTempBuffer = new HeapPixelBuffer(320 * 240);
	PackedImage8 fieldEdges(newTempBuffer, 320, 240, 320);
	for (int y = 0; y < fieldEdges.height(); y++) {
		for (int x = 0; x < fieldEdges.width(); x++) {
			if(leftPoints[x + fieldEdges.width() * y])
				*fieldEdges.pixelAddress(x,y) = 255;
			else if (rightPoints[x + fieldEdges.width() * y])
				*fieldEdges.pixelAddress(x,y) = 255;
			else if(topPoints[x + fieldEdges.width() * y])
				*fieldEdges.pixelAddress(x,y) = 255;
			else
				*fieldEdges.pixelAddress(x,y) = 0;
		}
	}

	// run RANSAC on the boundary points and get the line
	std::vector<BoundaryLine> lines;
	findBoundaryLines(lines,
					  gIn->width(),
					  gIn->height(),
					  topPoints,
					  leftPoints,
					  rightPoints);

	// add and send the field boundary lines message to display the RANSAC lines
	messages::BoundaryLines* boundaryLines = new messages::BoundaryLines;
	for (int i = 0; i < lines.size(); i++) {
		messages::BoundaryLines_Line* aLine;
		aLine = boundaryLines->add_line();
		aLine->set_x1(lines[i].p1.x);
		aLine->set_y1(lines[i].p1.y);
		aLine->set_x2(lines[i].p2.x);
		aLine->set_y2(lines[i].p2.y);
	}

	gImage.setMessage(Message<PackedImage8>(gIn));
	fImage.setMessage(Message<PackedImage8>(&fieldEdges));
	bLineList.setMessage(Message<BoundaryLines>(boundaryLines));
}

void FieldBoundaryModule::findFieldEdges(uint8_t* pixels,
										 int width,
										 int height,
										 std::vector<bool> &top, 
										 std::vector<bool> &left,
										 std::vector<bool> &right)
{

	std::cout << "Finding field edges" << std::endl;

	// run through the image and mark when the threshold is passed
	// above, left, and right
	std::vector<int> yBins(width, 0);
	std::vector<bool> yEdgeTrue(width, 0);

	// run through the image by row in the inner loop,
	// keep track of the values in the rows and bins for columns
	for (int y = 0; y < height; y++) {
		int lw = 0;
		int rw = 0;
		bool leftFound = false;
		bool rightFound = false;
		for (int x = 0; x < width; x++) {

			// if there's no appropriate edge left
			if (!yEdgeTrue[x]) {
				yBins[x] += pixels[x + width * y];

				// mark the point as a boundary if it passes the threshold
				if(yBins[x] > FIELD_EDGE_THRESHOLD) {
					top[x + width * y] = true;
					yEdgeTrue[x] = true;
				}
			}

			// if there's no appropriate edge left
			if (!leftFound) {
				lw += pixels[x + width * y];

				// mark
				if (lw >= FIELD_EDGE_THRESHOLD) {
					left[x + width * y] = true;
					leftFound = true;
				}
			}

			// and same fo rright
			if (!rightFound) {
				rw += pixels[(width - x) + width * y];
				
				if (rw >= FIELD_EDGE_THRESHOLD) {
					right[(width - x) + width * y] = true;
					rightFound = true;
				}
			}
		}
	}
}

// TODO find more rigorous error and iteration rates
void FieldBoundaryModule::findBoundaryLines(std::vector<BoundaryLine> &lines,
					   int width,
					   int height,
					   std::vector<bool> &top,
					   std::vector<bool> &left,
					   std::vector<bool> &right)
{
	std::vector<BoundaryLine> topLines;


	Ransac lineFit(width, height, RANSAC_ERROR_THRESH);

	// fit the top lines
	// NOTE don't populate points because topLines works differently, in case
	// we want two or more lines later
	lineFit.bestTopFitLines(RANSAC_ITERATIONS, top, topLines);

	// lineFit.populateNewPoints(top);
	// BoundaryLine topLine = lineFit.bestFitLine(RANSAC_ITERATIONS);

	// find the left and the right boundary lines from RANSAC on those respective 
	// sets of points
	lineFit.populateNewPoints(left);
	BoundaryLine leftLine = lineFit.bestFitLine(RANSAC_ITERATIONS);

	lineFit.populateNewPoints(right);
	BoundaryLine rightLine = lineFit.bestFitLine(RANSAC_ITERATIONS);

	// A line to represent the bottom of the image, where we assume
	// the field to be
	Point bot1 = { 0 , height };
	Point bot2 = { width, height };
	BoundaryLine botLine = { bot1, bot2, 0 };
	
	if (topLines.size() == 1) {
		// now get the intersections of top/left, top/right, left/bottom, right/bottom
		Point tL = getIntersection(topLines[0], leftLine);
		Point tR = getIntersection(topLines[0], rightLine);
		Point bL = getIntersection(botLine, leftLine);
		Point bR = getIntersection(botLine, rightLine);

		BoundaryLine nLeft = { bL, tL, 0 };
		BoundaryLine nRight = { bR, tR, 0 };
		BoundaryLine nTop = { tL, tR, 0 };

		lines.push_back(nLeft);
		lines.push_back(nRight);
		lines.push_back(nTop);
	}

	// if there are two lines, then do the intersections appropriately
	else if (topLines.size() == 2) {
		std::cout << "Two top lines!" << std::endl;
		BoundaryLine topLeft = topLines[0];
		BoundaryLine topRight = topLines[1];

		if (std::min(topLines[1].p1.x, topLines[1].p2.x) < std::min(topLines[0].p1.x, topLines[0].p2.x)) {
			topLeft = topLines[1];
			topRight = topLines[0];
		}

		Point tL = getIntersection(topLeft, leftLine);
		Point mid = getIntersection(topLeft, topRight);
		Point tR = getIntersection(topRight, rightLine);
		Point bL = getIntersection(botLine, leftLine);
		Point bR = getIntersection(botLine, rightLine);

		BoundaryLine nLeft = { bL, tL, 0 };
		BoundaryLine nRight = { bR, tR, 0 };
		BoundaryLine nLTop = { tL, mid, 0 };
		BoundaryLine nRTop = { mid, tR, 0 };

		lines.push_back(nLeft);
		lines.push_back(nRight);
		lines.push_back(topLines[0]);
		lines.push_back(topLines[1]);
	}
}

// thanks, dan
Point FieldBoundaryModule::getIntersection(BoundaryLine line1, BoundaryLine line2)
{
	// calculations to find the point at which a line intersects another line
	double denom = (line1.p1.x - line1.p2.x) * (line2.p1.y - line2.p2.y)
					- (line1.p1.y - line1.p2.y) * (line2.p1.x - line2.p2.x);

	double xNum = (line1.p1.x * line1.p2.y - line1.p1.y * line1.p2.x) * (line2.p1.x - line2.p2.x)
				   - (line2.p1.x * line2.p2.y - line2.p1.y * line2.p2.x) * (line1.p1.x - line1.p2.x);

	double yNum = (line1.p1.x * line1.p2.y - line1.p1.y * line1.p2.x) * (line2.p1.y - line2.p2.y)
				   - (line2.p1.x * line2.p2.y - line2.p1.y * line2.p2.x) * (line1.p1.y - line1.p2.y);

	Point p = { (int)(xNum / denom), (int)(yNum / denom)};

	return p;			   
}
}
}