#include <FrontierX/ProfileDocument.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace
{
void require(bool value, const char * message)
{
    if (!value) throw std::runtime_error(message);
}
struct Scratch
{
    std::filesystem::path path;
    Scratch()
    {
        for (int i = 0; i < 100; ++i)
        {
            path = std::filesystem::temp_directory_path() /
                ("frontierx-profile-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" + std::to_string(i));
            if (std::filesystem::create_directory(path)) return;
        }
        throw std::runtime_error("cannot create test directory");
    }
    ~Scratch() { std::error_code error; std::filesystem::remove_all(path, error); }
};
}
int main()
{
    try
    {
        FrontierX::ProfileDocument document;
        std::string error;
        require(document.Sections().empty() && !document.Value("a", "b"), "unloaded document");
        const char * text = "[Startup]\r\nDataPath= Data\r\nEmpty=\r\nBoundary=\r\n[Startup]\r\nDataPath=other";
        require(document.LoadText(text, error) && error.empty(), "load text");
        require(document.Sections() == std::vector<std::string>({"Startup", "Startup"}), "section enumeration");
        require(document.Value("STARTUP", "datapath") == "Data", "ASCII lookup/duplicate precedence");
        require(document.Value("Startup", "Empty") == " ", "empty sentinel");
        require(document.Value("Startup", "Boundary") == "\r", "legacy boundary CR changed");
        require(!document.Value("Startup", "absent"), "missing key");
        require(!document.LoadText(std::string("a\0b", 3), error), "embedded NUL accepted");
        require(!document.LoadText("\xff", error), "non-ASCII accepted");
        require(!document.LoadText(std::string(FrontierX::ProfileDocument::MaxBytes + 1, 'a'), error), "oversize accepted");
        require(document.Value("startup", "datapath") == "Data", "failed load changed state");
        auto copy = document;
        require(document.LoadText("", error) && document.Sections().empty(), "empty document");
        require(copy.Value("startup", "datapath") == "Data", "copy ownership");
        Scratch scratch;
        const auto path = scratch.path / "config.ini";
        { std::ofstream file(path, std::ios::binary); file << text; require(bool(file), "write fixture"); }
        require(document.LoadFile(path, error) && document.Value("startup", "datapath") == "Data", "file load");
        require(!document.LoadFile(scratch.path / "missing.ini", error) && !error.empty(), "missing file accepted");
        require(!document.LoadFile(scratch.path, error), "directory accepted");
        require(document.Value("startup", "datapath") == "Data", "file failure changed state");
        std::cout << "Owning profile document passed\n";
    }
    catch (const std::exception & error) { std::cerr << error.what() << '\n'; return 1; }
}
