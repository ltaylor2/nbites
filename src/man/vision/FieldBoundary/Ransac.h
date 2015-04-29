#pragma once

#include <vector>

namespace man {
namespace vision {


struct Point {
	int x;
	int y;
};

struct BoundaryLine {
	Point p1;
	Point p2;
	int score;
};

class Ransac {
public:
	// TODO fix constructor rep
	Ransac(int width_, int height_, int errThr_);

	void populateNewPoints(std::vector<bool> &markers);

	BoundaryLine bestFitLine(int iterations);
	BoundaryLine bestFitLine(int iterations, std::vector<Point> &outliers);

	void bestTopFitLines(int iterations, std::vector<bool> &markers, std::vector<BoundaryLine> &lines);

private:
	bool isBelowLine(Point point, BoundaryLine line);
	int rateLine(BoundaryLine line);
	int rateLine(BoundaryLine line, std::vector<Point> &outliers);
	double getDistance(BoundaryLine line, Point point);

	int width;
	int height;
	int errThr;

	std::vector<Point> allPoints;
	BoundaryLine bestLine;
};


}
}