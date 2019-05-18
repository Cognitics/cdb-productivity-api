/****************************************************************************
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
/*! \file ccl/AttributeContainer.h
\headerfile ccl/AttributeContainer.h
\brief Container for attributes stored by string key
\author Kevin Bentley kbentley@cognitics.net
*/
#include <sstream>
#include "ccl/AttributeContainer.h"
#include <algorithm>

namespace ccl
{
    void AttributeContainer::setAttribute(const std::string &key, ccl::Variant var, bool ignoreCase)
    {    
        if(!ignoreCase)
        {
            if(attributes.find(key)==attributes.end())
                return;
            attributes[key] = var;
            return;
        }        
        std::string lowerkey = key;        
        std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
        std::map<std::string,std::string>::const_iterator iter = lowCaseAttributeMap.find(lowerkey);
        if(iter!=lowCaseAttributeMap.end())
        {            
            attributes[iter->second] = var;
            return;
        }
        //If we didn't find one, make the new one
        attributes[key] = var;
        lowCaseAttributeMap[lowerkey] = key;
    }

    bool AttributeContainer::getAttribute(const std::string &key, ccl::Variant &var, bool ignoreCase) const
    {
        if(!ignoreCase)
        {
            if(attributes.find(key)==attributes.end())
                return false;
            var = attributes.find(key)->second;
            return true;
        }
        
        std::string lowerkey = key;        
        std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
        std::map<std::string,std::string>::const_iterator iter = lowCaseAttributeMap.find(lowerkey);
        if(iter!=lowCaseAttributeMap.end())
        {
            var = attributes.find(iter->second)->second;
            return true;
        }
        
        return false;
    }
    bool AttributeContainer::hasAttribute(const std::string &key, bool ignoreCase) const
    {
        if(!ignoreCase)
        {
            if(attributes.find(key)==attributes.end())
                return false;
            else
                return true;
        }
        std::string lowerkey = key;        
        std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
        std::map<std::string,std::string>::const_iterator iter = lowCaseAttributeMap.find(lowerkey);
        if(iter!=lowCaseAttributeMap.end())
        {
            return true;
        }
        return false;
    }

    std::vector<std::string> AttributeContainer::getKeys() const
    {
        std::vector<std::string> ret;
        VariantMap::const_iterator iter = attributes.begin();
        while(iter!=attributes.end())
        {
            ret.push_back(iter->first);
            iter++;
        }
        return ret;
    }

    bool AttributeContainer::getAttribute(const std::string &key, int &val, bool ignoreCase) const
    {
        ccl::Variant var;
        bool ret = getAttribute(key,var,ignoreCase);
        if(!ret)
            return false;
        if(var.type()==Variant::TYPE_INT || var.parseable<int>())
        {
            val = var.as_int();
            return true;
        }
        return false;
    }

    bool AttributeContainer::getAttribute(const std::string &key, bool &val, bool ignoreCase) const
    {
        ccl::Variant var;
        bool ret = getAttribute(key,var,ignoreCase);
        if(!ret)
            return false;
        val = false;
        if(var.as_string() == "true")
            val = true;
        if(var.as_int() != 0)
            val = true;
        return true;
    }

    bool AttributeContainer::getAttribute(const std::string &key, double &val, bool ignoreCase) const
    {
        ccl::Variant var;
        bool ret = getAttribute(key,var,ignoreCase);
        if(!ret)
            return false;
        if(var.type()==Variant::TYPE_INT || var.parseable<double>())
        {
            val = var.as_double();
            return true;
        }
        return false;        
    }

    bool AttributeContainer::getAttribute(const std::string &key, std::string &val, bool ignoreCase) const
    {
        ccl::Variant var;
        bool ret = getAttribute(key,var,ignoreCase);
        if(!ret)
            return false;
        val = var.as_string();
        return true;
    }

    void AttributeContainer::setAttribute(const std::string &key, int val, bool ignoreCase)
    {
        ccl::Variant var(val);
        setAttribute(key,var);
    }

    void AttributeContainer::setAttribute(const std::string &key, bool val, bool ignoreCase)
    {
        ccl::Variant var(val);
        setAttribute(key,var);
    }

    void AttributeContainer::setAttribute(const std::string &key, double val, bool ignoreCase)
    {
        ccl::Variant var(val);
        setAttribute(key,var);
    }

    void AttributeContainer::setAttribute(const std::string &key, std::string val, bool ignoreCase)
    {
        ccl::Variant var(val);
        setAttribute(key,var);
    }

    std::string AttributeContainer::getAttributeAsString(const std::string &key,bool ignoreCase, std::string _default) const
    {
        std::string val;
        if(getAttribute(key,val,ignoreCase))
            return val;
        else
            return _default;
    }
    
    bool AttributeContainer::getAttributeAsBool(const std::string &key,bool ignoreCase, bool _default) const
    {
        bool val;
        if(getAttribute(key,val,ignoreCase))
            return val;
        else
            return _default;
    }

    int AttributeContainer::getAttributeAsInt(const std::string &key,bool ignoreCase, int _default) const
    {
        int val = 0;
        if(getAttribute(key,val,ignoreCase))
            return val;
        else
            return _default;
    }
    
