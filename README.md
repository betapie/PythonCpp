# PythonCpp
Working with the Python C API can be quite a hassle. Some API functions will give you a handle to a Python object that has to be ref counted, some will give you borrowed references and some will steal them. Additionally pretty much any non trivial call to a Python C API function has a reasonable chance to fail at runtime which leads to code that has to error check after any call of a Python C API function.

The aim of this project is to offer a light weight c++ wrapper around the Python C API that - by using RAII, exceptions and type checking - makes using it easy and safe.

Consider the following (to be fair pathological) example. Let us define a Printer class in in a file my_print.py that might process the input by setting the _args field.

```python
class Printer:
    def __init__(self):
        self._args = None

    def Print(self, arg):
        print(arg)
```

Now let us import that class, instantiate an instance of it and call its Print Method with some parameter from c++. For demonstrations sake we will be using a vector which will be turned to a Python list and then be printed. An implementation using the raw Python C API with pretty much full error checking and decreffing might look something like this - to be fair this could be written a lot better and I was honestly suprised at how long this got...

```c++
void LogPythonError();

void ConvertAndPrint(const std::vector<int>& toPrint)
{
    Py_Initialize();

    // add the module's directory to system path so it can be imported
    constexpr auto path = "path/to/script";

    const auto py_path_list = PySys_GetObject("path");
    if (!py_path_list)
    {
        LogPythonError();
        Py_Finalize();
        return;
    }

    const auto py_path_string = PyUnicode_FromString(path);
    if (!py_path_string)
    {
        LogPythonError();
        Py_Finalize();
    }

    if (PyList_Append(py_path_list, py_path_string) != 0)
    {
        Py_DECREF(py_path_string);
        LogPythonError();
        Py_Finalize();
        return;
    }
    Py_DECREF(py_path_string);

    // import the print module
    const auto pyPrinterModule = PyImport_ImportModule("my_print");
    if (!pyPrinterModule)
    {
        LogPythonError();
        Py_Finalize();
        return;
    }

    // get a handle to the Printer class
    const auto pyPrinterClass = PyObject_GetAttrString(pyPrinterModule, "Printer");
    Py_DECREF(pyPrinterModule);
    if (!pyPrinterClass)
    {
        LogPythonError();
        Py_Finalize();
        return;
    }

    // create an instance of the Printer class
    auto pyBuildValue = Py_BuildValue("()");
    if (!pyBuildValue)
    {
        Py_DECREF(pyPrinterClass);
        LogPythonError();
        Py_Finalize();
        return;
    }

    const auto pyPrinterInstance = PyEval_CallObject(pyPrinterClass, pyBuildValue);
    if (!pyPrinterInstance)
    {
        Py_DECREF(pyBuildValue);
        Py_DECREF(pyPrinterClass);
        LogPythonError();
        Py_Finalize();
        return;
    }
    Py_DECREF(pyBuildValue);
    pyBuildValue = nullptr;

    // get a handle to the Print method of the Printer instance
    const auto pyPrintMethod = PyObject_GetAttrString(pyPrinterInstance, "Print");
    if (!pyPrintMethod)
    {
        Py_DECREF(pyPrinterInstance);
        LogPythonError();
        Py_Finalize();
        return;
    }

    // Create a Python list that holds the values of toPrint
    const auto pyList = PyList_New(toPrint.size());
    if (!pyList)
    {
        Py_DECREF(pyPrinterClass);
        Py_DECREF(pyPrintMethod);
        Py_DECREF(pyPrinterInstance);
        LogPythonError();
        Py_Finalize();
        return;
    }
    size_t idx = 0;
    for (const auto elem : toPrint)
    {
        if (PyList_SetItem(pyList, idx, PyLong_FromLong(elem)) == -1)
        {
            Py_DECREF(pyPrinterClass);
            Py_DECREF(pyPrintMethod);
            Py_DECREF(pyPrinterInstance);
            Py_DECREF(pyList);
            LogPythonError();
            Py_Finalize();
            return;
        }
        ++idx;
    }

    // Call the Print method with the created list as arg
    pyBuildValue = Py_BuildValue("(O)", pyList);
    if (!pyBuildValue)
    {
        Py_DECREF(pyPrinterClass);
        Py_DECREF(pyPrintMethod);
        Py_DECREF(pyPrinterInstance);
        Py_DECREF(pyList);
        LogPythonError();
        Py_Finalize();
        return;
    }

    const auto pyRetVal = PyEval_CallObject(pyPrintMethod, pyBuildValue);
    Py_DECREF(pyBuildValue);
    Py_DECREF(pyPrinterClass);
    Py_DECREF(pyPrintMethod);
    Py_DECREF(pyPrinterInstance);
    Py_DECREF(pyList);
    if (!pyRetVal)
    {        
        LogPythonError();
        Py_Finalize();
        return;
    }

    Py_DECREF(pyRetVal);
    Py_Finalize();
}

void LogPythonError()
{
    if (PyErr_Occurred())
    {
        std::cout << "PythonError: ";

        PyObject* pyExcType;
        PyObject* pyExcValue;
        PyObject* pyExcTraceback;

        PyErr_Fetch(&pyExcType, &pyExcValue, &pyExcTraceback);
        PyErr_NormalizeException(&pyExcType, &pyExcValue, &pyExcTraceback);

        if (pyExcType)
        {
            // We are not interested in the type for now
            Py_DECREF(pyExcType);
        }
        if (pyExcValue)
        {
            const auto pyStrRepr = PyObject_Repr(pyExcValue);
            const auto pyStr = PyUnicode_AsEncodedString(pyStrRepr, "utf-8", "~E~");
            std::cout << PyBytes_AsString(pyStr);
            Py_DECREF(pyExcValue);
            Py_DECREF(pyStrRepr);
            Py_DECREF(pyStr);
        }
        if (pyExcTraceback)
        {
            const auto pyStrRepr = PyObject_Repr(pyExcTraceback);
            const auto pyStr = PyUnicode_AsEncodedString(pyStrRepr, "utf-8", "~E~");
            std::cout << PyBytes_AsString(pyStr);
            Py_DECREF(pyExcTraceback);
            Py_DECREF(pyStrRepr);
            Py_DECREF(pyStr);
        }
    }
    else
        std::cout << "No Python Error!";

    std::cout << std::endl;
}
```
Using this wrapper all of this could be condensed to this:

```c++
void ConvertAndPrint(const std::vector<int>& toPrint)
{
    auto handle = pycpp::PythonInterpreter::Handle();

    try
    {
        pycpp::PythonList myList(toPrint);

        pycpp::PythonSys::AddToSystemPath("path/to/script");

        const auto printerModule = pycpp::ImportModule("my_print");

        pycpp::PythonCallable printerClass = printerModule.GetAttribute("Printer");

        const auto printerInstance = printerClass();

        pycpp::PythonCallable printMethod = printerInstance.GetAttribute("Print");

        printMethod(myList);
    }
    catch (pycpp::PythonError& ex)
    {
        std::cout << "PythonError caught: " << ex.what() << std::endl;
    }
}
```
In fact this could even be templated and work with any STL container as long as
```c++
pycpp::isPythonBaseType<Container::value_type>::value
```
evaluates to true:
```c++
template<typename Container>
void ConvertAndPrint(const Container& toPrint)
{
    pycpp::PythonInterpreter::Open();

    try
    {
        pycpp::PythonList myList(toPrint);

        pycpp::PythonSys::AddToSystemPath("path/to/script");

        const auto printerModule = pycpp::ImportModule("my_print");

        pycpp::PythonCallable printerClass = printerModule.GetAttribute("Printer");

        const auto printerInstance = printerClass();

        pycpp::PythonCallable printMethod = printerInstance.GetAttribute("Print");

        printMethod(myList);
    }
    catch (pycpp::PythonError& ex)
    {
        std::cout << "PythonError caught: " << ex.what() << std::endl;
    }
}
```
