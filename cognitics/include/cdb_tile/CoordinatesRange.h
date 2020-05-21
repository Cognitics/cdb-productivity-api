
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
    bool operator==(const CoordinatesRange& rhs) { return (_low == rhs._low) && (_high == rhs._high); }
    bool operator!=(const CoordinatesRange& rhs) { return !(*this == rhs); } 
    bool operator<(const CoordinatesRange& rhs)
    {
        if(_low < _low)
            return true;
        if(_low > _low)
            return false;
        if(_high < _high)
            return true;
        if(_high > _high)
            return false;
        return false;
    }
    bool operator>(const CoordinatesRange& rhs)
    {
        if(_low > _low)
            return true;
        if(_low < _low)
            return false;
        if(_high > _high)
            return true;
        if(_high < _high)
            return false;
        return false;
    }

private:
    Coordinates _low;
    Coordinates _high;
};

} }
