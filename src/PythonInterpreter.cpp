#include "PythonInterpreter.h"

size_t pycpp::PythonInterpreter::s_refCnt = 0;
std::mutex pycpp::PythonInterpreter::s_mutex{};
std::unique_ptr<pycpp::detail::PyInstance> pycpp::PythonInterpreter::s_pInterpreter{};

pycpp::detail::PyInstance::PyInstance()
{
    Py_Initialize();
    // TODO handle failure of initialization
}

pycpp::detail::PyInstance::~PyInstance()
{
    Py_Finalize();
}

pycpp::PythonInterpreterHandle::PythonInterpreterHandle()
{
    PythonInterpreter::Open();
}

pycpp::PythonInterpreterHandle::~PythonInterpreterHandle()
{
    if (_owns)
        PythonInterpreter::Close();
}

pycpp::PythonInterpreterHandle::PythonInterpreterHandle(const PythonInterpreterHandle& other)
{
    _owns = other._owns;
    if (_owns)
        PythonInterpreter::Open();
}

pycpp::PythonInterpreterHandle& pycpp::PythonInterpreterHandle::operator=(const PythonInterpreterHandle& other)
{
    if (_owns)
        PythonInterpreter::Close();
    _owns = other._owns;
    if (_owns)
        PythonInterpreter::Open();
    return *this;
}

pycpp::PythonInterpreterHandle::PythonInterpreterHandle(PythonInterpreterHandle&& other) noexcept
{
    _owns = other._owns;
    other._owns = false;
}

pycpp::PythonInterpreterHandle& pycpp::PythonInterpreterHandle::operator=(PythonInterpreterHandle&& other) noexcept
{
    if (_owns)
        PythonInterpreter::Close();
    _owns = other._owns;
    other._owns = false;
    return *this;
}

void pycpp::PythonInterpreterHandle::Release()
{
    if (_owns)
        PythonInterpreter::Close();
    _owns = false;
}

void pycpp::PythonInterpreterHandle::Aquire()
{
    if (!_owns)
        PythonInterpreter::Open();
    _owns = true;
}

bool pycpp::PythonInterpreterHandle::Aqurired() noexcept
{
    return _owns;
}

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

pycpp::PythonInterpreterHandle pycpp::PythonInterpreter::Handle()
{
    return PythonInterpreterHandle();
}

[[nodiscard]]
auto pycpp::PythonInterpreter::getLock()
{
    return std::unique_lock<decltype(s_mutex)>(s_mutex);
}
