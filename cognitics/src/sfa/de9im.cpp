/*************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
#include "sfa/de9im.h"
#include <sstream>
#include <exception>
#include <stdexcept>
#include <algorithm>

namespace sfa
{
    de9im::de9im(void)
    {
        this->set(-1,-1,-1,-1,-1,-1,-1,-1,-1);
    }

    de9im::de9im(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {
        this->set(a,b,c,d,e,f,g,h,i);
    }

    de9im::de9im(char m[])
    {
        for (int i = 0; i < 9; i++)
        {
            if(m[i] == 'T')
                matrix[i] = 0;
            else if (m[i] == 'F')
                matrix[i] = -1;
            else if (m[i] == '*')
                matrix[i] = -1;
            else if (m[i] == '0')
                matrix[i] = 0;
            else if (m[i] == '1')
                matrix[i] = 1;
            else if (m[i] == '2')
                matrix[i] = 2;
            else
                throw std::runtime_error("de9im::de9im(char* m[]): invalid char encountered!");
        }
    }

    de9im::de9im(const de9im& other)
    {
        this->matrix[0] = other.matrix[0];
        this->matrix[1] = other.matrix[1];
        this->matrix[2] = other.matrix[2];
        this->matrix[3] = other.matrix[3];
        this->matrix[4] = other.matrix[4];
        this->matrix[5] = other.matrix[5];
        this->matrix[6] = other.matrix[6];
        this->matrix[7] = other.matrix[7];
        this->matrix[8] = other.matrix[8];
    }

    de9im::de9im(const de9im* other)
    {
        this->matrix[0] = other->matrix[0];
        this->matrix[1] = other->matrix[1];
        this->matrix[2] = other->matrix[2];
        this->matrix[3] = other->matrix[3];
        this->matrix[4] = other->matrix[4];
        this->matrix[5] = other->matrix[5];
        this->matrix[6] = other->matrix[6];
        this->matrix[7] = other->matrix[7];
        this->matrix[8] = other->matrix[8];
    }

    de9im& de9im::operator=(const de9im& other)
    {
        this->matrix[0] = other.matrix[0];
        this->matrix[1] = other.matrix[1];
        this->matrix[2] = other.matrix[2];
        this->matrix[3] = other.matrix[3];
        this->matrix[4] = other.matrix[4];
        this->matrix[5] = other.matrix[5];
        this->matrix[6] = other.matrix[6];
        this->matrix[7] = other.matrix[7];
        this->matrix[8] = other.matrix[8];
        return *this;
    }

    bool de9im::operator==(const de9im& other) const
    {
        if (this->matrix[0] != other.matrix[0]) return false;
        else if (this->matrix[1] != other.matrix[1]) return false;
        else if (this->matrix[2] != other.matrix[2]) return false;
        else if (this->matrix[3] != other.matrix[3]) return false;
        else if (this->matrix[4] != other.matrix[4]) return false;
        else if (this->matrix[5] != other.matrix[5]) return false;
        else if (this->matrix[6] != other.matrix[6]) return false;
        else if (this->matrix[7] != other.matrix[7]) return false;
        else if (this->matrix[8] != other.matrix[8]) return false;
        else return true;
    }

    bool de9im::operator !=(const de9im& other) const
    {
        return !(other == *this);
    }

    void de9im::set(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {
        matrix[0] = a;
        matrix[1] = b;
        matrix[2] = c;
        matrix[3] = d;
        matrix[4] = e;
        matrix[5] = f;
        matrix[6] = g;
        matrix[7] = h;
        matrix[8] = i;
    }
    
    void de9im::set(Location a, Location b, int value)
    {
        set(int(a),int(b),value);
    }

    void de9im::set(int row, int col, int value)
    {
        matrix[row*3+col] = value;
    }

    void de9im::setAtLeast(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {
        matrix[0] = std::max<int>(a,matrix[0]);
        matrix[1] = std::max<int>(b,matrix[1]);
        matrix[2] = std::max<int>(c,matrix[2]);
        matrix[3] = std::max<int>(d,matrix[3]);
        matrix[4] = std::max<int>(e,matrix[4]);
        matrix[5] = std::max<int>(f,matrix[5]);
        matrix[6] = std::max<int>(g,matrix[6]);
        matrix[7] = std::max<int>(h,matrix[7]);
        matrix[8] = std::max<int>(i,matrix[8]);
    }
    
    void de9im::setAtLeast(const de9im& another)
    {
        setAtLeast(    another.getValue(0,0),
                    another.getValue(1,0),
                    another.getValue(2,0),
                    another.getValue(0,1),
                    another.getValue(1,1),
                    another.getValue(2,1),
                    another.getValue(0,2),
                    another.getValue(1,2),
                    another.getValue(2,2) );
    }

    void de9im::setAtLeast(const de9im* another)
    {
        setAtLeast(*another);
    }

    void de9im::setAtLeast(Location a, Location b, int value)
    {
        if (a == BOUNDARY && b == INTERIOR && value == 0)    //    DEBUG
            int breakpoint = 0;                                //    DEBUG
        setAtLeast(int(a),int(b),value);
    }

    void de9im::setAtLeast(int row, int col, int value)
    {
        matrix[row*3+col] = std::max<int>(value,matrix[row*3+col]);
    }

    void de9im::setAtLeast(Label l)
    {
        if (l.loc[0] != UNKNOWN && l.loc[1] != UNKNOWN)
            setAtLeast(l.loc[0],l.loc[1],l.dim);
    }

    int de9im::getValue(int row, int col) const
    {
        return matrix[row*3+col];
    }

    int de9im::getValue(Location a, Location b) const
    {
        return getValue(int(a), int(b));
    }

    de9im& de9im::transpose(void)
    {
        int temp;
        temp = matrix[1];
        matrix[1] = matrix[3];
        matrix[3] = temp;
        temp = matrix[2];
        matrix[2] = matrix[6];
        matrix[6] = temp;
        temp = matrix[5];
        matrix[5] = matrix[7];
        matrix[7] = temp;
        return *this;
    }

    std::string de9im::toString(void) const
    {
        std::stringstream result;
        for (int i = 0; i < 9; i++)
        {
            if (matrix[i] == -1)
                result << 'F';
            else result << matrix[i];
        }
        return result.str();
    }

    std::string de9im::toMatrixString(void) const
    {
        std::stringstream result;
        for (int i = 0; i < 9; i++)
        {
            if (i == 3 || i == 6) result << "\n";
            if (matrix[i] == -1)
                result << 'F';
            else result << matrix[i];
        }
        return result.str();
    }

    std::string de9im::getRowAsString(int n) const
    {
        std::stringstream st;
        if (n == 0)
        {
            if (matrix[0] == -1) st << "F ";
            else st << matrix[0] << " ";
            if (matrix[1] == -1) st << "F ";
            else st << matrix[1] << " ";
            if (matrix[2] == -1) st << "F ";
            else st << matrix[2];
        }
        else if (n == 1)
        {
            if (matrix[3] == -1) st << "F ";
            else st << matrix[3] << " ";
            if (matrix[4] == -1) st << "F ";
            else st << matrix[4] << " ";
            if (matrix[5] == -1) st << "F ";
            else st << matrix[5];
        }
        else if (n == 2)
        {
            if (matrix[6] == -1) st << "F ";
            else st << matrix[6] << " ";
            if (matrix[7] == -1) st << "F ";
            else st << matrix[7] << " ";
            if (matrix[8] == -1) st << "F ";
            else st << matrix[8];
        }
        return st.str();
    }
        
    bool de9im::compare(const std::string& another) const
    {
        if (another.length() < 9) return false;

        const char* other = another.c_str();

        for (int i = 0; i < 9; i++)
        {
            if(other[i] == 'T' || other[i] == 't')
            {
                if (matrix[i] == -1) return false;
            }
            else if (other[i] == 'F' || other[i] == 'f')
            {
                if (matrix[i] != -1) return false;
            }
            else if (other[i] == '*')
            {
                //matrix can be anything
            }
            else if (other[i] == '0')
            {
                if (matrix[i] != 0) return false;
            }
            else if (other[i] == '1')
            {
                if (matrix[i] != 1) return false;
            }
            else if (other[i] == '2')
            {
                if (matrix[i] != 2) return false;
            }
            else
                throw std::runtime_error("de9im::compare(): invalid char encountered!");
        }
        return true;
    }
}