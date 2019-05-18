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

#include "dom/DOMImplementation.h"
#include "dom/DocumentType.h"
#include "dom/Document.h"
#include "dom/Element.h"
#include "dom/DOMException.h"
#include "dom/LSParser.h"
#include "dom/LSSerializer.h"
#include "dom/LSInput.h"
#include "dom/LSOutput.h"
#include <iostream>
#include <fstream>

namespace dom
{
    DocumentSP DOMImplementation::readXML(const DOMString &filename, DOMImplementationSP domImplementation)
    {
        if(!domImplementation)
            domImplementation = DOMImplementationSP(new DOMImplementation);
        std::string filestr = filename;
        std::ifstream filestream;
        filestream.open(filestr.c_str());
        if(!filestream.is_open())
            return DocumentSP();
        std::stringstream data;
        data << filestream.rdbuf(); 
        filestream.close();
        LSReader datastr(data.str());
        LSInputSP input(new LSInput);
        input->characterStream = &datastr;
        LSParserSP parser(domImplementation->createLSParser());
        return parser->parse(input);
    }

    bool DOMImplementation::writeXML(const DOMString &filename, DocumentSP document, DOMImplementationSP domImplementation)
    {
        if(!document)
            return false;
        if(!domImplementation)
            domImplementation = DOMImplementationSP(new DOMImplementation);
        LSSerializerSP serializer(domImplementation->createLSSerializer());
        serializer->setNewLine("\n");
        serializer->setIndent("\t");
        DOMString datastr = serializer->writeToString(document);
        if(datastr.empty())
            return false;
        std::string data(datastr);
        std::string filestr = filename;
        std::ofstream filestream;
        filestream.open(filestr.c_str());
        filestream << data;
        filestream.close();
        return true;
    }

    bool DOMImplementation::writeXMLToString(std::string &str, DocumentSP document, DOMImplementationSP domImplementation)
    {
        if(!document)
            return false;
        if(!domImplementation)
            domImplementation = DOMImplementationSP(new DOMImplementation);
        LSSerializerSP serializer(domImplementation->createLSSerializer());
        serializer->setNewLine("\n");
        serializer->setIndent("\t");
        DOMString datastr = serializer->writeToString(document);
        if(datastr.empty())
            return false;
        std::string data(datastr);
        std::stringstream ss;
        ss << data;
        str = ss.str();
        return true;
    }

    bool DOMImplementation::writeXML(const DOMString &filename, ElementSP element, DOMImplementationSP domImplementation)
    {
        if(!element)
            return false;
        if(!domImplementation)
            domImplementation = DOMImplementationSP(new DOMImplementation);
        DocumentSP document = domImplementation->createDocument();
        document->appendChild(element);
        return writeXML(filename, document, domImplementation);
    }

    bool DOMImplementation::writeXMLToString(std::string &str, ElementSP element, DOMImplementationSP domImplementation)
    {
        if(!element)
            return false;
        if(!domImplementation)
            domImplementation = DOMImplementationSP(new DOMImplementation);
        DocumentSP document = domImplementation->createDocument();
        document->appendChild(element);
        return writeXMLToString(str, document, domImplementation);
    }

    struct DOMImplementation::_DOMImplementation
    {
        std::map<DOMString, DOMObjectSP> features;
        std::map<DOMString, FactoryFunc> factories;
    };

    DOMImplementation::~DOMImplementation(void)
    {
        delete _data;
    }