    double AttributeContainer::getAttributeAsDouble(const std::string &key,bool ignoreCase, double _default) const
    {
        double val = 0.0f;
        if(getAttribute(key,val,ignoreCase))
            return val;
        else
            return _default;
    }

    ccl::Variant AttributeContainer::getAttributeAsVariant(const std::string &key,bool ignoreCase) const
    {
        ccl::Variant var;
        getAttribute(key,var,ignoreCase);
        return var;
    }

    void AttributeContainer::removeAttribute(const std::string &key,bool ignoreCase)
    {
        if(!ignoreCase)
        {
            if(attributes.find(key)==attributes.end())
                return;
            attributes.erase(attributes.find(key));
            return;
        }
        std::string lowerkey = key;        
        std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
        std::map<std::string,std::string>::const_iterator iter = lowCaseAttributeMap.find(lowerkey);
        if(iter!=lowCaseAttributeMap.end())
        {            
            attributes.erase(attributes.find(iter->second));
            lowCaseAttributeMap.erase(lowCaseAttributeMap.find(iter->first));
            return;
        }

    }

    void AttributeContainer::updateLowerCaseMap()
    {        
        if(lowCaseAttributeMap.size()!=attributes.size())
        {
            ccl::VariantMap::const_iterator iter = attributes.begin();
            while(iter!=attributes.end())
            {
                std::string lowerkey = iter->first;        
                std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
                if(lowCaseAttributeMap.find(lowerkey)==lowCaseAttributeMap.end())
                {
                    lowCaseAttributeMap[lowerkey] = iter->first;
                }
                iter++;
            }
        }
    }

    VariantMap *AttributeContainer::getVariantMap(void)
    {
        return &attributes;
    }

    int AttributeContainer::getAttributeType(const std::string &key,bool ignoreCase)
    {
        ccl::Variant var;
        getAttribute(key,var,ignoreCase);
        return var.type();//if getAttribute fails, we get a type of empty anyway
    }

    void AttributeContainer::insertAttributes(const AttributeContainer &in)
    {
        ccl::VariantMap::const_iterator iter = in.attributes.begin();
        while(iter!=in.attributes.end())
        {
            this->setAttribute(iter->first,iter->second);
            iter++;
        }
    }

    void AttributeContainer::insertAttributes(const ccl::VariantMap &in)
    {
        ccl::VariantMap::const_iterator iter = in.begin();
        while(iter!=in.end())
        {
            this->setAttribute(iter->first,iter->second);
            iter++;
        }
    }

    void AttributeContainer::bindData(ccl::BindStream bs)
    {
        ccl::LittleEndian<uint32_t> count = ccl::uint32_t(attributes.size());
        bs.bind(count);
        if(bs.writing())
        {
            ccl::VariantMap::const_iterator iter = attributes.begin();
            while(iter!=attributes.end())
            {
                ccl::binary bin;
                std::string key = iter->first;
                ccl::Variant var = iter->second;
                iter++;
                ccl::LittleEndian<uint32_t> keylen = ccl::uint32_t(key.size());
                bs.bind(keylen);
                bs.bind(key,int(key.length()));
                
                bin = var.encode();
                ccl::LittleEndian<uint32_t> len = ccl::uint32_t(bin.size());
                bs.bind(len);
                if(len)
                    bs.bind(bin,int(bin.size()));
            }
        }
        else
        {
            for(ccl::uint32_t i=0;i<count;i++)
            {
                ccl::binary bin;
                std::string key;
                ccl::LittleEndian<uint32_t> keylen;
                bs.bind(keylen);
                key.resize(keylen);
                bs.bind(key,int(key.length()));

                ccl::LittleEndian<uint32_t> len;
                bs.bind(len);
                bs.bind(bin,len);
                ccl::binary::iterator bin_iter = bin.begin();
                ccl::Variant var = ccl::Variant::decode(bin_iter);
                attributes[key] = var;
                std::string lowerkey = key;        
                std::transform(lowerkey.begin(), lowerkey.end(), lowerkey.begin(), ::tolower); 
                lowCaseAttributeMap[lowerkey] = key;
            }
        }

    }

    ccl::binary AttributeContainer::toBinary()
    {        
        std::ostringstream ss;
        ccl::BindStream bs;
        bs.setStream(ss);
        bindData(bs);
        ccl::binary ret(bin(ss.str()));
        int lenret = int(ret.size());
        return ret;
    }

    AttributeContainer AttributeContainer::fromBinary(ccl::binary const &input)
    {
        AttributeContainer ret;
        //std::string s(input);
        ccl::binaryistringstream ss(input);
        ccl::BindStream bs;
        bs.setStream((std::istream &)ss);        
        ret.bindData(bs);
        return ret;
    }

    void AttributeContainer::clear()
    {
        attributes.clear();
        lowCaseAttributeMap.clear();
    }

    std::string AttributeContainer::toString(void)
    {
        std::stringstream ss;
        for(ccl::VariantMap::iterator it = attributes.begin(), end = attributes.end(); it != end; ++it)
        {
            if(it != attributes.begin())
                ss << " ";
            ss << it->first << "=\"" << it->second.as_string() << "\"";
        }
        return ss.str();
    }

}
