#include <tinyxml.h>

#include <cstring>
#include <cwchar>
#include <iostream>
#include <limits>
#include <string>

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

	const wchar_t * unicode = L"\u03a9\U0001f642";
	item->SetAttribute("unicode", unicode);
	if (!require(std::strcmp(item->Attribute("unicode"), "03a9d83dde420000") == 0,
		"TinyXML did not preserve Win32 UTF-16 attribute units"))
		return 7;
	wchar_t * decoded = nullptr;
	if (!require(item->QueryUnicodeValue("unicode", &decoded) == TIXML_SUCCESS &&
			decoded && std::wcscmp(decoded, unicode) == 0,
		"TinyXML did not convert UTF-16 attribute units to native wide text"))
		return 8;

	const std::wstring longText(5000, L'x');
	item->SetAttribute("longUnicode", longText.c_str());
	if (!require(std::strlen(item->Attribute("longUnicode")) == 20004 &&
			item->QueryUnicodeValue("longUnicode", &decoded) == TIXML_SUCCESS &&
			decoded && longText == decoded,
		"TinyXML truncated a wide attribute beyond the former fixed buffer"))
		return 9;

	item->SetAttribute("invalidUnicode", "03ag0000");
	if (!require(item->QueryUnicodeValue("invalidUnicode", &decoded) == TIXML_WRONG_TYPE &&
			decoded == nullptr,
		"TinyXML accepted malformed hexadecimal wide text"))
		return 10;
	item->SetAttribute("invalidUnicode", "d83d0000");
	if (!require(item->QueryUnicodeValue("invalidUnicode", &decoded) == TIXML_WRONG_TYPE &&
			decoded == nullptr,
		"TinyXML accepted an unpaired UTF-16 high surrogate"))
		return 11;

	TiXmlDocument malformed;
	malformed.Parse("<root><item></root>");
	if (!require(malformed.Error(), "TinyXML accepted mismatched closing tags"))
		return 6;

	return 0;
}
