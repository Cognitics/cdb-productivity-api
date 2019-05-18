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

#pragma warning ( disable : 4996 )    // unsafe
#pragma warning ( disable : 4309 )    // truncation of constant value

#include "dom/LSParser.h"
#include "dom/LSParserFilter.h"
#include "dom/LSInput.h"
#include "dom/DOMImplementation.h"
#include "dom/DOMConfiguration.h"
#include "dom/DocumentFragment.h"
#include "dom/DocumentType.h"
#include "dom/Document.h"
#include "dom/Entity.h"
#include "dom/Element.h"
#include "dom/Comment.h"
#include "dom/ProcessingInstruction.h"
#include "dom/CDATASection.h"

#include <cstdio>

namespace dom
{
    struct LSParser::_LSParser
    {
        DOMImplementationSP implementation;        // used for document creation
        DocumentSP document;                    // used for node creation
        NamedNodeMap entities;
        LSInputSP input;

        _LSParser(DOMImplementationSP implementation) : implementation(implementation)
        {
        }

        DocumentFragmentSP DocumentFragment(DocumentSP doc)
        {
            LSInput ls(input);
            DocumentFragmentSP fragment(doc->createDocumentFragment());
            if(Content(fragment))
            {
                ls.pass();
                return fragment;
            }
            ls.fail();
            return DocumentFragmentSP();
        }

        // [1] document ::= prolog element Misc*
        DocumentSP Document(void)
        {
            LSInput ls(input);
            DocumentSP doc(implementation->createDocument());
            doc->setOwnerDocument(doc);
            if(Prolog(doc) && Element(doc))
            {
                while(Misc(doc));
                ls.pass();
                return doc;
            }
            ls.fail();
            return DocumentSP();
        }

        // [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
        bool Char(void)
        {
            return input->ch(0x09) || input->ch(0x0A) || input->ch(0x0D) || input->range(0x20, 0xFF); // 0xD7FF); // || range(0x10000, 0x10FFFF);
        }

        // [3] S ::= (#x20 | #x9 | #xD | #xA)+
        bool S(void)
        {
            LSInput ls(input);
            if(ls.ch(0x20) || ls.ch(0x09) || ls.ch(0x0D) || ls.ch(0x0A))
            {
                while(ls.ch(0x20) || ls.ch(0x09) || ls.ch(0x0D) || ls.ch(0x0A));
                return ls.pass();
            }
            return ls.fail();
        }

        // [4] NameStartChar ::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
        bool NameStartChar(void)
        {
            return input->ch(':') || input->range('A', 'Z') || input->ch('_') || input->range('a', 'z') || input->range(0xC0, 0xD6)
                || input->range(0xD8, 0xF6) || input->range(0xF8, 0xFF); // || input->range(0x00F8, 0x02FF) || input->range(0x0370, 0x037D) || input->range(0x037F, 0x1FFF)
                //|| input->range(0x200C, 0x200D) || input->range(0x2070, 0x218F) || input->range(0x2C00, 0x2FEF) || input->range(0x3001, 0xD7FF)
                //|| input->range(0xF900, 0xFDCF) || input->range(0xFDF0, 0xFFFD); // || input->range(0x10000, 0xEFFFF))
        }

        // [4a] NameChar ::= NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040] 
        bool NameChar(void)
        {
            return NameStartChar() || input->ch('-') || input->ch('.') || input->range('0', '9') || input->ch(0xB7);
                // || input->range(0x0300, 0x036F) || input->range(0x203F, 0x2040);
        }

        // [5] Name ::= NameStartChar (NameChar)* 
        bool Name(DOMString &name)
        {
            LSInput ls(input);
            if(NameStartChar())
            {
                while(NameChar());
                name = ls.copy();
                return ls.pass();
            }
            return ls.fail();
        }

        // [6] Names ::= Name (#x20 Name)* 
        bool Names(DOMStringList &names)
        {
            LSInput ls(input);
            DOMString name;
            if(Name(name))
            {
                names.push_back(name);
                while(ls.ch(' ') && Name(name))
                    names.push_back(name);
                return ls.pass();
            }
            return ls.fail();
        }

