#include "Interpreter.h"

size_t pycpp::Interpreter::s_refCnt = 0;
std::mutex pycpp::Interpreter::s_mutex{};
std::unique_ptr<pycpp::detail::PyInstance> pycpp::Interpreter::s_pInterpreter{};

pycpp::detail::PyInstance::PyInstance()
{
    Py_Initialize();
    // TODO handle failure of initialization
}

pycpp::detail::PyInstance::~PyInstance()
{
    Py_Finalize();
}

pycpp::InterpreterHandle::InterpreterHandle()
{
    Interpreter::Open();
}

pycpp::InterpreterHandle::~InterpreterHandle()
{
    if (_owns)
        Interpreter::Close();
}

pycpp::InterpreterHandle::InterpreterHandle(const InterpreterHandle& other)
{
    _owns = other._owns;
    if (_owns)
        Interpreter::Open();
}

pycpp::InterpreterHandle& pycpp::InterpreterHandle::operator=(const InterpreterHandle& other)
{
    if (_owns)
        Interpreter::Close();
    _owns = other._owns;
    if (_owns)
        Interpreter::Open();
    return *this;
}

pycpp::InterpreterHandle::InterpreterHandle(InterpreterHandle&& other) noexcept
{
    _owns = other._owns;
    other._owns = false;
}

pycpp::InterpreterHandle& pycpp::InterpreterHandle::operator=(InterpreterHandle&& other) noexcept
{
    if (_owns)
        Interpreter::Close();
    _owns = other._owns;
    other._owns = false;
    return *this;
}

void pycpp::InterpreterHandle::Release()
{
    if (_owns)
        Interpreter::Close();
    _owns = false;
}

void pycpp::InterpreterHandle::Aquire()
{
    if (!_owns)
        Interpreter::Open();
    _owns = true;
}

bool pycpp::InterpreterHandle::Aqurired() noexcept
{
    return _owns;
}

void pycpp::Interpreter::Open()
{
    if (s_refCnt == 0)
    {
        s_pInterpreter = std::make_unique<detail::PyInstance>();
    }
    ++s_refCnt;
}

void pycpp::Interpreter::Close()
{
    if (s_refCnt > 0)
        --s_refCnt;
    if (s_refCnt == 0 && s_pInterpreter)
    {
        s_pInterpreter.reset();
    }
}

pycpp::InterpreterHandle pycpp::Interpreter::Handle()
{
    return InterpreterHandle();
}

[[nodiscard]]
auto pycpp::Interpreter::getLock()
{
    return std::unique_lock<decltype(s_mutex)>(s_mutex);
}
