
#include "CoordinateSystems/EGM.h"

#include <cmath>
#include <limits>
#include <algorithm>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        EGM::EGM(int postsPerDegree)
        {
            PostsPerDegree = postsPerDegree;
            Rows = (180 * PostsPerDegree) + 1;
            Columns = 360 * PostsPerDegree;
            _Image.Width = Columns;
            _Image.Height = Rows;
            _Image.Channels = 1;
            _Image.Data = new float[_Image.Width * _Image.Height];
        }

        int EGM::Row(double latitude)
        {
            return (int)std::floor((90 - latitude) * PostsPerDegree);
        }

        int EGM::Column(double longitude)
        {
            return (int)std::floor((longitude + 180) * PostsPerDegree);
        }

        double EGM::Latitude(int row)
        {
            return 90 - ((double)row / PostsPerDegree);
        }

        double EGM::Longitude(int column)
        {
            return ((double)column / PostsPerDegree) - 180;
        }

        int EGM::Index(double latitude, double longitude)
        {
            return (Row(latitude) * _Image.Width) + Column(longitude);
        }

        std::pair<float, float> EGM::Range()
        {
            float min = std::numeric_limits<float>::max();
            float max = std::numeric_limits<float>::min();
            for (int i = 0, c = _Image.Width * _Image.Height; i < c; ++i)
            {
                min = std::min(min, _Image.Data[i]);
                max = std::max(max, _Image.Data[i]);
            }
            return std::pair<float, float>(min, max);
        }

        float EGM::Height(double latitude, double longitude)
        {
            double lat = std::max(std::min(latitude, 90.0), -89.9999);
            double lon = std::max(std::min(latitude, 180.0), -179.9999);
            int nw_row = Row(lat);
            int nw_col = Column(lon);
            if (nw_row >= Rows - 1)
                --nw_row;
            int rot_lon = (nw_col < Columns - 1) ? 0 : Columns;
            int nw_index = (nw_row * Columns) + nw_col;
            double nw_lat = Latitude(nw_row);
            double nw_lon = Longitude(nw_col);
            float nw_h = _Image.Data[nw_index];
            float ne_h = _Image.Data[nw_index + 1 - rot_lon];
            float sw_h = _Image.Data[nw_index + Columns];
            float se_h = _Image.Data[nw_index + Columns + 1 - rot_lon];
            float nx = (float)(lon - nw_lon) * PostsPerDegree;
            float ny = (float)(nw_lat - lat) * PostsPerDegree;
            float a00 = nw_h;
            float a10 = ne_h - nw_h;
            float a01 = sw_h - nw_h;
            float a11 = nw_h - ne_h - sw_h + se_h;
            float height = a00 + (a10 * nx) + (a01 * ny) + (a11 * nx * ny);
            return height;
        }

    }
}

