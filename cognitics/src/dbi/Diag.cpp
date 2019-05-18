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

#include "dbi/Diag.h"

#include <string>
#include <sstream>

namespace dbi
{
    struct DiagRec::_DiagRec
    {
        SQLSMALLINT type;
        SQLHANDLE handle;
        SQLRETURN result;
        int code;
        std::string text;
        std::string state;
    };

    DiagRec::DiagRec(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN result, SQLCHAR *sqlState, SQLSMALLINT nativeError, SQLCHAR *messageText) : _data(std::shared_ptr<_DiagRec>(new _DiagRec))
    {
        _data->type = type;
        _data->handle = handle;
        _data->result = result;
        _data->code = nativeError;
        _data->text = std::string((char *)messageText);
        _data->state = std::string((char *)sqlState);
    }

    DiagRec::~DiagRec(void)
    {
    }

    SQLSMALLINT DiagRec::getType(void)
    {
        return _data->type;
    }

    SQLHANDLE DiagRec::getHandle(void)
    {
        return _data->handle;
    }

    SQLRETURN DiagRec::getResult(void)
    {
        return _data->result;
    }

    std::string DiagRec::getResultName(void)
    {
        switch(_data->result)
        {
            case SQL_SUCCESS:            return("SQL_SUCCESS");
            case SQL_SUCCESS_WITH_INFO:    return("SQL_SUCCESS_WITH_INFO");
            case SQL_NO_DATA:            return("SQL_NO_DATA");
            case SQL_ERROR:                return("SQL_ERROR");
            case SQL_INVALID_HANDLE:    return("SQL_INVALID_HANDLE");
            case SQL_STILL_EXECUTING:    return("SQL_STILL_EXECUTING");
            case SQL_NEED_DATA:            return("SQL_NEED_DATA");
            default:                    return("UNKNOWN");
        }
        return("UNKNOWN");
    }

    int DiagRec::getCode(void)
    {
        return _data->code;
    }

    std::string DiagRec::getText(void)
    {
        return _data->text;
    }

    std::string DiagRec::getState(void)
    {
        return _data->state;
    }

    std::string DiagRec::getLogString(void)
    {
        std::stringstream ss;
        ss << "[";
        switch(_data->type)
        {
            case SQL_HANDLE_ENV: ss << "ENV"; break;
            case SQL_HANDLE_DBC: ss << "DBC"; break;
            case SQL_HANDLE_STMT: ss << "STMT"; break;
        }
        ss << "(" << _data->handle << "): " << getResultName() << "]";
        if(_data->state.size())
            ss << " [STATE: " << _data->state << "]";
        ss << " (" << getCode() << ")";
        if(_data->text.size())
            ss << " " << _data->text;
        return ss.str();
    }

    DiagSP Diag::_instance = DiagSP();

    Diag::Diag(void)
    {
    }

    Diag::~Diag(void)
    {
    }

    DiagSP Diag::instance(void)
    {
        if(!_instance)
            _instance = DiagSP(new Diag());
        return _instance;
    }

    void Diag::attach(ObserverSP observer)
    {
        observers.insert(observer);
    }

    void Diag::detach(ObserverSP observer)
    {
        observers.erase(observers.find(observer));
    }

    void Diag::notify(DiagRecSP diagRec)
    {
        for(std::set<ObserverSP>::iterator it = observers.begin(), end = observers.end(); it != end; it++)
            (*it)->notify(diagRec);
    }

    DiagLog::~DiagLog(void)
    {
    }

    DiagLog::DiagLog(void)
    {
    }

    void DiagLog::notify(dbi::DiagRecSP diagRec)
    {
        ccl::LOGLEVEL level = ccl::LWARNING;
        switch(diagRec->getResult())
        {
            case SQL_SUCCESS:                level = ccl::LDEBUG; break;
            case SQL_SUCCESS_WITH_INFO:        level = ccl::LDEBUG; break;
            case SQL_NO_DATA:                level = ccl::LINFO; break;
            case SQL_ERROR:                    level = ccl::LERR; break;
            case SQL_INVALID_HANDLE:        level = ccl::LEMERG; break;
            case SQL_STILL_EXECUTING:        level = ccl::LWARNING; break;
            case SQL_NEED_DATA:                level = ccl::LWARNING; break;
            default:                        level = ccl::LDEBUG; break;
        }
        ccl::Log::instance()->write(level, diagRec->getLogString());
    }

}
