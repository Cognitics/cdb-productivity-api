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
/*! \file dbi/include/Diag.h
\headerfile dbi/include/Diag.h
\brief Provides dbi::Diag and dbi::DiagRec.
\author Aaron Brinton <abrinton@cognitics.net>
\date 05 July 2010
*/
#pragma once

#include <ccl/ccl.h>
#include <set>
//#include <boost/tr1/memory.hpp>

namespace dbi
{
    class Diag;
    typedef std::shared_ptr<Diag> DiagSP;

    class DiagRec;
    typedef std::shared_ptr<DiagRec> DiagRecSP;

/*! \class dbi::Observer Diag.h Diag.h
\brief Observer base class for dbi::Diag.
\sa dbi::Diag, dbi::DiagRec
*/
    class Observer
    {
    public:
        virtual ~Observer(void) { }
        Observer(void) { }

        //! Event handler.
        virtual void notify(DiagRecSP diagRec) = 0;
    };

    typedef std::shared_ptr<Observer> ObserverSP;

/*! \class dbi::DiagRec Diag.h Diag.h
\brief ODBC Diagnostic Record
\sa dbi::Diag, dbi::Observer
*/
    class DiagRec
    {
    private:
        struct _DiagRec;
        std::shared_ptr<_DiagRec> _data;

    public:
        ~DiagRec(void);
        DiagRec(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN result, SQLCHAR *sqlState, SQLSMALLINT nativeError, SQLCHAR *messageText);

        //! Get the handle type.
        SQLSMALLINT getType(void);

        //! Get the handle.
        SQLHANDLE getHandle(void);

        //! Get the sql return code;
        SQLRETURN getResult(void);

        //! Get the sql return code string.
        std::string getResultName(void);

        //! Get the native error code.
        int getCode(void);

        //! Get the error string.
        std::string getText(void);

        //! Get the state.
        std::string getState(void);

        //! \brief Get a formatted string suitable for logging.
        std::string getLogString(void);

    };


/*! \class dbi::Diag Diag.h Diag.h
\brief Singleton class for distributing diagnostic records to registered observers.
\sa dbi::Observer, dbi::DiagRec
*/
    class Diag
    {
    private:
        static DiagSP _instance;
        std::set<ObserverSP> observers;

    protected:
        Diag(void);
        Diag(const Diag &);
        Diag &operator=(const Diag &);

    public:
        ~Diag(void);

        //! Return the singleton log instance.
        static DiagSP instance(void);

        //! Attach an observer to the log.
        void attach(ObserverSP observer);

        //! Detach an observer from the log.
        void detach(ObserverSP observer);

        //! Send a diagnostic record to all attached observers.
        void notify(DiagRecSP diagRec);

    };

/*! \class dbi::DiagLog Diag.h Diag.h
\brief Observer class for ccl::Log
\sa dbi::Observer, dbi::DiagRec
*/
    class DiagLog : public Observer
    {
    public:
        virtual ~DiagLog(void);
        DiagLog(void);
        virtual void notify(dbi::DiagRecSP diagRec);
    };


}



