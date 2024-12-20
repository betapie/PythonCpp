#pragma once
#ifndef PYCPP_INTERPRETER_H
#define PYCPP_INTERPRETER_H

/*
    This class resembles a handle to the Python interpreter which is created by PyInitialize().
    There is only one interpreter which has to be shared among all uses of the Python C API.
    Use the Open() method to before any other Python related code. Close() when done. Opening
    multiple times will increase the ref count of the interpreter handle. If all clients close,
    Python will be finalized. Else Python will be finalized on destruction of the App.
*/

#include "Python.h"
#include "Defines.h"
#include <mutex>
#include <memory>

namespace pycpp
{
    namespace detail
    {
        class PyInstance
        {
            friend class PythonInterpreter;
            friend std::unique_ptr<PyInstance> std::make_unique<PyInstance>();
        private:
            PyInstance();
        public:
            ~PyInstance();
            PyInstance(const PyInstance& other) = delete;
            PyInstance& operator=(const PyInstance& other) = delete;
        };
    }

    class PYCPP_API InterpreterHandle
    {
        friend class Interpreter;
    private:
        InterpreterHandle();
    public:
        ~InterpreterHandle();

        InterpreterHandle(const InterpreterHandle& other);
        InterpreterHandle& operator=(const InterpreterHandle& other);
        InterpreterHandle(InterpreterHandle&& other) noexcept;
        InterpreterHandle& operator=(InterpreterHandle&& other) noexcept;

        void Release();
        void Aquire();

        [[nodiscard]] bool Aqurired() noexcept;

    private:
        bool _owns = true;
    };

    class PYCPP_API Interpreter
    {
    public:
        static void Open();
        static void Close();
        static InterpreterHandle Handle();

        /* 
            if you want to access the interpreter from multiple threads,
            retrieve the lock for each task first. Unsynchronized use of
            the interpreter will lead to undefined behaviour. Any multithreading 
            in the Python interpreter should be handled from the Python code itself
        */
        [[nodiscard]] static auto getLock();

    private:
        static size_t s_refCnt;
        static std::mutex s_mutex;
        static std::unique_ptr<detail::PyInstance> s_pInterpreter;
    };
}

#endif // PYTHON_INTERPRETER_H
