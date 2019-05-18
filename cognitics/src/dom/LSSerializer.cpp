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

#include "dom/LSSerializer.h"
#include "dom/LSSerializerFilter.h"
#include "dom/LSOutput.h"
#include "dom/DOMConfiguration.h"
#include "dom/Node.h"
#include "dom/Element.h"
#include "dom/Attr.h"
#include "dom/Text.h"
#include "dom/CDATASection.h"
#include "dom/ProcessingInstruction.h"
#include "dom/Comment.h"
#include "dom/Document.h"

#include <sstream>

namespace dom
{
    struct LSSerializer::_LSSerializer
    {
        DOMString newLine;
        DOMString indent;
        LSSerializerFilterSP filter;
    };

    LSSerializer::~LSSerializer(void)
    {
    }

    LSSerializer::LSSerializer(void) : _data(std::shared_ptr<_LSSerializer>(new _LSSerializer))
    {
    }

/*
    DOMConfiguration LSSerializer::getDOMConfig(void)
    {
        throw std::runtime_error("LSSerializer::getDOMConfig(): not yet implemented");
    }
*/

    DOMString LSSerializer::getNewLine(void)
    {
        return _data->newLine;
    }

    void LSSerializer::setNewLine(const DOMString &newLine)
    {
        _data->newLine = newLine;
    }

    DOMString LSSerializer::getIndent(void)
    {
        return _data->indent;
    }

    void LSSerializer::setIndent(const DOMString &indent)
    {
        _data->indent = indent;
    }

    LSSerializerFilterSP LSSerializer::getFilter(void)
    {
        return _data->filter;
    }

    void LSSerializer::setFilter(LSSerializerFilterSP filter)
    {
        _data->filter = filter;
    }

    bool LSSerializer::write(NodeSP nodeArg, LSOutputSP destination, bool recursive, int depth)
    {
        // TODO: finish
        // TODO: entity resolution
        // TODO: filter

        DOMString tab;
        for(int i = 0; i < depth; ++i)
            tab += _data->indent;
        DOMString result;
        switch(nodeArg->getNodeType())
        {
            case Node::ELEMENT_NODE:
                {
                    ElementSP element(std::dynamic_pointer_cast<Element>(nodeArg));
                    if(!destination->write(tab + "<" + element->getTagName()))
                        return false;
                    if(element->hasAttributes())
                    {
                        NamedNodeMap attributes = element->getAttributes();
                        for(NamedNodeMap::iterator it = attributes.begin(), end = attributes.end(); it != end; ++it)
                        {
                            if(!destination->write(" "))
                                return false;
                            if(!write(it->second, destination))
                                return false;
                        }
                    }
                    if(recursive && element->hasChildNodes())
                    {
                        NodeList childNodes = element->getChildNodes();
                        DOMString etab = tab;
                        DOMString newLine = _data->newLine;
                        if((childNodes.size() == 1) && (childNodes[0]->getNodeType() == Node::TEXT_NODE))
                        {
                            newLine = DOMString();
                            etab = DOMString();
                        }
                        if(!destination->write(">" + newLine))
                            return false;
                        for(NodeList::iterator it = childNodes.begin(), end = childNodes.end(); it != end; ++it)
                        {
                            if(!write(*it, destination, recursive, depth + 1))
                                return false;
                        }
                        if(!destination->write(etab + "</" + element->getTagName() + ">" + _data->newLine))
                            return false;
                    }
                    else
                    {
                        if(!destination->write(" />" + _data->newLine))
                            return false;
                    }
                }
                break;
            case Node::ATTRIBUTE_NODE:
                {
                    AttrSP attr(std::dynamic_pointer_cast<Attr>(nodeArg));
                    std::string value = attr->getValue().as_string();
                    while(value.find("&") != value.npos)
                        value.replace(value.find("&"), 1, "%%%%%amp;");
                    while(value.find("%%%%%amp;") != value.npos)
                        value.replace(value.find("%%%%%amp;"), 1, "&amp;");
                    while(value.find("'") != value.npos)
                        value.replace(value.find("'"), 1, "&apos;");
                    while(value.find("<") != value.npos)
                        value.replace(value.find("<"), 1, "&lt;");
                    while(value.find(">") != value.npos)
                        value.replace(value.find(">"), 1, "&gt;");
                    while(value.find("\"") != value.npos)
                        value.replace(value.find("\""), 1, "&quot;");
                    if(!destination->write(attr->getName() + "=\"" + value + "\""))
                        return false;
                }
                break;
            case Node::TEXT_NODE:
                {
                    TextSP text(std::dynamic_pointer_cast<Text>(nodeArg));
                    if(!destination->write(text->getData()))
                        return false;
                }
                break;
            case Node::CDATA_SECTION_NODE:
                {
                    CDATASectionSP cdataSection(std::dynamic_pointer_cast<CDATASection>(nodeArg));
                    if(!destination->write(tab + "<![CDATA[" + cdataSection->getData() + "]]>" + _data->newLine))
                        return false;
                }
                break;
            case Node::PROCESSING_INSTRUCTION_NODE:
                {
                    ProcessingInstructionSP processingInstruction(std::dynamic_pointer_cast<ProcessingInstruction>(nodeArg));
                    if(!destination->write(tab + "<?" + processingInstruction->getTarget() + " " + processingInstruction->getData() + "?>" + _data->newLine))
                        return false;
                }
                break;
            case Node::COMMENT_NODE:
                {
                    CommentSP comment(std::dynamic_pointer_cast<Comment>(nodeArg));
                    if(!destination->write(tab + "<!--" + comment->getData() + "-->" + _data->newLine))
                        return false;
                }
                break;
            case Node::DOCUMENT_NODE:
                {
                    DocumentSP document(std::dynamic_pointer_cast<Document>(nodeArg));
                    if(!destination->write("<?xml version=\"1.0\" encoding=\"utf-8\"?>" + _data->newLine))
                        return false;
                    NodeList childNodes = document->getChildNodes();
                    for(NodeList::iterator it = childNodes.begin(), end = childNodes.end(); it != end; ++it)
                    {
                        if(!write(*it, destination))
                            return false;
                    }
                }
                break;
            case Node::DOCUMENT_TYPE_NODE:
            case Node::ENTITY_REFERENCE_NODE:
            case Node::ENTITY_NODE:
            case Node::DOCUMENT_FRAGMENT_NODE:
            case Node::NOTATION_NODE:
            default:
                throw std::runtime_error("LSSerializer::write(): node type not yet implemented");

        }
        return true;
    }

/*
    bool LSSerializer::writeToURI(NodeSP nodeArg, const DOMString &uri)
    {
        throw std::runtime_error("LSSerializer::writeToURI(): not yet implemented");
    }
*/

    DOMString LSSerializer::writeToString(NodeSP nodeArg, bool recursive)
    {
        LSWriter result;
        LSOutputSP output(new LSOutput());
        output->characterStream = &result;
        return write(nodeArg, output, recursive) ? result.str() : DOMString();
    }



}