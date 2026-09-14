#include <tinyxml.h>

#include <cstring>
#include <iostream>
#include <limits>

namespace
{
bool require(bool condition, const char * message)
{
	if (!condition)
	{
		std::cerr << message << '\n';
	}
	return condition;
}
}

int main()
{
	const char * xml =
		"<?xml version=\"1.0\"?>"
		"<root version=\"1.0\"><item name=\"test\">42 &amp; ready</item></root>";

	TiXmlDocument document;
	document.Parse(xml);
	if (!require(!document.Error(), "TinyXML failed to parse a valid document"))
		return 1;

	TiXmlElement * root = document.RootElement();
	if (!require(root != 0 && std::strcmp(root->Value(), "root") == 0,
		"TinyXML did not produce the expected root element"))
		return 2;
	if (!require(root->Attribute("version") != 0 &&
			std::strcmp(root->Attribute("version"), "1.0") == 0,
		"TinyXML did not preserve the root attribute"))
		return 3;

	TiXmlElement * item = root->FirstChildElement("item");
	TiXmlNode * textNode = item ? item->FirstChild() : 0;
	if (!require(textNode != 0 && textNode->ToText() != 0 &&
			std::strcmp(textNode->Value(), "42 & ready") == 0,
		"TinyXML did not decode the expected text entity"))
		return 4;

	const unsigned long expectedWideValue = std::numeric_limits<unsigned long>::max();
	item->SetAttribute("wide", expectedWideValue);
	unsigned long actualWideValue = 0;
	if (!require(item->QueryUnsignedIntValue("wide", &actualWideValue) == TIXML_SUCCESS &&
			actualWideValue == expectedWideValue,
		"TinyXML failed the unsigned long attribute round trip"))
		return 5;

	TiXmlDocument malformed;
	malformed.Parse("<root><item></root>");
	if (!require(malformed.Error(), "TinyXML accepted mismatched closing tags"))
		return 6;

	return 0;
}