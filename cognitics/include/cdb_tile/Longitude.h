
#pragma once

namespace cognitics {
namespace cdb {

class Longitude
{
public:
    Longitude(void);
    Longitude(double value);

    double value(void) const;
    bool operator==(const Longitude& rhs) { return (_value == rhs._value); }
    bool operator!=(const Longitude& rhs) { return !(*this == rhs); } 
    bool operator<(const Longitude& rhs) { return (_value < rhs._value); }
    bool operator>(const Longitude& rhs) { return (_value > rhs._value); }

private:
    double _value;
};

} }
