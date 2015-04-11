#pragma once

#include <algorithm>

namespace man {
namespace vision {

// **************************
// *                        *
// *          Fool          *
// *                        *
// **************************
// No, I'm no talking about me.
// This is the FuzzyLogic class that helps
// find fieldlines using logical thresholds instead
// of strict booleans.(Operating, for instance, on the range [0...1])
// Overrided operators used for Fuzzy logic calculations.
class Fool {

public:
    Fool(double value_) : value(value_) {}
    double get() const { return value; }
    inline Fool operator&(const Fool& f) const;
    inline Fool operator|(const Fool& f) const;
    inline Fool operator!() const;

private:
    double value;
};

inline Fool Fool::operator&(const Fool& f) const
{
    return Fool(std::min(f.get(), get())); 
}

inline Fool Fool::operator|(const Fool& f) const
{
    return Fool(std::max(f.get(), get()));
}

inline Fool Fool::operator!() const
{
    return Fool(1 - get());
}


// Fuzzy thresholds construct and store the two ends
// of a fuzzy logic threshold, allowing you to do Fool
// operations
class FuzzyThr {
public:
    FuzzyThr(double t0_, double t1_) : t0(t0_), t1(t1_) {}
    inline double weight(double x) const;
    inline double getT0();
    inline double getT1();
    
private:
    double t0;
    double t1;
};

inline double FuzzyThr::getT0() {
    return t0;
}

inline double FuzzyThr::getT1() {
    return t1;
}
// TODO could be made more efficient, in particular are std::max and std::min efficient?
inline double FuzzyThr::weight(double x) const
{
    if (t0 == t1)
        return (x >= t0 ? 1. : 0.);
    return std::min(std::max((x - t0) / (t1 - t0), 0.), 1.);
}

inline Fool operator>(double x, const FuzzyThr& thr)
{
    return Fool(thr.weight(x));
}

inline Fool operator<(double x, const FuzzyThr& thr)
{
    return !(x > thr);
}

}
}
