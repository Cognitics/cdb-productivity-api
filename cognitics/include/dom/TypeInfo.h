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
/*! \file dom/TypeInfo.h
\headerfile dom/TypeInfo.h
\brief Provides dom::TypeInfo.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::TypeInfo TypeInfo.h TypeInfo.h
\brief Type Information

The TypeInfo interface represents a type referenced from Element or Attr nodes, specified in the schemas associated with the document.
The type is a pair of a namespace URI and name properties, and depends on the document's schema.

If the document's schema is an XML DTD, the values are computed as follows:
\li If this type is referenced from an Attr node, typeNamespace is "http://www.w3.org/TR/REC-xml" and typeName represents the attribute type property in the XML Information Set. If there is no declaration for the attribute, typeNamespace and typeName are null.
\li If this type is referenced from an Element node, typeNamespace and typeName are null.

If the document's schema is an XML Schema, the values are computed as follows using the post-schema-validation infoset contributions (also called PSVI contributions):
\li If the [validity] property exists AND is "invalid" or "notKnown": the {target namespace} and {name} properties of the declared type if available, otherwise null.
\note At the time of writing, the XML Schema specification does not require exposing the declared type.
Thus, DOM implementations might choose not to provide type information if validity is not valid.

\li If the [validity] property exists and is "valid":
-# If [member type definition] exists:
    -# If {name} is not absent, then expose {name} and {target namespace} properties of the [member type definition] property;
    -# Otherwise, expose the namespace and local name of the corresponding anonymous type name.
-# If the [type definition] property exists:
    -# If {name} is not absent, then expose {name} and {target namespace} properties of the [type definition] property;
    -# Otherwise, expose the namespace and local name of the corresponding anonymous type name.
-# If the [member type definition anonymous] exists:
    -# If it is false, then expose [member type definition name] and [member type definition namespace] properties;
    -# Otherwise, expose the namespace and local name of the corresponding anonymous type name.
-# If the [type definition anonymous] exists:
    -# If it is false, then expose [type definition name] and [type definition namespace] properties;
    -# Otherwise, expose the namespace and local name of the corresponding anonymous type name.

\note Other schema languages are outside the scope of the W3C and therefore should define how to represent their type systems using TypeInfo.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p99]
\note Introduced in DOM Level 3.
*/
    class TypeInfo
    {
    private:
        struct _TypeInfo;
        _TypeInfo *_data;

    public:
        virtual ~TypeInfo(void);
        TypeInfo(void);

        static const unsigned short DERIVATION_RESTRICTION    = 0x01;
        static const unsigned short DERIVATION_EXTENSION    = 0x02;
        static const unsigned short DERIVATION_UNION        = 0x04;
        static const unsigned short DERIVATION_LIST            = 0x08;

/*! \brief Get the name of the type declared for the associated element or attribute.
\return The name of the type declared for the associated element or attribute, or empty if unknown.
*/
        virtual DOMString getTypeName(void);
        virtual void setTypeName(const DOMString &typeName);

/*! \brief Get the namespace of the type declared for the associated element or attribute.
\return The namespace of the type declared for the associated element or attribute, or empty if the element does not have declaration or if no namespace information is available.
*/
        virtual DOMString getTypeNamespace(void);
        virtual void setTypeNamespace(const DOMString &typeNamespace);

/*! \brief Check if there is a derivation between the reference type definition and the specified type definition.

This method returns if there is a derivation between the reference type definition, i.e. the TypeInfo on which the method is being called, and the other type definition, i.e. the one passed as parameters.

The following is a set of possible types of derivation, and the values represent bit positions.
If a bit in the derivationMethod parameter is set to 1, the corresponding type of derivation will be taken into account when evaluating the derivation between the reference type definition and the other type definition.
When using the isDerivedFrom method, combining all of them in the derivationMethod parameter is equivalent to invoking the method for each of them separately and combining the results with the OR boolean function.
This specification only defines the type of derivation for XML Schema.

In addition to the types of derivation listed below, please note that:
\li any type derives from xsd:anyType.
\li any simple type derives from xsd:anySimpleType by restriction.
\li any complex type does not derive from xsd:anySimpleType by restriction.

Defined Constants:
\li DERIVATION_EXTENSION: If the document's schema is an XML Schema, this constant represents the derivation by extension.
The reference type definition is derived by extension from the other type definition if the other type definition can be reached recursively following the {base type definition} property from the reference type definition, and at least one of the derivation methods involved is an extension.
\li DERIVATION_LIST: If the document's schema is an XML Schema, this constant represents the list.
The reference type definition is derived by list from the other type definition if there exists two type definitions T1 and T2 such as the reference type definition is derived from T1 by DERIVATION_RESTRICTION or DERIVATION_EXTENSION, T2 is derived from the other type definition by DERIVATION_RESTRICTION, T1 has {variety} list, and T2 is the {item type definition}.
Note that T1 could be the same as the reference type definition, and T2 could be the same as the other type definition.
\li DERIVATION_RESTRICTION: If the document's schema is an XML Schema, this constant represents the derivation by restriction if complex types are involved, or a restriction if simple types are involved.
The reference type definition is derived by restriction from the other type definition if the other type definition is the same as the reference type definition, or if the other type definition can be reached recursively following the {base type definition} property from the reference type definition, and all the derivation methods involved are restriction.
\li DERIVATION_UNION: If the document's schema is an XML Schema, this constant represents the union if simple types are involved.
The reference type definition is derived by union from the other type definition if there exists two type definitions T1 and T2 such as the reference type definition is derived from T1 by DERIVATION_RESTRICTION or DERIVATION_EXTENSION, T2 is derived from the other type definition by DERIVATION_RESTRICTION, T1 has {variety} union, and one of the {member type definitions} is T2.
Note that T1 could be the same as the reference type definition, and T2 could be the same as the other type definition.

\param typeNamespaceArg The namespace of the other type definition.
\param typeNameArg The name of the other type definition.
\param derivationMethod The type of derivation and conditions applied between two types, as described in the list of constants provided in this interface.
\return If the document's schema is a DTD or no schema is associated with the document, this method will always return false.
If the document's schema is an XML Schema, the method will true if the reference type definition is derived from the other type definition according to the derivation parameter.
If the value of the parameter is 0 (no bit is set to 1 for the derivationMethod parameter), the method will return true if the other type definition can be reached by recursing any combination of {base type definition}, {item type definition}, or {member type definitions} from the reference type definition.
*/
        // TODO: virtual bool isDerivedFrom(const DOMString &typeNamespaceArg, const DOMString &typeNameArg, unsigned long derivationMethod);

    };


}
