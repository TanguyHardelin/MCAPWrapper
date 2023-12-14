#ifndef MCAP_FILE_WRITER_HPP
#define MCAP_FILE_WRITER_HPP

#include <string>
#include <iostream>
#include "mcap/writer.hpp"

namespace mcap_wrapper
{
    class MCAPFileWriter
    {
    public:
        MCAPFileWriter();
        bool open(std::string file_name);

    protected:
        mcap::FileWriter _file_writer;
    };
};

#endif