
#pragma once

#include "Coordinates.h"

namespace cognitics {
namespace cdb {

class CoordinatesRange
{
public:
    CoordinatesRange(void);
    CoordinatesRange(const Coordinates& min, const Coordinates& max);

    CoordinatesRange(double minLon, double maxLon, double minLat, double maxLat);

    Coordinates low(void) const;
    Coordinates high(void) const;

private:
    Coordinates _low;
    Coordinates _high;
};

} }
