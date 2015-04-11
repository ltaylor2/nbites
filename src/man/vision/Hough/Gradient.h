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
// gradient image that holds gradients and squared magnitude,
// following the logic from Bill's C# code, (x,y) comps are computed
// on the fly. Gradients constructed from an image, or from a list
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

	std::vector<Edge> getEdges(int noiseThr);

private:
	int *gx;
	int *gy;
	int *mag;
	int width, height;
};

}
}