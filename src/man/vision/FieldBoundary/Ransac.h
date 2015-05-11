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
	Ransac(int width_, int height_, int errThr_);

	// populate the member vectors with input points,
	// allows different RANSAC fits with one constructor
	void populateNewPoints(std::vector<bool> &markers);

	// find the best fit line by running ransac with the 
	BoundaryLine bestFitLine(int iterations);
	BoundaryLine bestFitLine(int iterations, std::vector<Point> &outliers);		// this time fill outliers

	// find the best top lines, which can return either one or two by filling input vector
	void bestTopFitLines(int iterations, std::vector<bool> &markers, std::vector<BoundaryLine> &lines);

private:
	// is a given point below the line in the Y image?
	bool isBelowLine(Point point, BoundaryLine line);

	// what are the inliers of the line that determine the score?
	int rateLine(BoundaryLine line);
	int rateLine(BoundaryLine line, std::vector<Point> &outliers);		// this time fill outliers

	// the shortest distance between a point and a line
	double getDistance(BoundaryLine line, Point point);

	int width;
	int height;
	int errThr;

	// all the populated points that will be used to find the bestFitLine 
	std::vector<Point> allPoints;

	BoundaryLine bestLine;
};


}
}