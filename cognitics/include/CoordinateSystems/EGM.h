#pragma once

#include "Image.h"

#include <utility>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class EGM
        {
        public:
            int PostsPerDegree = 0;
            int Rows = 0;
            int Columns = 0;

            // image is +90,-180 in top left to -90,+180 in bottom right
            Image<float> Image;

            std::pair<float, float> Range();
            float Height(double latitude, double longitude);

        protected:
            EGM(int postsPerDegree);
            int Row(double latitude);
            int Column(double longitude);
            double Latitude(int row);
            double Longitude(int column);
            int Index(double latitude, double longitude);

        };

    }
}
