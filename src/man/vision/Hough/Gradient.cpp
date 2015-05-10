#include "Gradient.h"

#include <iostream>
#include <math.h>

namespace man {
namespace vision {

// Construct an empty gradient by specifying only sizes
Gradient::Gradient(int wd, int ht)
	: width(wd), height(ht)
{
	gx = new int[width * height];
	gy = new int[width * height];
	mag = new int[width * height];
}

// gradient from a pointer to 16-bit integers that store
// pixel information. This is how we get the Gradient from
// the y-image output in ImageAquisition (src/image)
Gradient::Gradient(uint16_t *pixels, int wd, int ht)
	: width(wd), height(ht)
{
	gx = new int[width * height];
	gy = new int[width * height];
	mag = new int[width * height];

	//[x][y] in 1d is [x + width*y]
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
		  // get the values from the y-image
          int u = (pixels[(x+1) + width * (y-1)] + 2 * pixels[(x+1) + width * y] + pixels[(x+1) + width * (y+1)]) -
          		  (pixels[(x-1) + width * (y-1)] + 2 * pixels[(x-1) + width * y] + pixels[(x-1) + width * (y+1)]);

          int v = (pixels[(x-1) + width * (y+1)] + 2 * pixels[x + width * (y+1)] + pixels[(x+1) + width * (y+1)]) - 
          		  (pixels[(x-1) + width * (y-1)] + 2 * pixels[x + width * (y-1)] + pixels[(x+1) + width * (y-1)]);

          gx[x + width * y] = u;
          gy[x + width * y] = v;
          mag[x + width * y] = u * u + v * v;
		}
	}
}

// gradient from edges
Gradient::Gradient(int wd, int ht, std::vector<Edge> &edges)
{
	gx = new int[width * height];
	gy = new int[width * height];
	mag = new int[width * height];

	for (unsigned int i = 0; i < edges.size(); i++) {
		Edge e = edges[i];
		int x = e.getX() + width / 2;
		int y = height / 2 - e.getY();

		gx[x + width * y] = e.getGX();
		gy[x + width * y] = e.getGY();
		mag[x + width * y] = e.getMagSquared();
	}
}

int Gradient::getMag(int x, int y)
{
	return (int)sqrt(mag[x + width * y]) >> 2;
}

// TODO make return a pointer to a vector
// get a list of edges from the gradient image
std::vector<Edge> Gradient::getEdges(int noiseThr, uint8_t* gPixels)
{
	// tables that show the directions of pixel neighbors
	int dXNeighbors[] = { 1,  1,  0, -1, -1, -1, 0, 1 };
	int dYNeighbors[] = { 0, -1, -1, -1,  0 , 1, 1, 1 };


	// tables for octant kernels
	// TODO fundamental relationship between direction and kernel values?
	int oct1[] = { 1, 0, -1, 1, 0, -1, 1, 0, -1 };
	int oct2[] = { 0, -1, -1, 1, 0, -1, 1, 1, 0 };
	int oct3[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	int oct4[] = { -1, -1, 0, -1, 0, 1, 0, 1, 1 };
	int oct5[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	int oct6[] = { 0, 1, 1, -1, 0, 1, -1, -1, 0 };
	int oct7[] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
	int oct8[] = { 1, 1, 0, 1, 0, -1, 0, -1, -1 };

	int* octants[] = { oct1, oct2, oct3, oct4, oct5, oct6, oct7, oct8 };
	std::vector<Edge> edges;

	// keep track of where an edge is pushed from ((x,y)), to visualize
	// in an output .txt file
	edgeTrue = new bool[width * height];
	fieldEdgeTrue = new bool[width * height];
	for (int i = 0; i < width * height; i++) {
		edgeTrue[i] = false;
		fieldEdgeTrue[i] = false;
	}

	noiseThr = noiseThr * noiseThr << 4; // adjusted for mag^2 like stored in gradient
	for (int y = 2; y < height - 2; y++) {
		for (int x = 2; x < width - 2; x++) {
			int z = mag[x + width * y];
			if (z >= noiseThr) {
				Edge e(x - width / 2, height / 2 - y, gx[x + width * y], -gy[x + width * y]);
				int a = ((e.getDir8() + 0x10) & 0xFF) >> 5;

				// asymmetric peak test
				if (z > mag[(x + dXNeighbors[a]) + width * (y + dYNeighbors[a])]
					&& z >= mag[(x - dXNeighbors[a]) + width * (y - dYNeighbors[a])]) {
					
					// found an edge!
					edgeTrue[x + width * y] = true;

					// now we have to check if it's in the field
					// do this by checking fetching  pixels directly from the green image
					// and checking them appropriately based on the direction of the edge
					// first find the direction of the edge to the nearest octant

					// get angle between 0 and 2PI
					double edgeAngle = (e.getDir8() + 0.5) * M_PI / 128.0;
					double angle = edgeAngle - floor(edgeAngle / (2 * M_PI)) * 2.0 * M_PI;

					// find octant 
					int octVal = floor(angle * 4 / M_PI);
					// now calc score
					int fieldScore = 0;
					int x = e.getX() + width / 2;
					int y = height / 2 - e.getY();
					for (int r = -1; r < 2; r++) {
						for (int c = -1; c < 2; c++) {
							fieldScore += octants[octVal][(r + 1) + 3 * (c + 1)] * gPixels[(x + r) + width * (y + c)];
						}
					}

					std::cout << octVal << "  " << fieldScore << std::endl;

					if (fieldScore >= ACCEPT_THRESHOLD) {
						edges.push_back(e);
						fieldEdgeTrue[x + width * y] = true;
					}
				}
			}
		}

	}

	return edges;
}


// Clean up the gradient
// TODO learn about destructors and make sure this is correct
Gradient::~Gradient() {
	delete[] gx;
	delete[] gy;
	delete[] mag;
	delete[] edgeTrue;
	delete[] fieldEdgeTrue;
}

}
}
