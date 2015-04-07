#pragma once

namespace man {
namespace vision {

class Edge {
public:
	Edge(int x_, int y_, int gx_, int gy_);

	int getX() { return x; }
	int getY() { return y; }
	int getGX() { return gx; }
	int getGY() { return gy; }
	
	int getMagSquared() { return magSquared; }
	int getMag() { return mag; }

	int getDir() { return dir; }
	int getDir8();

private:
	int x, y, gx, gy;
	double dir;
	double magSquared;
	double mag;
};

}
}
