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
private:
    int line;
    int col;
public:
    PConfigError(int line, int col, const std::string msg) :
        std::runtime_error(msg), line(line), col(col) {};
    virtual ~PConfigError() {};

    int getLine() const {
        return line;
    }

    int getColumn() const {
        return col;
    }
};

// Quick&dirty config parser.
class PConfig {
private:
    int colNum;
    int lineNum;
    std::ifstream ifile;

    template <class T>
    T readValue(std::istringstream &ss, const std::string &name,
                T floor, T ceil) {
        T val;

        ss >> val;
        if (ss.fail())
            throw PConfigError(lineNum, colNum, name + " was expected next, but " +
                               "got nothing");
        if (val < floor)
            throw PConfigError(lineNum, colNum, name + " can not be less than " +
                               std::to_string(floor));
        if (val > ceil)
            throw PConfigError(lineNum, colNum, name + " can not be greater than " +
                               std::to_string(ceil));

        colNum++;
        return val;
    }

    
public:
    PConfig(const char *cfg_file);
    virtual ~PConfig();
    std::unique_ptr<PConfigEntry> nextEntry();
    static const char *formatString();
};

#endif /* _PCONFIG_HPP_ */
