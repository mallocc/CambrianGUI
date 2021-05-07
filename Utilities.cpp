#include "Utilities.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include "Wtsapi32.h"
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>

void createConsole()
{
	AllocConsole();

	FILE* dummyFile1;
	freopen_s(&dummyFile1, "nul", "r", stdin);

	FILE* dummyFile2;
	freopen_s(&dummyFile2, "nul", "w", stdout);

	FILE* dummyFile3;
	freopen_s(&dummyFile3, "nul", "w", stderr);

	// Redirect unbuffered stdin from the current standard input handle

	HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "r");
			if (file != NULL)
			{
				int dup2Result = _dup2(_fileno(file), _fileno(stdin));
				if (dup2Result == 0)
				{
					setvbuf(stdin, NULL, _IONBF, 0);
				}
			}
		}
	}

	// Redirect unbuffered stdout to the current standard output handle

	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if (file != NULL)
			{
				int dup2Result = _dup2(_fileno(file), _fileno(stdout));
				if (dup2Result == 0)
				{
					setvbuf(stdout, NULL, _IONBF, 0);
				}
			}
		}
	}

	// Redirect unbuffered stderr to the current standard error handle

	stdHandle = GetStdHandle(STD_ERROR_HANDLE);
	if (stdHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		if (fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if (file != NULL)
			{
				int dup2Result = _dup2(_fileno(file), _fileno(stderr));
				if (dup2Result == 0)
				{
					setvbuf(stderr, NULL, _IONBF, 0);
				}
			}
		}
	}

	// Clear the error state for each of the C++ standard stream objects. We need to do this, as attempts to access the
	// standard streams before they refer to a valid target will cause the iostream objects to enter an error state. In
	// versions of Visual Studio after 2005, this seems to always occur during startup regardless of whether anything
	// has been read from or written to the targets or not.

	std::wcin.clear();
	std::cin.clear();

	std::wcout.clear();
	std::cout.clear();

	std::wcerr.clear();
	std::cerr.clear();
}

void outputToDebugFile(std::string in)
{
	std::filesystem::path p = __FILE__;
	p = p.parent_path() /= "DebugOut.log";
	std::ofstream logFile(p.c_str(), std::ios::app);
	logFile << in << std::endl;
	logFile.close();
}

HMODULE GetCurrentModule()
{ // NB: XP+ solution!
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}
std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(GetCurrentModule(), buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
std::string DLLPath() {
	char buffer[MAX_PATH];
	GetModuleFileName(GetCurrentModule(), buffer, MAX_PATH);
	return std::string(buffer);
}

bool checkJSON(nlohmann::json j, std::string fieldName)
{
	//std::cout << "Looking for " << fieldName << std::endl;
	//std::cout << j.dump(4) << std::endl; 
	return !j.empty() && j.contains(fieldName);
}

bool readJSONAsString(nlohmann::json j, std::string fieldName, std::string& field, bool debugPrint)
{
	if (debugPrint)
	{
		std::cout << "Attempting field name " << fieldName << std::endl;
		std::cout << "JSON " << j.dump(4) << std::endl;
	}
	if (checkJSON(j, fieldName))
	{
		if (j.at(fieldName).is_string())
			field = j.at(fieldName).get<std::string>();
		else
			field = j.at(fieldName).dump();

		if (debugPrint)
			std::cout << "JSON value found: '" << field << "'" << std::endl;
		return true;
	}
	else
	{
		if (debugPrint)
			std::cout << "JSON doesnt contain field name " << fieldName << std::endl;
	}
	return false;
}

void printGlob(nlohmann::json& j1)
{
	for (const auto& j : nlohmann::json::iterator_wrapper(j1))
	{
		std::cout << "Glob contains property " << j.key() << ": " << j.value() << std::endl;
	}
}

void globProperties(nlohmann::json& j1, const nlohmann::json& j2)
{
	for (const auto& j : nlohmann::json::iterator_wrapper(j2))
	{
		if (j.key() != "class")
			j1[j.key()] = j.value();			
	}
}
void mergeJsonObjects(nlohmann::json& destination, const nlohmann::json& source)
{
	for (const auto& i : nlohmann::json::iterator_wrapper(source))
	{
		if (i.value().is_array())
		{
			if (destination.contains(i.key()))
			{
				for (const auto& o : i.value())
					destination.at(i.key()).push_back(o);
			}
			else
			{
				destination[i.key()] = i.value();
			}
		}
		else
		{
			destination[i.key()] = i.value();
		}
	}
}


std::string colorToString(float r, float g, float b)
{
	int R = (r * 255);
	int G = (g * 255);
	int B = (b * 255);
	std::stringstream ss, rs, gs, bs;
	rs << std::setfill('0') << std::setw(2) << std::right << std::hex << R;
	gs << std::setfill('0') << std::setw(2) << std::right << std::hex << G;
	bs << std::setfill('0') << std::setw(2) << std::right << std::hex << B;
	ss << "#" << rs.str() << gs.str() << bs.str();
	return ss.str();
}

void replaceVariables(nlohmann::json& j, std::string key, std::string val)
{
	for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {

		if (it.value().is_structured())
		{
			replaceVariables(it.value(), key, val);
		}
		else if (it.value() == key)
		{
			*it = val;
		}
	}
}

void gatherInstances(std::string fieldName, const nlohmann::json& j, std::vector<nlohmann::json>& instances)
{
	for (const auto& i : j.items())
	{
		if (i.value().is_structured())
		{
			gatherInstances(fieldName, i.value(), instances);
		}
		else
		{
			//std::cout << i.key() << " : " << i.value() << std::endl;
			if (i.key() == fieldName)
			{
				instances.push_back({ { i.key(), i.value() } });
			}
		}
	}
}

std::vector<std::string> splitWithCharacters(const std::string& str, int splitLength)
{
	int NumSubstrings = str.length() / splitLength;
	std::vector<std::string> ret;

	for (int i = 0; i < NumSubstrings; i++)
	{
		ret.push_back(str.substr(i * splitLength, splitLength));
	}

	if (str.length() % splitLength != 0)
	{
		ret.push_back(str.substr(splitLength * NumSubstrings));
	}

	return ret;
}
