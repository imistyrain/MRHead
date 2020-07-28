#pragma once
#include "sstream"
#include "vector"
#include "algorithm"
#include "string"
#include <memory>

template <class T>
static std::string tostring(const T &t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template <class T>
static T fromstring(const std::string &s)
{
    std::stringstream ss(s);
	T t;
	ss >> t;
	return t;
}

static std::string int2string(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

static int string2int(std::string s)
{
	std::stringstream ss(s);
	int n;
	ss >> n;
	return n;
}

static std::string double2string(double d)
{
	std::stringstream ss;
	ss << d;
	return ss.str();
}

static double string2double(std::string s)
{
	std::stringstream ss(s);
	double d;
	ss >> d;
	return d;
}

static std::vector<std::string> split(const std::string& s, const std::string delim)
{
	std::vector< std::string > ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
	return ret;
}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

#ifdef _WIN32
	#include <windows.h>
	void static MRsleep(unsigned milliseconds)
	{
		Sleep(milliseconds);
	}
#else
	#include <unistd.h>
	void static MRsleep(unsigned milliseconds)
	{
		usleep(milliseconds * 1000); // takes microseconds
	}
#endif
