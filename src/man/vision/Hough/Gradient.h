#pragma once

#include "Edge.h"
#include "VisionDef.h"
#include "Profiler.h"
#include <iostream>
#include <vector>

namespace man {
namespace vision {

// gradient image that holds gradients and squared magnitude,
// following the logic from Bill's C# code, (x,y) comps are computed
// on the fly
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

	// public for now to find the center of all the edges,
	// definitely fix this

private:
	int *gx;
	int *gy;
	int *mag;
	int width, height;
};

}
}