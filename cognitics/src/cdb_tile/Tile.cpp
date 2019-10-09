#include "cdb_tile/Tile.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace cognitics
{
    namespace cdb
    {
        std::string get_uref_subdir(uint32_t uref)
        {
            std::stringstream ss;
            ss << "U" << uref;
            return ss.str();
        }

        std::string getLongitudeString(int lon)
        {
            std::stringstream ss;
            if (lon < 0)
                ss << "W" << std::setw(3) << std::setfill('0') << abs(lon);
            else
                ss << "E" << std::setw(3) << std::setfill('0') << abs(lon);
            return ss.str();
        }

        std::string getLatitudeString(int lat)
        {
            std::stringstream ss;
            if (lat < 0)
                ss << "S" << std::setw(2) << std::setfill('0') << abs(lat);
            else
                ss << "N" << std::setw(2) << std::setfill('0') << abs(lat);
            return ss.str();
        }

        ////////////////////////////////////////////////////////////////////////////////

        std::string Tile::getFilename(void) const
        {
            int32_t isouth = static_cast<int32_t>(floor(coordinates.low().latitude().value()));
            int32_t iwest = static_cast<int32_t>(floor(coordinates.low().longitude().value()));
            // LatLon_Dnnn_Snnn_Tnnn_LOD_Un_Rn.xxx (p117)
            std::stringstream ss;
            ss << "Tiles";
            ss << "/" << getLatitudeString(isouth);
            ss << "/" << getLongitudeString(iwest);
            ss << "/";
            ss << std::setw(3) << std::setfill('0') << int(dataset.code());
            ss << "_" << dataset.name();

            if (lod < 0)
                ss << "/LC";
            else
                ss << "/L" << std::setw(2) << std::setfill('0') << int(lod);
            ss << "/U" << uref;

            //Filename part
            ss << "/" << getLatitudeString(isouth);
            ss << getLongitudeString(iwest);
            ss << "_D" << std::setw(3) << std::setfill('0') << int(dataset.code());
            ss << "_S" << std::setw(3) << std::setfill('0') << cs1;
            ss << "_T" << std::setw(3) << std::setfill('0') << cs2;
            if (lod < 0)
                ss << "_LC";
            else
                ss << "_L" << std::setw(2) << std::setfill('0') << int(lod);
            ss << "_U" << uref;
            ss << "_R" << rref;
            ss << ".jp2";
            return ss.str();
        }

        Tile::Tile(CoordinatesRange _coordinates, Dataset _ds, int _lod, uint32_t _uref, uint32_t _rref, uint32_t _cs1,
                   uint32_t _cs2) :
            coordinates(_coordinates), dataset(_ds), lod(_lod), uref(_uref), rref(_rref), cs1(_cs1), cs2(_cs2)
        {
        }
    }
}
