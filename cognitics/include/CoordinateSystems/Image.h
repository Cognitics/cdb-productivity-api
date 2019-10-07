#pragma once

namespace Cognitics
{
    class IImage
    {
    public:
        int Width = 0;
        int Height = 0;
        int Channels = 0;
    };

    template<typename T>
    class Image : public IImage
    {
    public:
        T* Data = nullptr;

        ~Image()
        {
            delete Data;
        }
    };

}

