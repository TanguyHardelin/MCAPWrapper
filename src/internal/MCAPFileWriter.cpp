#include "internal/MCAPFileWriter.h"

namespace mcap_wrapper
{
    MCAPFileWriter::MCAPFileWriter()
    {
    }
    bool MCAPFileWriter::open(std::string file_name)
    {
        mcap::Status open_status = _file_writer.open(file_name);
        if(open_status.code == mcap::StatusCode::Success)
            return true;
        else    
            std::cerr << "Error occur during the initialization of file " << file_name << ". Error message: " << open_status.message << std::endl;
        return false;
    }

};