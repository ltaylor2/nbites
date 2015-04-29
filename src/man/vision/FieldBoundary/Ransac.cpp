#include "Ransac.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

namespace man {
namespace vision {

Ransac::Ransac(int width_, int height_, int errThr_)
	: width(width_),
	  height(height_),
	  errThr(errThr_)
{
	// initialize bestline struct
	// TODO initializing structs
	bestLine = {0,0,0,0,0};

	// seed the random generator
	srand(time(NULL));

}

void Ransac::populateNewPoints(std::vector<bool> &markers)
{
	allPoints.clear();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (markers[x + width * y]) {

				Point p = {x, y};
				allPoints.push_back(p);
			}
		}
	}
}

BoundaryLine Ransac::bestFitLine(int iterations)
{
	// clears earlier info
	bestLine.score = 0;

	// find the best fit line by picking two random points,
	// scoring them, and keeping track of the best line;
	for (int i = 0; i < iterations; i++) {
		int randOne = rand() % allPoints.size();
		int randTwo = 0;
		do {
			randTwo = rand() % allPoints.size();
		} while (randTwo == randOne);

		Point one = allPoints[randOne];
		Point two = allPoints[randTwo];
		BoundaryLine line = {one, two, 0};
		line.score = rateLine(line);

		if (line.score > bestLine.score) {
			bestLine = line;
		}
	}

	return bestLine;
}

BoundaryLine Ransac::bestFitLine(int iterations, std::vector<Point> &outliers)
{
	// clears earlier info
	bestLine.score = 0;

	// find the best fit line by picking two random points,
	// scoring them, and keeping track of the best line;
	for (int i = 0; i < iterations; i++) {
		int randOne = rand() % allPoints.size();
		int randTwo = 0;
		do {
			randTwo = rand() % allPoints.size();
		} while (randTwo == randOne);

		Point one = allPoints[randOne];
		Point two = allPoints[randTwo];
		BoundaryLine line = {one, two, 0};
		line.score = rateLine(line, outliers);

		if (line.score > bestLine.score) {
			bestLine = line;
		}
	}

	return bestLine;
}

// TODO more carefully look at top line(s)finding
void Ransac::bestTopFitLines(int iterations, std::vector<bool> &markers, std::vector<BoundaryLine> &lines)
{		
	// try finding one line, the best overall line from RANSAC
	populateNewPoints(markers);
	std::vector<Point> outliers;
	BoundaryLine line1 = bestFitLine(iterations, outliers);
	line1.score = (double)(line1.score) / allPoints.size();

	// then store all the outliers that are below the line1 found
	std::vector<Point> downAndOutliers;
	for (unsigned int i = 0; i < outliers.size(); i++) {
		if (isBelowLine(outliers[i], line1)) 
			downAndOutliers.push_back(outliers[i]);
	}

	BoundaryLine line2 = bestFitLine(iterations, downAndOutliers);
	line2.score = (double)(line2.score) / allPoints.size();

	double twoLineAve = (line1.score + line2.score) / 2.0;
	if (line1.score >= twoLineAve) {
		std::cout << "Returning 1 Line" << std::endl;
		lines.push_back(line1);
	}
	else {
		std::cout << "Returning 2 Lines" << std::endl;
		lines.push_back(line1);
		lines.push_back(line2);
	}
}

bool Ransac::isBelowLine(Point point, BoundaryLine line) {
	// this is a hilarious way to do 
	double m = (line.p1.y - line.p2.y) / (line.p1.x - line.p2.x);
	double lineYAtX = m * (point.x - line.p1.x) + line.p1.y;

	return point.y >= lineYAtX;
}

int Ransac::rateLine(BoundaryLine line)
{
	int inliers = 0;
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		if (getDistance(line, allPoints[i]) <= errThr)
			inliers++;
	}	
	
	return inliers;
}

int Ransac::rateLine(BoundaryLine line, std::vector<Point> &outliers)
{
	int inliers = 0;
	for (unsigned int i = 0; i < allPoints.size(); i++) {
		if (getDistance(line, allPoints[i]) <= errThr) {
			inliers++;
		}
		else
			outliers.push_back(allPoints[i]);
	}	
	
	return inliers;
}

double Ransac::getDistance(BoundaryLine line, Point point)
{
	// finds the distance from a point to the line by h = 2A/b
	// of a triangle with vertices at endpoints and point
	double doubleArea = abs((line.p2.y - line.p1.y) * point.x - (line.p2.x - line.p1.x) * point.y
							 + line.p2.x * line.p1.y - line.p2.y * line.p1.x);
	double baseLength = sqrt(pow(line.p2.y - line.p1.y, 2) + pow(line.p2.x - line.p1.x, 2));

	return doubleArea / baseLength;
}

}
}