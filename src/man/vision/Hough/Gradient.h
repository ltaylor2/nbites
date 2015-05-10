#pragma once

#include "Edge.h"
#include "VisionDef.h"
#include "Profiler.h"
#include <iostream>
#include <vector>

namespace man {
namespace vision {

// **************************
// *                        *
// *        Gradient        *
// *                        *
// **************************
// Vector for each point in an image for direction and magnitude of
// changes in brightness at a point. 
// Gradients constructed from an image, or from a list
// of edges, and store an array of directional information (gx, gy) as
// well as the magnitude of that gradient direction at each index
class Gradient {
	
public:
	// TODO gradient constructor from image
	Gradient(int wd, int ht);
	Gradient(int wd, int ht, std::vector<Edge> &edges);
	Gradient(uint16_t *pixels_, int wd, int ht);

	~Gradient();

	int getWidth() { return width; }
	int getHeight() { return height; }

	int getMag(int x, int y);
	bool* getEdgeMarkers() { return edgeTrue; }
	bool* getFieldEdgeMarkers() { return fieldEdgeTrue; }
	int* getMagnitudes() { return mag; }

	std::vector<Edge> getEdges(int noiseThr, uint8_t* gPixels);

private:
	static const int ACCEPT_THRESHOLD = 120;
	int *gx;
	int *gy;
	int *mag;
	bool *edgeTrue;
	bool *fieldEdgeTrue;
	int width, height;
};

}
}