/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

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

#include "ModelKit/Connection.h"

namespace modelkit
{
	Connection::~Connection(void)
	{
	}

	Connection::Connection(void)
	{
	}

	Connection::Connection(Attachment *a, Attachment *b)
	{
		attachments.push_back(a);
		attachments.push_back(b);
	}

	Connection::Connection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB)
	{
		attachments.push_back(new Attachment(crossSectionReferenceA, attachmentPointNameA));
		attachments.push_back(new Attachment(crossSectionReferenceB, attachmentPointNameB));
	}

	void Connection::addAttachment(Attachment *attachment)
	{
		attachments.push_back(attachment);
	}

	Attachment *Connection::addAttachment(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName)
	{
		Attachment *attachment = new Attachment(crossSectionReference, attachmentPointName);
		addAttachment(attachment);
		return attachment;
	}

	std::vector<Attachment *> Connection::getAttachments(void)
	{
		return attachments;
	}

	Attachment* Connection::getAttachment(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName)
	{
		for(std::vector<Attachment *>::iterator it = attachments.begin(), end = attachments.end(); it != end; ++it)
		{
			if( (*it)->getCrossSectionReference() == crossSectionReference && (*it)->getAttachmentPointName() == attachmentPointName )
            {
                return *it;
            }
		}
		return NULL;
	}
    
    bool Connection::hasAttachment(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName)
	{
		for(std::vector<Attachment *>::iterator it = attachments.begin(), end = attachments.end(); it != end; ++it)
		{
			if((*it)->getCrossSectionReference() != crossSectionReference)
				continue;
			if(attachmentPointName.empty())
				return true;
			if((*it)->getAttachmentPointName() == attachmentPointName)
				return true;
		}
		return false;
	}



}