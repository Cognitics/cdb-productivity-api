
#include "CoordinateSystems/EGM2008.h"

#include <fstream>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        EGM2008::EGM2008() : EGM(24) { }

        EGM2008* EGM2008::CreateFromNGA(const char* filename)
        {
            std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
            size_t len = ifs.tellg();
            std::unique_ptr<char> bytes(new char[len]);
            char* bytes_ptr = bytes.get();
            ifs.seekg(0, std::ios::beg);
            ifs.read(bytes_ptr, len);

            auto egm = new EGM2008();
            for (int row = 0, row_count = egm->Rows; row < row_count; ++row)
            {
                for (int col = 0, col_count = egm->Columns; col < col_count; ++col)
                {
                    int img_index = (row * col_count) + col;
                    int src_index = 1 + (row * (col_count + 2)) + col;
                    egm->Image.Data[img_index] = *(float*)&bytes_ptr[src_index * sizeof(float)];
                }
            }
            return egm;
        }

    }
}

