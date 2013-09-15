#include <string>
#include <fstream>
#include "pconfig.hpp"

PConfig::PConfig(const char *cfg_file)
{
    ifile.open(cfg_file, std::ifstream::in);

    // oh, how I hate this ridiculous error reporting
    // in C++ streams. Even more than I hate streams
    // themselves.
    if (ifile.fail())
        throw std::ios_base::failure(strerror(errno));

    lineNum = 0;
}

PConfig::~PConfig()
{
    ifile.close();
}

std::unique_ptr<PConfigEntry> PConfig::nextEntry()
{
    std::string line;

    while (true) {
        if (ifile.eof())
            return nullptr;

        std::getline(ifile, line);
        lineNum++;

        if (line == "")
            return nullptr;
        if (line[0] != '#')
            break;
    }

    std::istringstream ss(line);
    PConfigEntry *entry = new PConfigEntry();

    entry->rx = readValue<double>(ss, "X coordinate", 0.0, 1.0);
    entry->ry = readValue<double>(ss, "Y coordinate", 0.0, 1.0);
    entry->vx = readValue<double>(ss, "X velocity", -1.0, 1.0);
    entry->vy = readValue<double>(ss, "Y velocity", -1.0, 1.0);
    entry->mass = readValue<int>(ss, "Mass", 1, 100);
    entry->radius = readValue<double>(ss, "Radius", 0.0, 1.0);
    entry->r = readValue<int>(ss, "Red value", 0, 255);
    entry->g = readValue<int>(ss, "Green value", 0, 255);
    entry->b = readValue<int>(ss, "Blue value", 0, 255);

    return std::unique_ptr<PConfigEntry>(entry);
}
