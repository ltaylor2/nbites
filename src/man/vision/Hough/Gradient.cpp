#include "Gradient.h"
#include <iostream>
#include <math.h>

namespace man {
namespace vision {

// gradient from size
Gradient::Gradient(int wd, int ht)
	: width(wd), height(ht)
{
	gx = new int[width * height];
	gy = new int[width * height];
	mag = new int[width * height];
}

// gradient from yImage taken from ImageAquisition
Gradient::Gradient(uint16_t *pixels, int wd, int ht)
	: width(wd), height(ht)
{
	gx = new int[width * height];
	gy = new int[width * height];
	mag = new int[width * height];

	//width * y + x
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {

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
std::vector<Edge> Gradient::getEdges(int noiseThr)
{
	// tables that show the directions of pixel neighbors
	// TODO make sure things work w/ green images
	// TODO understand this edge detection process
	int dXNeighbors[] = { 1, 1, 0, -1, -1, -1, 0, 1};
	int dYNeighbors[] = { 0, -1, -1, -1, 0 , 1, 1, 1};

	std::vector<Edge> edges;

	noiseThr = noiseThr * noiseThr << 4; // adjusted for mag^2 like stored in gradient
	for (int y = 2; y < height - 2; y++) {
		for (int x = 2; x < width - 2; x++) {
			int z = mag[x + width * y];
			if (z >= noiseThr) {
				Edge e(x - width / 2, height / 2 - y, gx[x + width * y], -gy[x + width * y]);
				int a = ((e.getDir8() + 0x10) & 0xFF) >> 5;

				// asymmetric peak test
				if (z > mag[(x + dXNeighbors[a]) + (width * (y + dYNeighbors[a]))]
					&& z >= mag[(x - dXNeighbors[a]) + (width * (y - dYNeighbors[a]))])
					edges.push_back(e);
			}
		}
	}

	return edges;
}

Gradient::~Gradient() {
	delete[] gx;
	delete[] gy;
	delete[] mag;
}

}
}
