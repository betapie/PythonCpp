#include <iostream>
#include <PythonCpp.h>

int main()
{
	auto handle = pycpp::Interpreter::Handle();

	try
	{
		auto plt = pycpp::ImportModule("matplotlib.pyplot");

		pycpp::Callable plotFn = plt.GetAttribute("plot");

		pycpp::Callable showFn = plt.GetAttribute("show");

		pycpp::List pyList( {1, 2, 7, 6} );

		plotFn(pyList);
		showFn();
	}
	catch( const pycpp::Error& ex )
	{
		std::cout << "Error occured: " << ex.what() << std::endl;
	}
	return 0;
}
