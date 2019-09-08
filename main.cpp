#include "YRLogger.h"
#include <iostream>
#include <string>

using namespace std;
using namespace youren;

int main()
{
    YRRollLogger::getInstance()->setLogInfo("Hello", "HelloServer", ".", 52428800, 10);

	YRRollLogger::getInstance()->logger()->setLogLevel("DEBUG");

	YRRollLogger::getInstance()->logger()->debug() << "[YOUREN]" << "Hello" << endl;
	
    return 0;
}