        // [7] Nmtoken ::= (NameChar)+ 
        bool Nmtoken(DOMString &nmtoken)
        {
            LSInput ls(input);
            if(NameChar())
            {
                while(NameChar());
                nmtoken = ls.copy();
                return ls.pass();
            }
            return ls.fail();
        }

        // [8] Nmtokens ::= Nmtoken (#x20 Nmtoken)* 
        bool Nmtokens(DOMStringList &nmtokens)
        {
            LSInput ls(input);
            DOMString nmtoken;
            if(Nmtoken(nmtoken))
            {
                nmtokens.push_back(nmtoken);
                while(ls.ch(' ') && Nmtoken(nmtoken))
                    nmtokens.push_back(nmtoken);
                return ls.pass();
            }
            return ls.fail();
        }

        // [9] EntityValue ::= '"' ([^%&"] | PEReference | Reference)* '"' | "'" ([^%&'] | PEReference | Reference)* "'" 
        bool EntityValue(DocumentSP doc, DOMString &value)
        {
            LSInput ls(input);
            DOMString str;
            if(ls.ch('"'))
            {
                while(ls.not_in("%&\"") || PEReference(doc) || Reference(str));
                if(ls.ch('"'))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            if(ls.ch('\''))
            {
                while(ls.not_in("%&'") || PEReference(doc) || Reference(str));
                if(ls.ch('\''))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            return ls.fail();
        }

        // [10] AttValue ::= '"' ([^<&"] | Reference)* '"' | "'" ([^<&'] | Reference)* "'" 
        bool AttValue(DocumentSP doc, DOMString &value)
        {
            LSInput ls(input);
            DOMString str;
            if(ls.ch('"'))
            {
                while(ls.not_in("<&\"") || Reference(str));
                if(ls.ch('"'))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            if(ls.ch('\''))
            {
                while(ls.not_in("<&'") || Reference(str));
                if(ls.ch('\''))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            return ls.fail();
        }

        // [11] SystemLiteral ::= ('"' [^"]* '"') | ("'" [^']* "'")
        bool SystemLiteral(DOMString &value)
        {
            LSInput ls(input);
            if(ls.ch('"'))
            {
                while(ls.not_in("\""));
                if(ls.ch('"'))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            if(ls.ch('\''))
            {
                while(ls.not_in("'"));
                if(ls.ch('\''))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            return ls.fail();
        }

        // [12] PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'" 
        bool PubidLiteral(DOMString &value)
        {
            LSInput ls(input);
            if(ls.ch('"'))
            {
                while(PubidChar());
                if(ls.ch('"'))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                return ls.fail();
            }
            if(ls.ch('\''))
            {
                if(ls.ch('\''))
                {
                    value = ls.copy(1, -1);
                    return ls.pass();
                }
                while(PubidChar())
                {
                    if(ls.ch('\''))
                    {
                        value = ls.copy(1, -1);
                        return ls.pass();
                    }
                }
                return ls.fail();
            }
            return ls.fail();
        }

        // [13] PubidChar ::= #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%] 
        bool PubidChar(void)
        {
            return input->ch(0x20) || input->ch(0x0D) || input->ch(0x0A) || input->range('a', 'z') || input->range('A', 'Z')
                || input->range('0', '9') || input->in("-'()+,./:=?;!*#@$_%");
        }

        // [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
        bool CharData(NodeSP node)
        {
            LSInput ls(input);
            while(ls.not_in("<&"));
            DOMString data = ls.copy();
            int p1 = int(data.find_first_not_of(" \t\r\n"));
            if(p1 == DOMString::npos)
                p1 = 0;
            int p2 = int(data.find_last_not_of(" \t\r\n"));
            data = data.substr(p1, p2 - p1 + 1);
            if(data.empty())
                return ls.pass();
            TextSP text = node->getOwnerDocument()->createTextNode(data);
            node->appendChild(text);
            return ls.pass();
        }

        // [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
        bool Comment(NodeSP node)
        {
            // TODO: this isn't fully compliant ('--->' is accepted)
            LSInput ls(input);
            if(!ls.str("<!--"))
                return ls.fail();
            while(!ls.str("-->"))
            {
                if(!Char())
                    return ls.fail();
            }
            DOMString data = ls.copy(4, -3);
            CommentSP comment = node->getOwnerDocument()->createComment(data);
            node->appendChild(comment);
            return ls.pass();
        }

        // [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>' 
        bool PI(NodeSP node)
        {
            LSInput ls(input);
            DOMString target;
            if(ls.str("<?") && PITarget(target))
            {
                DOMString data;
                if(S())
                {
                    while(!ls.str("?>"))
                    {
                        if(!Char())
                            return ls.fail();
                    }
                    data = ls.copy(2 + int(target.length()) + 1, -2);
                }
                ProcessingInstructionSP pi = node->getOwnerDocument()->createProcessingInstruction(target, data);
                node->appendChild(pi);
                return ls.pass();
            }
            return ls.fail();
        }

        // [17] PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l')) 
        bool PITarget(DOMString &piTarget)
        {
            LSInput ls(input);
            DOMString name;
            if(Name(name))
            {
                DOMString cmp;
                cmp.resize(name.length());
                std::transform(name.begin(), name.end(), cmp.begin(), tolower);
                if(cmp == "xml")
                    return ls.fail();
                piTarget = name;
                return ls.pass();
            }
            return ls.fail();
        }

        // [18] CDSect ::= CDStart CData CDEnd
        bool CDSect(NodeSP node)
        {
            LSInput ls(input);
            DOMString data;
            if(CDStart() && CData(data) && CDEnd())
            {
                CDATASectionSP cd = node->getOwnerDocument()->createCDATASection(data);
                node->appendChild(cd);
                return ls.pass();
            }
            return ls.fail();
        }

        // [19] CDStart ::= '<![CDATA[' 
        bool CDStart(void)
        {
            return input->str("<![CDATA[");
        }

        // [20] CData ::= (Char* - (Char* ']]>' Char*))
        bool CData(DOMString &cdata)
        {
            LSInput ls(input);
            while(ls.not_str("]]>"))
            {
                if(!Char())
                    return ls.fail();
            }
            cdata = ls.copy();
            return ls.pass();
        }

        // [21] CDEnd ::= ']]>' 
        bool CDEnd(void)
        {
            return input->str("]]>");
        }

        // [22] prolog ::= XMLDecl? Misc* (doctypedecl Misc*)? 
        bool Prolog(DocumentSP doc)
        {
            LSInput ls(input);
            XMLDecl(doc);
            while(Misc(doc));
            DocTypeDecl(doc);
            while(Misc(doc));
            return ls.pass();
        }

        // [23] XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>' 
        bool XMLDecl(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.str("<?xml") && VersionInfo(doc))
            {
                EncodingDecl(doc);
                SDDecl(doc);
                S();
                if(ls.str("?>"))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [24] VersionInfo ::= S 'version' Eq ("'" VersionNum "'" | '"' VersionNum '"') 
        bool VersionInfo(DocumentSP doc)
        {
            LSInput ls(input);
            if(S() && ls.str("version") && Eq())
            {
                DOMString version;
                if((ls.ch('"') && VersionNum(version) && ls.ch('"'))
                    || (ls.ch('\'') && VersionNum(version) && ls.ch('\'')))
                {
                    doc->setXmlVersion(version);
                    return ls.pass();
                }
            }
            return ls.fail();
        }

        // [25] Eq ::= S? '=' S? 
        bool Eq(void)
        {
            LSInput ls(input);
            S();
            if(ls.ch('='))
            {
                S();
                return ls.pass();
            }
            return ls.fail();
        }

        // [26] VersionNum ::= '1.' [0-9]+ 
        bool VersionNum(DOMString &version)
        {
            LSInput ls(input);
            if(ls.str("1.") && ls.range('0', '9'))
            {
                while(ls.range('0', '9'));
                version = ls.copy();
                return ls.pass();
            }
            return ls.fail();
        }

        // [27] Misc ::= Comment | PI | S 
        bool Misc(DocumentSP doc)
        {
            LSInput ls(input);
            if(Comment(doc) || PI(doc) || S())
                return ls.pass();
            return ls.fail();
        }

        // [28] doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
        bool DocTypeDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            DOMString publicId; // TODO
            DOMString systemId;    // TODO
            if(ls.str("<!DOCTYPE") && S() && Name(name) && ((S() && ExternalID(doc)) || true) && (S() || true)
                && ((ls.ch('[') && IntSubset(doc) && ls.ch(']') && (S() || true)) || true) && ls.ch('>'))
            {
                DocumentTypeSP doctype(doc->getImplementation()->createDocumentType(name, publicId, systemId));
                doc->appendChild(doctype);
                return ls.pass();
            }
            return ls.fail();
        }

        // [28a] DeclSep ::= PEReference | S
        bool DeclSep(DocumentSP doc)
        {
            LSInput ls(input);
            if(PEReference(doc) || S())
                return ls.pass();
            return ls.fail();
        }

        // [28b] intSubset ::= (markupdecl | DeclSep)* 
        bool IntSubset(DocumentSP doc)
        {
            while(MarkupDecl(doc) || DeclSep(doc));
            return true;
        }

        // [29] markupdecl ::= elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment 
        bool MarkupDecl(DocumentSP doc)
        {
            LSInput ls(input);
            if(ElementDecl(doc) || AttlistDecl(doc) || EntityDecl(doc) || NotationDecl(doc) || PI(doc) || Comment(doc))
                return ls.pass();
            return ls.fail();
        }

        // [30] extSubset ::= TextDecl? extSubsetDecl
        bool ExtSubset(DocumentSP doc)
        {
            LSInput ls(input);
            TextDecl(doc);
            if(ExtSubsetDecl(doc))
                return ls.pass();
            return ls.fail();
        }

        // [31] extSubsetDecl ::= ( markupdecl | conditionalSect | DeclSep)*
        bool ExtSubsetDecl(DocumentSP doc)
        {
            while(MarkupDecl(doc) || ConditionalSect(doc) || DeclSep(doc));
            return true;
        }

        // [32] SDDecl ::= S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"')) 
        bool SDDecl(DocumentSP doc)
        {
            LSInput ls(input);
            if(S() && ls.str("standalone") && Eq())
            {
                if((ls.ch('"') && ls.str("yes") && ls.ch('"'))
                    || (ls.ch('\'') && ls.str("yes") && ls.ch('\'')))
                {
                    doc->setXmlStandalone(true);
                    return ls.pass();
                }
                if((ls.ch('"') && ls.str("no") && ls.ch('"'))
                    || (ls.ch('\'') && ls.str("no") && ls.ch('\'')))
                {
                    doc->setXmlStandalone(false);
                    return ls.pass();
                }
            }
            return ls.fail();
        }

        // [39] element ::= EmptyElemTag | STag content ETag 
        bool Element(NodeSP node)
        {
            DOMString name;
            {
                // look ahead to get the element name
                LSInput ls(input);
                if(!ls.ch('<') || !Name(name))
                    return ls.fail();
                ls.fail();    // reset stream
            }
            LSInput ls(input);
            ElementSP element(node->getOwnerDocument()->createElement(name));
            if(EmptyElemTag(element) || (STag(element) && Content(element) && ETag(element)))
            {
                node->appendChild(element);
                return ls.pass();
            }
            return ls.fail();
        }

        // [40] STag ::= '<' Name (S Attribute)* S? '>'
        bool STag(ElementSP element)
        {
            LSInput ls(input);
            if(ls.ch('<') && ls.str(element->getNodeName().c_str()))
            {
                while(S() && Attribute(element));
                S();
                if(ls.ch('>'))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [41] Attribute ::= Name Eq AttValue
        bool Attribute(ElementSP element)
        {
            LSInput ls(input);
            DOMString name;
            DOMString value;
            if(Name(name) && Eq() && AttValue(element->getOwnerDocument(), value))
            {
                element->setAttribute(name, value);
                return ls.pass();
            }
            return ls.fail();
        }

        // [42] ETag ::= '</' Name S? '>' 
        bool ETag(ElementSP element)
        {
            LSInput ls(input);
            if(ls.str("</") && ls.str(element->getNodeName().c_str()) && (S() || true) && ls.ch('>'))
                return ls.pass();
            return ls.fail();
        }

        // [43] content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)* 
        bool Content(NodeSP node)
        {
            DOMString data;
            CharData(node);
            while((Element(node) || Reference(data) || CDSect(node) || PI(node) || Comment(node)) && (CharData(node) || true));
            return true;
        }

        // [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>' 
        bool EmptyElemTag(ElementSP element)
        {
            LSInput ls(input);
            if(ls.ch('<') && ls.str(element->getNodeName().c_str()))
            {
                while(S() && Attribute(element));
                S();
                if(ls.str("/>"))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [45] elementdecl ::= '<!ELEMENT' S Name S contentspec S? '>'
        bool ElementDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("<!ELEMENT") && S() && Name(name) && S() && ContentSpec(doc) && (S() || true) && ls.ch('>'))
                return ls.pass();
            return ls.fail();
        }

        // [46] contentspec ::= 'EMPTY' | 'ANY' | Mixed | children 
        bool ContentSpec(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.str("EMPTY") || ls.str("ANY") || Mixed(doc) || Children(doc))
                return ls.pass();
            return ls.fail();
        }

        // [47] children ::= (choice | seq) ('?' | '*' | '+')? 
        bool Children(DocumentSP doc)
        {
            LSInput ls(input);
            if((Choice(doc) || Seq(doc))
                && (ls.ch('?') || ls.ch('*') || ls.ch('+') || true))
                return ls.pass();
            return ls.fail();
        }

        // [48] cp ::= (Name | choice | seq) ('?' | '*' | '+')? 
        bool CP(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if((Name(name) || Choice(doc) || Seq(doc)) && (ls.ch('?') || ls.ch('*') || ls.ch('+') || true))
                return ls.pass();
            return ls.fail();
        }

        // [49] choice ::= '(' S? cp ( S? '|' S? cp )+ S? ')'
        bool Choice(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.ch('(') && (S() || true) && CP(doc) && (S() || true) && ls.ch('|') && (S() || true) && CP(doc))
            {
                while(ls.ch('|'))
                {
                    S();
                    if(!CP(doc))
                        return ls.fail();
                    S();
                }
                if(ls.ch(')'))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [50] seq ::= '(' S? cp ( S? ',' S? cp )* S? ')' 
        bool Seq(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.ch('(') && (S() || true) && CP(doc))
            {
                S();
                while(ls.ch(','))
                {
                    S();
                    if(!CP(doc))
                        return ls.fail();
                    S();
                }
                if(ls.ch(')'))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [51] Mixed ::= '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')' 
        bool Mixed(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.ch('(') && (S() || true) && ls.str("#PCDATA"))
            {
                S();
                if(ls.str(")*"))
                    return ls.pass();
                if(ls.ch(')'))
                    return ls.pass();
                while(ls.ch('|'))
                {
                    S();
                    DOMString name;
                    if(!Name(name))
                        return ls.fail();
                    S();
                }
                if(ls.str(")*"))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [52] AttlistDecl ::= '<!ATTLIST' S Name AttDef* S? '>' 
        bool AttlistDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("<!ATTLIST") && S() && Name(name))
            {
                while(AttDef(doc));
                S();
                if(ls.ch('>'))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [53] AttDef ::= S Name S AttType S DefaultDecl 
        bool AttDef(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(S() && Name(name) && S() && AttType(doc) && S() && DefaultDecl(doc))
                return ls.pass();
            return ls.fail();
        }

        // [54] AttType ::= StringType | TokenizedType | EnumeratedType 
        bool AttType(DocumentSP doc)
        {
            return StringType() || TokenizedType() || EnumeratedType(doc);
        }

        // [55] StringType ::= 'CDATA' 
        bool StringType(void)
        {
            return input->str("CDATA");
        }

        // [56] TokenizedType ::= 'ID' | 'IDREF' | 'IDREFS' | 'ENTITY' | 'ENTITIES' | 'NMTOKEN' | 'NMTOKENS' 
        bool TokenizedType(void)
        {
            return input->str("IDREFS") || input->str("IDREF") || input->str("ID") || input->str("ENTITIES")
                || input->str("ENTITY") || input->str("NMTOKENS") || input->str("NMTOKEN");
        }

        // [57] EnumeratedType ::= NotationType | Enumeration 
        bool EnumeratedType(DocumentSP doc)
        {
            return NotationType(doc) || Enumeration(doc);
        }

        // [58] NotationType ::= 'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')' 
        bool NotationType(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("NOTATION") && S() && ls.ch('(') && (S() || true) && Name(name))
            {
                S();
                while(ls.ch('|'))
                {
                    S();
                    DOMString name;
                    if(!Name(name))
                        return ls.fail();
                    S();
                }
                S();
                if(ls.ch(')'))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [59] Enumeration ::= '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')' 
        bool Enumeration(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString nmtoken;
            if(ls.ch('(') && (S() || true) && Nmtoken(nmtoken))
            {
                S();
                while(ls.ch('|'))
                {
                    S();
                    DOMString nmtoken;
                    if(!Nmtoken(nmtoken))
                        return ls.fail();
                    S();
                }
                S();
                if(ls.ch(')'))
                    return ls.pass();
                return ls.fail();
            }
            return ls.fail();
        }

        // [60] DefaultDecl ::= '#REQUIRED' | '#IMPLIED' | (('#FIXED' S)? AttValue) 
        bool DefaultDecl(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.str("#REQUIRED") || ls.str("#IMPLIED"))
                return ls.pass();
            if(ls.str("#FIXED"))
            {
                DOMString value;
                if(S() && AttValue(doc, value))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [61] conditionalSect ::= includeSect | ignoreSect 
        bool ConditionalSect(DocumentSP doc)
        {
            return IncludeSect(doc) || IgnoreSect(doc);
        }

        // [62] includeSect ::= '<![' S? 'INCLUDE' S? '[' extSubsetDecl ']]>'
        bool IncludeSect(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.str("<![") && (S() || true) && ls.str("INCLUDE") && (S() || true) && ls.ch('[') && ExtSubsetDecl(doc) && ls.str("]]>"))
                return ls.pass();
            return ls.fail();
        }

        // [63] ignoreSect ::= '<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'
        bool IgnoreSect(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.str("<![") && (S() || true) && ls.str("IGNORE") && (S() || true) && ls.ch('['))
            {
                while(IgnoreSectContents(doc));
                if(ls.str("]]>"))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [64] ignoreSectContents ::= Ignore ('<![' ignoreSectContents ']]>' Ignore)* 
        bool IgnoreSectContents(DocumentSP doc)
        {
            // TODO
            return false;
        }

        // [65] Ignore ::= Char* - (Char* ('<![' | ']]>') Char*) 
        bool Ignore(DocumentSP doc)
        {
            // TODO
            return false;
        }

        // [66] CharRef ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';' 
        bool CharRef(DOMString &data)
        {
            LSInput ls(input);
            if(ls.str("&#"))
            {
                if(ls.ch('x') && (ls.range('0', '9') || ls.range('a', 'f') || ls.range('A', 'F')))
                {
                    while(ls.range('0', '9') || ls.range('a', 'f') || ls.range('A', 'F'));
                    if(ls.ch(';'))
                    {
                        std::wstring numstr = ccl::wstring(ls.copy(3, -1));
                        wchar_t num;
                        if(swscanf(numstr.c_str(), L"%x", &num) != EOF)
                        {
                            char numc = num;
                            data.append(&numc, 1);
                            return ls.pass();
                        }
                    }
                    return ls.fail();
                }
                else
                {
                    if(ls.range('0', '9'))
                    {
                        while(ls.range('0', '9'))
                        if(ls.ch(';'))
                        {
                            std::wstring numstr = ccl::wstring(ls.copy(2, -1));
                            wchar_t num;
                            if(swscanf(numstr.c_str(), L"%d", &num) != EOF)
                            {
                                char numc = num;
                                data.append(&numc, 1);
                                return ls.pass();
                            }
                        }
                    }
                    return ls.fail();
                }
            }
            return ls.fail();
        }

        // [67] Reference ::= EntityRef | CharRef 
        bool Reference(DOMString &data)
        {
            return CharRef(data) || EntityRef(data);
        }

        // [68] EntityRef ::= '&' Name ';' 
        bool EntityRef(DOMString &data)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.ch('&') && Name(name) && ls.ch(';'))
            {
                // TODO: retrieve value from entities
                if(name == "amp")
                    data.append("&");
                if(name == "apos")
                    data.append("'");
                if(name == "lt")
                    data.append("<");
                if(name == "gt")
                    data.append(">");
                if(name == "quot")
                    data.append("\"");
                return ls.pass();
            }
            return ls.fail();
        }

        // [69] PEReference ::= '%' Name ';' 
        bool PEReference(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.ch('%') && Name(name) && ls.ch(';'))
                return ls.pass();
            return ls.fail();
        }

        // [70] EntityDecl ::= GEDecl | PEDecl 
        bool EntityDecl(DocumentSP doc)
        {
            return GEDecl(doc) || PEDecl(doc);
        }

        // [71] GEDecl ::= '<!ENTITY' S Name S EntityDef S? '>' 
        bool GEDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("<!ENTITY") && S() && Name(name) && S() && EntityDef(doc) && (S() || true) && ls.ch('>'))
            {
                EntitySP entity(new Entity());
                entity->setNotationName(name);

                // TODO: entity value

                entities[name] = entity;
                return ls.pass();
            }
            return ls.fail();
        }

        // [72] PEDecl ::= '<!ENTITY' S '%' S Name S PEDef S? '>' 
        bool PEDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("<!ENTITY") && S() && ls.ch('%') && S() && Name(name) && S() && PEDef(doc) && (S() || true) && ls.ch('>'))
            {
                EntitySP entity(new Entity());
                entity->setNotationName(name);        // TODO: this isn't right

                // TODO: entity value

                entities[name] = entity;
                return ls.pass();
            }
            return ls.fail();
        }

        // [73] EntityDef ::= EntityValue | (ExternalID NDataDecl?) 
        bool EntityDef(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString value;
            if(EntityValue(doc, value))
                return ls.pass();
            if(ExternalID(doc))
            {
                NDataDecl(doc);
                return ls.pass();
            }
            return ls.fail();
        }

        // [74] PEDef ::= EntityValue | ExternalID 
        bool PEDef(DocumentSP doc)
        {
            DOMString value;
            return EntityValue(doc, value) || ExternalID(doc);
        }

        // [75] ExternalID ::= 'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral 
        bool ExternalID(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString value;
            if((ls.str("SYSTEM") && S() && SystemLiteral(value)) || (ls.str("PUBLIC") && S() && PubidLiteral(value) && S() && SystemLiteral(value)))
                return ls.pass();
            return ls.fail();
        }

        // [76] NDataDecl ::= S 'NDATA' S Name 
        bool NDataDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(S() && ls.str("NDATA") && S() && Name(name))
                return ls.pass();
            return ls.fail();
        }

        // [77] TextDecl ::= '<?xml' VersionInfo? EncodingDecl S? '?>' 
        bool TextDecl(DocumentSP doc)
        {
            // TODO
            return false;
        }

        // [78] extParsedEnt ::= TextDecl? content 
        bool ExtParsedEnt(DocumentSP doc)
        {
            LSInput ls(input);
            TextDecl(doc);
            if(Content(doc))
                return ls.pass();
            return ls.fail();
        }

        // [80] EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" ) 
        bool EncodingDecl(DocumentSP doc)
        {
            LSInput ls(input);
            if(S() && ls.str("encoding") && Eq())
            {
                if(ls.ch('"') && EncName(doc) && ls.ch('"'))
                    return ls.pass();
                if(ls.ch('\'') && EncName(doc) && ls.ch('\''))
                    return ls.pass();
            }
            return ls.fail();
        }

        // [81] EncName ::= [A-Za-z] ([A-Za-z0-9._] | '-')* 
        bool EncName(DocumentSP doc)
        {
            LSInput ls(input);
            if(ls.range('A', 'Z') || ls.range('a', 'z'))
            {
                while(ls.range('A', 'Z') || ls.range('a', 'z') || ls.range('0', '9') || ls.ch('.') || ls.ch('_') || ls.ch('-'));
                doc->setXmlEncoding(ls.copy());
                return ls.pass();
            }
            return ls.fail();
        }

        // [82] NotationDecl ::= '<!NOTATION' S Name S (ExternalID | PublicID) S? '>'
        bool NotationDecl(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString name;
            if(ls.str("<!NOTATION") && S() && Name(name) && (ExternalID(doc) || PublicID(doc)) && (S() || true) && ls.ch('>'))
                return ls.pass();
            return ls.fail();
        }

        // [83] PublicID ::= 'PUBLIC' S PubidLiteral
        bool PublicID(DocumentSP doc)
        {
            LSInput ls(input);
            DOMString value;
            if(ls.str("PUBLIC") && S() && PubidLiteral(value))
                return ls.pass();
            return ls.fail();
        }

    };

    //--------------------------------------------------------------------------------

    LSParser::~LSParser(void)
    {
    }

    LSParser::LSParser(DOMImplementationSP implementation) : _data(std::shared_ptr<_LSParser>(new _LSParser(implementation)))
    {
    }

/*
    DOMConfiguration LSParser::getDOMConfig(void)
    {
        throw std::runtime_error("LSParser::getDOMConfig(): not yet implemented");
    }
*/

/*
    LSParserFilterSP LSParser::getFilter(void)
    {
        throw std::runtime_error("LSParser::getFilter(): not yet implemented");
    }
*/

/*
    void LSParser::setFilter(LSParserFilterSP filter)
    {
        throw std::runtime_error("LSParser::setFilter(): not yet implemented");
    }
*/

/*
    bool LSParser::isAsync(void)
    {
        throw std::runtime_error("LSParser::isAsync(): not yet implemented");
    }
*/

/*
    bool LSParser::isBusy(void)
    {
        throw std::runtime_error("LSParser::isBusy(): not yet implemented");
    }
*/

    DocumentSP LSParser::parse(LSInputSP input)
    {
        if(!input->characterStream)
            throw std::runtime_error("LSParser::parse(): only characterStream implemented");

        _data->input = input;
        return _data->Document();
    }

/*
    DocumentSP LSParser::parseURI(const DOMString &uri)
    {
        throw std::runtime_error("LSParser::parseURI(): not yet implemented");
    }
*/

    NodeSP LSParser::parseWithContext(LSInputSP input, NodeSP contextArg, unsigned short action)
    {
        if(!input->characterStream)
            throw std::runtime_error("LSParser::parseWithContext(): only characterStream implemented");

        // exceptions per LS p22

        _data->input = input;

        // TODO:

        if(contextArg->getNodeType() == Node::DOCUMENT_NODE)
        {
            _data->document = std::dynamic_pointer_cast<Document>(contextArg);

            // parse as document


            return NodeSP();
        }

        _data->document = contextArg->getOwnerDocument();
        DocumentFragmentSP fragment = _data->DocumentFragment(_data->document);
        if(!fragment)
            return NodeSP();
        NodeList children = fragment->getChildNodes();
        for(NodeList::iterator it = children.begin(), end = children.end(); it != end; ++it)
            contextArg->appendChild(*it);
        return fragment;
    }

/*
    void LSParser::abort(void)
    {
        throw std::runtime_error("LSParser::abort(): not yet implemented");
    }
*/

}