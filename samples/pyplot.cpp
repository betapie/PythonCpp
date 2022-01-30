#include <iostream>
#include <PythonCpp.h>

int main()
{
	auto handle = pycpp::PythonInterpreter::Handle();

	try
	{
		auto plt = pycpp::ImportModule("matplotlib.pyplot");

		pycpp::PythonCallable plotFn = plt.GetAttribute("plot");

		pycpp::PythonCallable showFn = plt.GetAttribute("show");

		pycpp::PythonList pyList( {1, 2, 7, 6} );

		plotFn(pyList);
		showFn();
	}
	catch( const pycpp::PythonError& ex )
	{
		std::cout << "PythonError occured: " << ex.what() << std::endl;
	}
	return 0;
}
