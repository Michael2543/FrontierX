#ifndef FRONTIERX_M3_SEMANTIC_WRITER_H
#define FRONTIERX_M3_SEMANTIC_WRITER_H

#include "M3StateOutput.h"
#include <DataParser.h>
#include "../../../Libs/Src/Viewer/DOCUVIEW/ViewSymbolOperations.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace FrontierXM3
{
class SemanticWriter
{
    M3StateOutput output;
    void *context;
public:
    SemanticWriter(M3StateOutput sink, void *target) : output(sink), context(target) {}
    void Text(const char *text) { output(text, static_cast<U32>(std::strlen(text)), context); }
    void String(const char *text)
    {
        Text("\"");
        for (; text && *text; ++text)
        {
            const unsigned char c = static_cast<unsigned char>(*text);
            char escaped[8];
            if (c == '"' || c == '\\')
            { escaped[0] = '\\'; escaped[1] = *text; escaped[2] = 0; Text(escaped); }
            else if (c < 32)
            { std::snprintf(escaped, sizeof(escaped), "\\u%04x", c); Text(escaped); }
            else output(text, 1, context);
        }
        Text("\"");
    }
    void Number(U32 value)
    { char buffer[16]; std::snprintf(buffer, sizeof(buffer), "%u", value); Text(buffer); }

    bool Fields(IDataParser *parser, const void *bytes, U32 length,
                bool partReference = false, U32 depth = 0, SYMBOL schema = nullptr)
    {
        if (!parser || !bytes || depth > 32) return false;
        struct Capture : IParserCallback
        {
            SemanticWriter &writer;
            const unsigned char *bytes;
            U32 length, depth, count = 0, partFields = 0;
            bool reference;
            SYMBOL member;
            bool array;
            Capture(SemanticWriter &w, const void *data, U32 size, bool ref, U32 nesting, SYMBOL type)
                : writer(w), bytes(static_cast<const unsigned char *>(data)),
                  length(size), depth(nesting), reference(ref),
                  member(type ? ResolveViewSymbolType(type)->datatype : nullptr),
                  array(type && ResolveViewSymbolType(type)->kind == ARRAYSYM) {}
            BOOL32 COMAPI VarInstance(IDataParser *nested, const VARIABLEDESC &field) override
            {
                const SYMBOL type = member ? ResolveViewSymbolType(member) : nullptr;
                if (member && !array) member = member->link;
                const bool unnamedEnum = !field.typeName && type && type->kind == ENUMSYM &&
                    type->size == static_cast<int>(field.size);
                if (field.offset > length || field.size > length - field.offset)
                {
                    std::fprintf(stderr, "M3 semantic field bounds: %.64s offset=%u size=%u extent=%u\n",
                        field.varName ? field.varName : "(array item)", field.offset, field.size, length);
                    return FALSE;
                }
                if (reference)
                {
                    ++partFields;
                    // Cache slot and debugger-only pointers are not mission identity.
                    if (!field.varName) return FALSE;
                    if (std::strcmp(field.varName, "dwMissionID"))
                    {
                        return (!std::strcmp(field.varName, "index") ||
                                !std::strcmp(field.varName, "pSave") ||
                                !std::strcmp(field.varName, "pInit")) && field.size == 4;
                    }
                    if (field.offset != 4 || field.size != 4 || nested) return FALSE;
                }
                if (count++) writer.Text(",");
                writer.Text("{\"name\":"); writer.String(field.varName ? field.varName : "");
                writer.Text(",\"type\":"); writer.String(field.typeName ? field.typeName :
                    (unnamedEnum ? "<anonymous enum>" : ""));
                writer.Text(",\"value\":");
                if (nested)
                {
                    if (!writer.Fields(nested, bytes + field.offset, field.size,
                            field.typeName && !std::strcmp(field.typeName, "MPartRef"), depth + 1, type))
                        return FALSE;
                }
                else
                {
                    if (field.kind != VARIABLEDESC::BASIC || (!field.typeName && !unnamedEnum) ||
                            (field.typeName && std::strchr(field.typeName, '*')) ||
                            (type && type->kind == POINTERSYM) || !field.size || field.size > 8 ||
                            field.bitOffset >= 64 || field.bitCount > 64 - field.bitOffset)
                    {
                        std::fprintf(stderr, "M3 semantic scalar: %.64s type=%.64s kind=%u size=%u bit=%u/%u\n",
                            field.varName ? field.varName : "(array item)",
                            field.typeName ? field.typeName : "(unnamed)",
                            static_cast<unsigned int>(field.kind), field.size, field.bitOffset, field.bitCount);
                        return FALSE;
                    }
                    std::uint64_t value = 0;
                    const U32 span = field.bitCount
                        ? (field.bitOffset + field.bitCount + 7U) / 8U : field.size;
                    const U32 readSize = span > field.size ? span : field.size;
                    if (readSize > 8 || readSize > length - field.offset) return FALSE;
                    std::memcpy(&value, bytes + field.offset, readSize);
                    if (field.bitCount)
                    {
                        value >>= field.bitOffset;
                        if (field.bitCount < 64) value &= (UINT64_C(1) << field.bitCount) - 1;
                    }
                    char bits[24];
                    std::snprintf(bits, sizeof(bits), "\"%016llx\"", static_cast<unsigned long long>(value));
                    writer.Text(bits);
                    writer.Text(",\"width\":"); writer.Number(field.size);
                    writer.Text(",\"bits\":"); writer.Number(field.bitCount);
                }
                writer.Text("}");
                return TRUE;
            }
        } capture(*this, bytes, length, partReference, depth, schema);
        Text("[");
        const bool valid = parser->Enumerate(&capture) != FALSE &&
            (!partReference || (capture.partFields == 4 && capture.count == 1));
        if (!valid && partReference)
            std::fprintf(stderr, "M3 semantic part reference fields=%u identity=%u extent=%u\n",
                capture.partFields, capture.count, length);
        Text("]");
        return valid;
    }
};
}
#endif
