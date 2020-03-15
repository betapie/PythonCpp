#pragma once
#ifndef PYTHON_INTERPRETER_H
#define PYTHON_INTERPRETER_H

/*
    This class resembles a handle to the Python interpreter which is created by PyInitialize().
    There is only one interpreter which has to be shared among all uses of the Python C API.
    Use the Open() method to before any other Python related code. Close() when done. Opening
    multiple times will increase the ref count of the interpreter handle. If all clients close,
    Python will be finalized. Else Python will be finalized on destruction of the App.
*/

#include "Python.h"
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
            PyInstance()
            {
                Py_Initialize();
                // TODO handle failure of initialization
            }
        public:
            ~PyInstance()
            {
                Py_Finalize();
            }
            PyInstance(const PyInstance& other) = delete;
            PyInstance& operator=(const PyInstance& other) = delete;
        };
    }

    //// Helper class to make a lock handle returnable. If the compiler chooses not to use RVO
    //// PtrLockGuard can be moved.
    //class PtrLockGuard
    //{
    //public:
    //    explicit PtrLockGuard(std::mutex& mutex)
    //        :m_pMutex(&mutex)
    //    {
    //        m_pMutex->lock();
    //    }

    //    ~PtrLockGuard() noexcept
    //    {
    //        if (m_pMutex)
    //            m_pMutex->unlock();
    //    }

    //    PtrLockGuard(const PtrLockGuard& other) = delete;
    //    PtrLockGuard& operator=(const PtrLockGuard& other) = delete;

    //    PtrLockGuard(PtrLockGuard&& other) noexcept
    //        :m_pMutex(other.m_pMutex)
    //    {
    //        other.m_pMutex = nullptr;
    //    }

    //    PtrLockGuard& operator=(PtrLockGuard&& other) noexcept
    //    {
    //        if (this == &other)
    //            return *this;

    //        m_pMutex = other.m_pMutex;
    //        other.m_pMutex = nullptr;
    //        return *this;
    //    }

    //    void Unlock()
    //    {
    //        if (m_pMutex)
    //        {
    //            m_pMutex->unlock();
    //            m_pMutex = nullptr;
    //        }
    //    }

    //private:
    //    std::mutex* m_pMutex;
    //};

    class PythonInterpreter
    {
    public:
        static void Open();
        static void Close();

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