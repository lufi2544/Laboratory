
#include <string>
#include <iostream>

// define some inline assembly that causes a break
// into the debugger -- this will be different on each
// target CPU
#ifdef _WIN64
	#define debugBreak() __debugbreak()

#else
	#define debugBreak() _asm{ int 3 }

#endif // _WIN64


#if _DEBUG

#define SLOW_ASSERT(exp) \
	if(exp){} \
	else{\
	reportSLOWAssertionFailure(#exp , __FILE__,__LINE__);\
	debugBreak();\
	}

	// this is a function for the slow assertions. This is meant to be the assertion that explains the problem more in depth. ( creation of a window for example ).
inline void reportSLOWAssertionFailure(const char* _exp, const char* _file, int _line) { std::cout << "Expression " << _exp << " has a slow assertion in the file -> " << _file << " .Line ->  " << _line; };

#endif // _DEBUG


// check the expression and fail if it is false
#define ASSERT(expr) \
	\
	if(expr){ } \
	\
	else \
	{ \
	reportAssertionFailure(#expr, \
	\
	__FILE__, __LINE__); \
	\
	debugBreak();\
	}

inline void reportAssertionFailure(std::string _exp, std::string _file, int _line) { std::cout << "Expression " << _exp << " has an assertion in the file -> " << _file << " .Line ->  " << _line; };
