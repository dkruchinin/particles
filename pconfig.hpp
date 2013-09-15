#ifndef _PCONFIG_HPP_
#define _PCONFIG_HPP_

#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <cerrno>
#include <exception>

struct PConfigEntry {
    double rx;
    double ry;
    double vx;
    double vy;
    double radius;
    int mass;
    int r;
    int g;
    int b;
};

class PConfigError : public std::runtime_error {
public:
    PConfigError(int lineNum, const std::string msg) :
        std::runtime_error(std::to_string(lineNum) + ": " + msg) {};
    virtual ~PConfigError() {};
};

// Quick&dirty config parser.
class PConfig {
private:
    int lineNum;
    std::ifstream ifile;

    template <class T>
    T readValue(std::istringstream &ss, const std::string &name,
                T floor, T ceil) {
        T val;

        ss >> val;
        if (ss.fail())
            throw PConfigError(lineNum, name + " was expected next, but " +
                               "got nothing");
        if (val < floor)
            throw PConfigError(lineNum, name + " can not be less than " +
                               std::to_string(floor));
        if (val > ceil)
            throw PConfigError(lineNum, name + " can not be greater than " +
                               std::to_string(ceil));

        return val;
    }

    
public:
    PConfig(const char *cfg_file);
    virtual ~PConfig();
    std::unique_ptr<PConfigEntry> nextEntry();
};

#endif /* _PCONFIG_HPP_ */
