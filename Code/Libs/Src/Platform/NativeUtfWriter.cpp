#include "FrontierX/UtfMetadata.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <new>
#include <string>
#include <vector>

namespace FrontierX
{
namespace
{
constexpr std::size_t HeaderBytes = 56;
constexpr std::size_t EntryBytes = 44;
constexpr std::size_t MaxArchiveBytes = 256U * 1024U * 1024U;

struct Node
{
    std::string name;
    bool directory = true;
    bool explicitItem = false;
    const std::vector<unsigned char> * payload = nullptr;
    std::map<std::string, std::unique_ptr<Node>> children;
    std::vector<Node *> childOrder;
    std::uint32_t index = 0;
    std::uint32_t nameOffset = 0;
    std::uint32_t dataOffset = 0;
};

void PutWord(std::vector<unsigned char> & bytes, std::size_t offset, std::uint32_t value)
{
    for (unsigned shift = 0; shift < 32; shift += 8)
        bytes[offset + shift / 8] = static_cast<unsigned char>(value >> shift);
}

bool ValidPart(const std::string & part)
{
    return !part.empty() && part != "." && part != ".." &&
        part.find('\\') == std::string::npos &&
        part.find('\0') == std::string::npos;
}

void Flatten(Node & node, std::vector<Node *> & ordered)
{
    node.index = static_cast<std::uint32_t>(ordered.size());
    ordered.push_back(&node);
    for (Node *child : node.childOrder) Flatten(*child, ordered);
}
}

bool EncodeUtfArchive(const std::vector<UtfArchiveItem> & items,
                      std::vector<unsigned char> & output, std::string & error)
{
    try
    {
        Node root;
        for (const UtfArchiveItem & item : items)
        {
            if (item.path.empty() || item.path.size() > 4096 ||
                item.path.front() == '/' || item.path.back() == '/' ||
                (item.directory && !item.bytes.empty()))
            { error = "invalid UTF writer item"; return false; }
            Node * parent = &root;
            std::size_t start = 0;
            while (start < item.path.size())
            {
                const std::size_t end = item.path.find('/', start);
                const std::string part = item.path.substr(start, end - start);
                if (!ValidPart(part) || !parent->directory)
                { error = "invalid UTF writer path"; return false; }
                auto & slot = parent->children[part];
                if (!slot)
                {
                    slot = std::make_unique<Node>();
                    slot->name = part;
                    // Original DOSFile DirectoryManager::addChildEntry appends
                    // siblings. Their stored order drives source enumeration.
                    parent->childOrder.push_back(slot.get());
                }
                Node * current = slot.get();
                if (end == std::string::npos)
                {
                    if (current->explicitItem ||
                        (!item.directory && !current->children.empty()))
                    { error = "duplicate or conflicting UTF writer item"; return false; }
                    current->explicitItem = true;
                    if (!item.directory)
                    {
                        current->directory = false;
                        current->payload = &item.bytes;
                    }
                    break;
                }
                parent = current;
                start = end + 1;
            }
        }

        std::vector<Node *> ordered;
        Flatten(root, ordered);
        if (ordered.size() > 65536)
        { error = "UTF writer entry limit exceeded"; return false; }
        std::vector<unsigned char> names(1, 0);
        std::size_t payloadSize = 0;
        for (Node * node : ordered)
        {
            if (node != &root)
            {
                node->nameOffset = static_cast<std::uint32_t>(names.size());
                names.insert(names.end(), node->name.begin(), node->name.end());
                names.push_back(0);
            }
            if (node->payload)
            {
                if (node->payload->size() > 64U * 1024U * 1024U ||
                    payloadSize > MaxArchiveBytes - node->payload->size())
                { error = "UTF writer payload limit exceeded"; return false; }
                node->dataOffset = static_cast<std::uint32_t>(payloadSize);
                payloadSize += node->payload->size();
            }
        }
        const std::size_t directorySize = ordered.size() * EntryBytes;
        const std::size_t dataOffset = HeaderBytes + directorySize + names.size();
        if (directorySize + names.size() > 64U * 1024U * 1024U ||
            dataOffset > MaxArchiveBytes || payloadSize > MaxArchiveBytes - dataOffset ||
            dataOffset + payloadSize > std::numeric_limits<std::uint32_t>::max())
        { error = "UTF writer metadata or archive limit exceeded"; return false; }
        std::vector<unsigned char> encoded(dataOffset + payloadSize, 0);
        for (std::size_t index = 0; index < ordered.size(); ++index)
        {
            const Node & node = *ordered[index];
            const std::size_t at = HeaderBytes + index * EntryBytes;
            PutWord(encoded, at + 4, node.nameOffset);
            PutWord(encoded, at + 8, node.directory ? 0x10U : 0x80U);
            if (node.directory)
            {
                if (!node.childOrder.empty())
                    PutWord(encoded, at + 16,
                        node.childOrder.front()->index * static_cast<std::uint32_t>(EntryBytes));
                for (std::size_t child = 0; child < node.childOrder.size(); ++child)
                {
                    if (child + 1 < node.childOrder.size())
                        PutWord(encoded, HeaderBytes + node.childOrder[child]->index * EntryBytes,
                            node.childOrder[child + 1]->index * static_cast<std::uint32_t>(EntryBytes));
                }
            }
            else
            {
                const auto size = static_cast<std::uint32_t>(node.payload->size());
                PutWord(encoded, at + 16, node.dataOffset);
                PutWord(encoded, at + 20, size);
                PutWord(encoded, at + 24, size);
                PutWord(encoded, at + 28, size);
                if (size)
                    std::copy(node.payload->begin(), node.payload->end(),
                              encoded.begin() + dataOffset + node.dataOffset);
            }
        }
        PutWord(encoded, 0, 0x20465455U);
        PutWord(encoded, 4, 0x101U);
        PutWord(encoded, 8, static_cast<std::uint32_t>(HeaderBytes));
        PutWord(encoded, 12, static_cast<std::uint32_t>(directorySize));
        PutWord(encoded, 20, static_cast<std::uint32_t>(EntryBytes));
        PutWord(encoded, 24, static_cast<std::uint32_t>(HeaderBytes + directorySize));
        PutWord(encoded, 28, static_cast<std::uint32_t>(names.size()));
        PutWord(encoded, 32, static_cast<std::uint32_t>(names.size()));
        PutWord(encoded, 36, static_cast<std::uint32_t>(dataOffset));
        std::copy(names.begin(), names.end(), encoded.begin() + HeaderBytes + directorySize);
        output.swap(encoded);
        error.clear();
        return true;
    }
    catch (const std::bad_alloc &)
    {
        error = "UTF writer allocation failed";
        return false;
    }
}
}
