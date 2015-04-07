#include "HoughSpace.h"
#include "Util.cpp"
#include "AdjustParams.h"
#include <iostream>

namespace man {
namespace vision {

HoughSpace::HoughSpace()
	: lines(), fieldLines()
{}

// complete hough transform on an edge list, places lines in an vector of lines
void HoughSpace::run(std::vector<Edge>& edges, AdjustParams p)
{
	clear();

	for (unsigned int i = 0; i < edges.size(); i++) {
		int t = edges[i].getDir8();
		processEdge(edges[i].getX(), edges[i].getY(), t - ANGLE_SPREAD, t + ANGLE_SPREAD);
	}

	smooth();

	getPeaks();

	std::cout << "Processed Lines: " << lines.size() << std::endl;

	// TODO DEFINITELY change the edges to something that will
	// store dynamic dimensions
	// suppress(edges.getCenterX(), edges.getCenterY());

	suppress(320, 240);

	std::cout << "Post-Suppression Lines: " << lines.size() << std::endl;

	// TODO, array of params to change refinement
	for (int i = 0; i < REFINE_STEPS; i++) {
		adjust(edges, p);
	}

	std::cout << "Post-Adjusted Lines: " << lines.size() << std::endl;

	findFieldLines();
}

// R coord in Hough space for a boundary point with direction t
// SLOW
double HoughSpace::getR(int x, int y, int t) {
	double a = (t & 0xFF) * M_PI / 128.0;
	return (int)floor(x * cos(a) + y * sin(a));
}

// Hough processing on boundary point (x,y) with direction t0 <= t <= t1
void HoughSpace::processEdge(int x, int y, int t0, int t1)
{
	int r0 = getR(x, y, t0);
	for (int t = t0; t <= t1; t++) {
		int t8 = t & 0xFF;
		int r1 = getR(x, y, t8 + 1);

		for (int r = std::min(r0, r1); r <= std::max(r0, r1); r++) {
			int ri = r + R_SPAN /2;
			if (0 <= ri && ri < R_SPAN)
				++hs[ri][t8];
		}

		r0 = r1;
	}
}

// 2x2 boxcar kernel
void HoughSpace::smooth() 
{
	// copy t = 0 to t = T_SPAN for when t = 0 is overwritten before getting to t = T_SPAN - 1
	for (int r = 0; r < R_SPAN; r++) {
		hs[r][T_SPAN] = hs[r][0];
	}

	// 2x2 boxcar smoothing
	for (int t = 0; t < T_SPAN; t++) {
		for (int r = 0; r < R_SPAN - 1; r++) {
			hs[r][t] = hs[r][t] + hs[r + 1][t] + hs[r][t + 1] + hs[r + 1][t+1];
		}
	}
}

// find Hough space peaks, add new lines
// a peak is a point of at least ACCEPT_THRESHOLD that's asymmetrically greater than 8 neighbors
// false peaks are suppressed in suppress()
void HoughSpace::getPeaks()
{
	int numNeighbors = 10;
	// neighbors
	// TODO why 10?
	int dRNeighbors[] = { 1, 1, 0, -1, -2, -2, -2, -1, 0, 1};
	int dTNeighbors[] = { 0, 1, 1, 1, 1, 0, -1, -2, -2 , -2};

	int peakPoints = numNeighbors;
	int threshold = 4 * ACCEPT_THRESHOLD; // smoothing w/ gain 4 (?)

	for (int t = 0; t < T_SPAN; t++) {
		for (int r =2; r < R_SPAN - 2; r++)
		{
			// if there's a peak, get its score, make sure it passes the threshold,
			// and make sure it's better than all of its neighbors
			bool isPeak = true;
			int z = hs[r][t];
			if (z >= threshold) {
				for (int i = 0; i < peakPoints; i++) {
					if (!(z > hs[r + dRNeighbors[i]][(t + dTNeighbors[i]) & 0xFF] 
						  && z >= hs[r - dRNeighbors[i]][(t - dTNeighbors[i]) & 0xFF]))
						isPeak = false;
				}
				if (isPeak) {
					peaks[r][t] = true;
					lines.push_back(Line(r, t,
									   r - R_SPAN / 2 + 0.5,
									   (t + 0.5) * M_PI / 128.0, 
									   z >> 2));
				}

			}
		}
	}
}

// removes lines that can't be removed from neighborhood based detection.
// checks if lines intersect w/ a low angle spread between rx and ry (in the actual source image)
// if they do, throw out the one with the lower accumulator score
// VERY SLOW (O(n^2)), runs through every pair of lines
void HoughSpace::suppress(int rx, int ry)
{
	// init line marking array
	bool markForDelete[lines.size()];
	for (unsigned int i = 0; i < lines.size(); i++) {
		markForDelete[i] = false;
	}

	// TODO redo with AngleBins
	for (unsigned int i = 0; i < lines.size() - 1; i++) {
		for (unsigned int j = i + 1; j < lines.size(); j++) {
			double px, py;

			//do they intersect?
			bool intersects = lines[i].intersect(lines[j], px, py);

			// get the angle difference
			int ijAngle = abs((lines[i].getTIndex() - lines[j].getTIndex()) & 0xFF) << 24 >> 24; // shifting left 24 then right 24?
			
			// check intersection w/ angle criteria in the image frame
			if (0 < ijAngle && ijAngle <= ANGLE_SPREAD
				&& intersects && abs(px) <= rx && abs(py) <= ry) {

				// throw out the one with the lower score
				double iLineScore = lines[i].getScore();
				double jLineScore = lines[j].getScore();

				if (iLineScore < jLineScore)
					markForDelete[i] = true;
				else if (jLineScore < iLineScore) 
					markForDelete[j] = true;
			}
		}
	}

	// then actually delete the marked lines by iterating through all the lines
	int n = 0;
	for (unsigned int i = 0; i < lines.size(); i++) {
		if (markForDelete[i]) {
			lines.erase(lines.begin() + n);
		}
		else
			n++;
	}
}

void HoughSpace::adjust(std::vector<Edge>& edges, AdjustParams p)
{
	unsigned int i = 0;
	while (i < lines.size()) {
		lines[i].adjust(edges, p);
		if (lines[i].getScore() < ACCEPT_THRESHOLD)
			lines.erase(lines.begin() + i);
		else
			i++;
	}
}

// find all the field lines in the image
void HoughSpace::findFieldLines()
{
	// get rid of the current field lines
	fieldLines.clear();
	for (int i = 0; i < lines.size(); i++) {
		lines[i].setFieldLineIndex(-1);
	}

	// consider every line extremely naively
	// TODO implement angle bins etc (same issues w/ EdgeList)
	for (unsigned int i = 0; i < lines.size(); i++) {
		Line line1 = lines[i];

		for (unsigned int j = 0; j < lines.size(); j++) {

			// now consider all opposite gradient lines (which are lines w/ an 
			// 0x80 offset) within a +0x10 range.
			if (lines[j].getBinaryAngle() >= (lines[i].getBinaryAngle() + 0x80)
				&& lines[j].getBinaryAngle() <= (lines[i].getBinaryAngle() + 0x90)) {
				
				Line line2 = lines[j];

				// check gradient, where cos(0x10) is about 0.92)
				// TODO understand this
				if (line1.getCosT() * line2.getCosT() + line1.getSinT() * line2.getSinT() < -0.92) {

					FieldLine testLine(line1, line2);

					// at this point just ignoring different plans						
					// TODO implement plans, height, tilt, etc.
					if (testLine.getSeparation() > 0) {
						// figure out if the current test pair of lines is better than the pair
						// at that field line index currently being stored
						int fi = std::max(line1.getFieldLineIndex(), line2.getFieldLineIndex());
						if (fi >= 0) {
							if (fieldLines[fi].getSeparation() > testLine.getSeparation()) {
								fieldLines[fi].getLine1().setFieldLineIndex(-1);
								fieldLines[fi].getLine2().setFieldLineIndex(-1);

								line1.setFieldLineIndex(fi);
								line2.setFieldLineIndex(fi);

								fieldLines[fi] = testLine;
							}
						}
						else { 
							line1.setFieldLineIndex(fieldLines.size());
							line2.setFieldLineIndex(fieldLines.size());
							fieldLines.push_back(testLine);
						}
					}
				}
			}
		}
	}
}

void HoughSpace::clear() {
	for (int t = 0; t < T_SPAN; t++) {
		for (int r = 0; r < R_SPAN; r++) {
			hs[r][t] = 0;
			peaks[r][t] = 0;
		}
	}
	lines.clear();
	fieldLines.clear();
}

}
}