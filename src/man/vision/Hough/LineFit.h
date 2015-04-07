#pragma once

namespace man {
namespace vision {

// linear regression
class LineFit {
public:
	LineFit();
	double getCenterX() { return sumX / sumW; }
	double getCenterY() { return sumY / sumW; }

	double getArea() { return sumW; }
	double getSecondPrincipalAxisU();
	double getSecondPrincipalAxisV();

	void add(double w, double x, double y);

private:
	double sumW;
	double sumX, sumY, sumXY, sumX2, sumY2;

	bool solved;
	double pLen1, pLen2;	 // principal lengths, pLen1 >= pLen2
	double uPAI, vPAI;		 // components of unit vector along 1st principal axis

	void solve();
};

}
}