    DOMImplementation::DOMImplementation(void) : _data(new _DOMImplementation)
    {
        setFeature("Core", "", DOMObjectSP());
        setFeature("Core", "1.0", DOMObjectSP());
        setFeature("Core", "2.0", DOMObjectSP());
        setFeature("Core", "3.0", DOMObjectSP());
        setFeature("XML", "", DOMObjectSP());
        setFeature("XML", "1.0", DOMObjectSP());
        setFeature("XML", "2.0", DOMObjectSP());
        setFeature("XML", "3.0", DOMObjectSP());
        setFeature("XMLVersion", "", DOMObjectSP());
        setFeature("XMLVersion", "1.0", DOMObjectSP());
        setFeature("Traversal", "", DOMObjectSP());
        setFeature("Traversal", "2.0", DOMObjectSP());
        setFeature("Range", "", DOMObjectSP());
        setFeature("Range", "2.0", DOMObjectSP());
        setFeature("Factory", "", DOMObjectSP());
        setFeature("Factory", "1.0", DOMObjectSP());
        setFeature("Factory", "2.0", DOMObjectSP());
        setFeature("Factory", "3.0", DOMObjectSP());
    }

    void DOMImplementation::setFeature(const DOMString &feature, const DOMString &version, DOMObjectSP featureObject)
    {
        DOMString key = feature;
        if(feature.substr(0, 1) == "+")
            key = feature.substr(1);
        if(!version.empty())
            key.append(":" + version);
        std::transform(key.begin(), key.end(), key.begin(), tolower);
        _data->features[key] = featureObject;
    }

    bool DOMImplementation::hasFeature(const DOMString &feature, const DOMString &version)
    {
        return !getFeature(feature, version);
    }

    DOMObjectSP DOMImplementation::getFeature(const DOMString &feature, const DOMString &version)
    {
        DOMString key = feature;
        if(feature.substr(0, 1) == "+")
            key = feature.substr(1);
        if(!version.empty())
            key.append(":" + version);
        std::transform(key.begin(), key.end(), key.begin(), tolower);
        std::map<DOMString, DOMObjectSP>::iterator it = _data->features.find(key);
        if(it == _data->features.end())
            return DOMObjectSP();
        return (it->second) ? it->second : shared_from_this();
    }

    DocumentTypeSP DOMImplementation::createDocumentType(const DOMString &qualifiedName, const DOMString &publicId, const DOMString &systemId)
    {
        // TODO: validate qualifiedName (p39)
        DocumentTypeSP doctype(new DocumentType());
        doctype->setName(qualifiedName);
        doctype->setPublicId(publicId);
        doctype->setSystemId(systemId);
        return doctype;
    }

    DocumentSP DOMImplementation::createDocument(const DOMString &namespaceURI, const DOMString &qualifiedName, DocumentTypeSP doctype)
    {
        // TODO: validate namespaceURI and qualifiedName (p38)
        if(doctype && doctype->getOwnerDocument())
            throw DOMException(WRONG_DOCUMENT_ERR);
        DocumentSP document(new Document());
        if(doctype)
        {
            doctype->setOwnerDocument(document);
            document->appendChild(doctype);
        }
        document->setImplementation(std::dynamic_pointer_cast<DOMImplementation>(shared_from_this()));
        if(!qualifiedName.empty())
            document->appendChild(document->createElementNS(namespaceURI, qualifiedName));
        return document;
    }

    LSParserSP DOMImplementation::createLSParser(unsigned short mode, const DOMString &schemaType)
    {
        return LSParserSP(new LSParser(std::dynamic_pointer_cast<DOMImplementation>(shared_from_this())));
    }

    LSSerializerSP DOMImplementation::createLSSerializer(void)
    {
        return LSSerializerSP(new LSSerializer());
    }

    LSInputSP DOMImplementation::createLSInput(void)
    {
        return LSInputSP(new LSInput());
    }

/*
    LSOutputSP DOMImplementation::createLSOutput(void)
    {
        throw std::runtime_error("DOMImplementation::createLSOutput(): not yet implemented");
    }
*/

    ElementSP DOMImplementation::createElementNS(const DOMString &namespaceURI, const DOMString &qualifiedName)
    {
        int t = int(_data->factories.size());
        if(_data->factories.find(qualifiedName) == _data->factories.end())
            return ElementSP(new Element());
        return _data->factories[qualifiedName](qualifiedName);
    }

    void DOMImplementation::setFactory(const DOMString &tagName, FactoryFunc func)
    {
        _data->factories[tagName] = func;
    }

}