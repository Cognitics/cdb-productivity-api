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
/*! \file dom/base.h
\headerfile dom/base.h
\brief Provides DOM base types
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.2
*/
#pragma once
#if (_MSC_VER >= 1700)
#include <memory>
#else
#include <boost/tr1/memory.hpp>
#endif
#include <boost/utility.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <ccl/ccl.h>

namespace dom
{
    typedef ccl::Variant Variant;
    typedef ccl::VariantList VariantList;
    typedef ccl::VariantMap VariantMap;
    typedef ccl::VariantMapList VariantMapList;

    typedef std::string DOMString;                        //!< DOM3-Core 1.2.1 [p24]
    typedef std::vector<DOMString> DOMStringList;        //!< DOM3-Core 1.4 [p33]
    typedef std::vector<DOMString> NameList;            //!< DOM3-Core 1.4 [p34]

    typedef unsigned long DOMTimeStamp;                    //!< DOM3-Core 1.2.2 [p24]

    typedef void DOMUserData;                            //!< DOM3-Core 1.2.3 [p25]
    typedef std::shared_ptr<DOMUserData> DOMUserDataSP;

    class DOMImplementation;
    typedef std::shared_ptr<DOMImplementation> DOMImplementationSP;
    typedef std::vector<DOMImplementationSP> DOMImplementationList;

    class DOMErrorHandler;
    typedef std::shared_ptr<DOMErrorHandler> DOMErrorHandlerSP;

    class DOMError;
    typedef std::shared_ptr<DOMError> DOMErrorSP;

    class DOMLocator;
    typedef std::shared_ptr<DOMLocator> DOMLocatorSP;

    class DOMConfiguration;
    typedef std::shared_ptr<DOMConfiguration> DOMConfigurationSP;

    class Node;
    typedef std::shared_ptr<Node> NodeSP;
    typedef std::weak_ptr<Node> NodeWP;
    typedef std::vector<NodeSP> NodeList;                //!< DOM3-Core 1.4 [p73]
    typedef std::map<DOMString, NodeSP> NamedNodeMap;    //!< DOM3-Core 1.4 [p73]

    class DocumentType;
    typedef std::shared_ptr<DocumentType> DocumentTypeSP;

    class DocumentFragment;
    typedef std::shared_ptr<DocumentFragment> DocumentFragmentSP;

    class Document;
    typedef std::shared_ptr<Document> DocumentSP;
    typedef std::weak_ptr<Document> DocumentWP;

    class Comment;
    typedef std::shared_ptr<Comment> CommentSP;

    class Attr;
    typedef std::shared_ptr<Attr> AttrSP;

    class Element;
    typedef std::shared_ptr<Element> ElementSP;
    typedef std::vector<ElementSP> ElementList;

    class CDATASection;
    typedef std::shared_ptr<CDATASection> CDATASectionSP;

    class CharacterData;
    typedef std::shared_ptr<CharacterData> CharacterDataSP;

    class TypeInfo;
    typedef std::shared_ptr<TypeInfo> TypeInfoSP;

    class Text;
    typedef std::shared_ptr<Text> TextSP;

    class ProcessingInstruction;
    typedef std::shared_ptr<ProcessingInstruction> ProcessingInstructionSP;

    class Notation;
    typedef std::shared_ptr<Notation> NotationSP;

    class UserDataHandler;
    typedef std::shared_ptr<UserDataHandler> UserDataHandlerSP;

    class EntityReference;
    typedef std::shared_ptr<EntityReference> EntityReferenceSP;

    class Entity;
    typedef std::shared_ptr<Entity> EntitySP;

    typedef std::shared_ptr<void> voidSP;

/*! \class dom::DOMObject base.h base.h
\brief Base DOM Object

The DOMObject type is used to represent an object.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.2.4 [p25]
*/
    class DOMObject : public std::enable_shared_from_this<DOMObject>
    {
    public:
        virtual ~DOMObject(void) { }
    };

    typedef std::shared_ptr<DOMObject> DOMObjectSP;

    class NodeIterator;
    typedef std::shared_ptr<NodeIterator> NodeIteratorSP;

    class NodeFilter;
    typedef std::shared_ptr<NodeFilter> NodeFilterSP;

    class TreeWalker;
    typedef std::shared_ptr<TreeWalker> TreeWalkerSP;

    class Range;
    typedef std::shared_ptr<Range> RangeSP;

    typedef std::istringstream LSInputStream;        //!< DOM3-LS 1.2.1
    typedef std::ostringstream LSOutputStream;        //!< DOM3-LS 1.2.2
    typedef std::istringstream LSReader;            //!< DOM3-LS 1.2.3
    typedef std::ostringstream LSWriter;            //!< DOM3-LS 1.2.4

    class LSInput;
    typedef std::shared_ptr<LSInput> LSInputSP;

    class LSOutput;
    typedef std::shared_ptr<LSOutput> LSOutputSP;

    class LSParser;
    typedef std::shared_ptr<LSParser> LSParserSP;

    class LSParserFilter;
    typedef std::shared_ptr<LSParserFilter> LSParserFilterSP;

    class LSResourceResolver;
    typedef std::shared_ptr<LSResourceResolver> LSResourceResolverSP;

    class LSSerializer;
    typedef std::shared_ptr<LSSerializer> LSSerializerSP;

    class LSSerializerFilter;
    typedef std::shared_ptr<LSSerializerFilter> LSSerializerFilterSP;

    // Cognitics
    typedef ccl::binary Binary;
    typedef unsigned char DOMChar;
    typedef std::stringstream DOMStringStream;
    typedef ElementSP (*FactoryFunc)(const dom::DOMString &);

}

