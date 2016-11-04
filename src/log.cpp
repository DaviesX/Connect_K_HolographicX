#include <fstream>
#include <ostream>
#include "log.h"


class Log
{
public:
        Log()
        {
                m_file.open("log", std::fstream::out | std::fstream::trunc);
                if (!m_file.is_open())
                        throw "Failed to create log file";
                m_file << "Starting logging..." << std::endl;
        }

        ~Log()
        {
                m_file << "End logging..." << std::endl;
                m_file.close();
        }

        std::ostream& get_stream()
        {
                return m_file;
        }

        void flush()
        {
                m_file.close();
                m_file.open("log", std::fstream::out | std::fstream::app);
        }
private:
        std::ofstream   m_file;
};

// Singleton.
static Log log;

std::ostream& get_log_stream()
{
        return log.get_stream();
}

void flush()
{
        log.flush();
}
