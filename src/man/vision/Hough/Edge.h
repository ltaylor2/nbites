#pragma once

namespace man {
namespace vision {

// **************************
// *                        *
// *          Edge          *
// *                        *
// **************************
// Edge class stores the aspects of an edge on the image gradient
// Later,these edges are used to find lines in HoughSpace run().
// +y from the center is up, -y is down.
class Edge {

public:
	Edge(int x_, int y_, int gx_, int gy_);

	// getters for all the aspects of an edge
	int getX() { return x; }
	int getY() { return y; }
	int getGX() { return gx; }
	int getGY() { return gy; }
	
	int getMagSquared() { return magSquared; }
	int getMag() { return mag; }

	int getDir() { return dir; }
	int getDir8();

private:
	// int values storing edge specifics
	// gy gx used to calculate gradient direction
	int x, y, gx, gy;
	double dir;

	// mag squared is the value stored in gradient
	int magSquared;
	int mag;
};

}
}
