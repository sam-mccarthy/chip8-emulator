#pragma once

#include <string>

class Octo {
public:
    Octo();
    Octo(const std::string& filename);
    ~Octo();

    void load_string(const std::string& program);
    void load_file(const std::string& filename);
    void run();
};