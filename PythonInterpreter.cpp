#include "PythonInterpreter.h"

size_t pycpp::PythonInterpreter::s_refCnt = 0;
std::mutex pycpp::PythonInterpreter::s_mutex{};
std::unique_ptr<pycpp::detail::PyInstance> pycpp::PythonInterpreter::s_pInterpreter{};

void pycpp::PythonInterpreter::Open()
{
    if (s_refCnt == 0)
    {
        s_pInterpreter = std::make_unique<detail::PyInstance>();
    }
    ++s_refCnt;
}

void pycpp::PythonInterpreter::Close()
{
    if (s_refCnt > 0)
        --s_refCnt;
    if (s_refCnt == 0 && s_pInterpreter)
    {
        s_pInterpreter.reset();
    }
}

[[nodiscard]]
auto pycpp::PythonInterpreter::getLock()
{
    return std::unique_lock<decltype(s_mutex)>(s_mutex);
}
