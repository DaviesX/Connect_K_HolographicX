#include <fstream>
#include <ostream>
#include <log.h>


class Log
{
public:
        Log()
        {
                m_file.open("log", std::ios::trunc);
                if (!m_file.is_open())
                        throw "Failed to create log file";
        }

        ~Log()
        {
                m_file.close();
        }

        std::ostream& get_stream()
        {
                return m_file;
        }

        void flush()
        {
                m_file.close();
                m_file.open("log", std::ios::out | std::ios::ate);
        }
private:
        std::ofstream   m_file;
};

// Singleton.
static Log log;

std::ostream& get_log_stream()
{
        log.get_stream();
}

void flush()
{
        log.flush();
}
