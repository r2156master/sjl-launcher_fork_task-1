/*
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//ManualSynchronization on 24.12.23 06:36
//#include <string>
//#include <windows.h>
#include <windows.h>
#include <string>
//ManualSynchronization ends

#include <vector>
#include "resource.h"
#include "jni.h"
#include "jni_md.h"
#include <time.h>

using namespace std;
// some constants
#define UPDATE_STATE_READING_TYPE 1
#define UPDATE_STATE_READING_MOVE_SOURCE 2
#define UPDATE_STATE_READING_MOVE_TARGET 3
#define UPDATE_STATE_READING_DELETE_FILE 4
#define UPDATE_STATE_READING_SLEEP_TIMEOUT 5
#define UPDATE_STATE_READING_SPLASH_FILE 6
#define SPLASH_WINDOW_CLASS_NAME L"SplashWindow"

// global variables
DWORD dwExitCode = 0;
bool debug_flag = false;
HINSTANCE hInst;
wstring base_path;
wstring executable_path;
wstring program_params;
wstring jvm_path;
wstring exception_message;
wstring sjl_path;
HANDLE mutex = NULL;
HANDLE log_file;
HANDLE splash_screen_thread;
HBITMAP h_splash_bitmap, h_splash_old_bitmap;
HDC h_dc, h_dc_mem;
BITMAP splash_bm;
PAINTSTRUCT splash_ps;
RECT splash_rect;
HWND splash_window_handle;
DWORD process_id;

// functions declaration
void show_last_error();
//ManualSynchronization on 24.12.23 20:24
void debug_args(wchar_t* format, ...);
//ManualSynchronization on 24.12.23 18:40
void debug_args(char* format, ...);
//ManualSynchronization on 24.12.23 07:48
void debug_(const char* format, ...);
//ManualSynchronization ends
void debug(wstring message);
void debug(string message);
void close_log_file_handle();
void trim_line(char *line);
void trim_line(wchar_t *line);
void add_option(string token, vector<string> &vmOptionLines);
//ManualSynchronization on 24.12.23 06:21
//string to_string(const std::wstring &str);
void init_paths();
wstring get_wstring_param(int id);
void init_debug_file();
bool is_file_exist(wstring path);
bool is_directory(wstring path);
void throw_exception(wstring message);
HANDLE init_utf8_file(wstring file_name);
void init_sjl_dir();
void create_mutex();
string get_string_param(int id);
//ManualSynchronization on 23.12.23 14:46
//wstring to_wstring(string s);
void copy_directory(wstring source, wstring target);
void remove_directory(wstring dir);
void show_splash(wstring imagePath);
void hide_current_splash();
LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void activate_window();
// jni declarations
typedef JNIIMPORT jint(JNICALL *JNI_createJavaVM)(JavaVM **pvm, JNIEnv **env, void *args);

void(JNICALL jniExitHook)(jint code)
{
	debug("exited");
	dwExitCode = code;
	string restartCode = get_string_param(IDS_RESTART_EXIT_CODE);
	if (mutex)
	{
		ReleaseMutex(mutex);
	}
	close_log_file_handle();
	if (code == stoi(restartCode))
	{
		wstring newParams = program_params;
		if (newParams.find(L"-sjl-restart") == string::npos)
		{
			newParams = newParams + L" -sjl-restart";
		}

		ShellExecuteW(NULL, L"open", executable_path.c_str(), newParams.c_str(), NULL, SW_RESTORE);		
	}
}

// main function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR pCmdLine, int nCmdShow)
{
	process_id = GetCurrentProcessId();
	hInst = hInstance;
	string cmdLineStr = pCmdLine;
	//ManualSynchronization on 24.12.23 07:37
	//program_params = to_wstring(cmdLineStr);
	//ManualSynchronization ends
	debug_flag = cmdLineStr.find("-sjl-debug") != string::npos || true;
	try
	{
		if (cmdLineStr.find("-sjl-restart") != string::npos)
		{
			Sleep(200);
		}
		init_paths();
		init_debug_file();
		//ManualSynchronization on 24.12.23 18:39
		debug_args("new outputDebug with args firstArg:%s SecondArg2:%d ThirdArg3:%s", cmdLineStr.c_str(), 2, "third arg");
		//ManualSynchronization on 24.12.23 07:55
		//ManualSynchronization on 24.12.23 20:30
		debug_args(L"Unicode new outputDebug with args firstArg:%S SecondArg2:%d ThirdArg3:%s", cmdLineStr.c_str(), 2, L"third arg");
		//debug_("new outputDebug arg1:%s arg2:%d arg3:%s",cmdLineStr.c_str(),2,"third arg");
		debug("command line is :" + cmdLineStr);
		debug("nCmdShow is :" + to_string(nCmdShow));
		create_mutex();
		_wchdir(base_path.c_str());
		debug(L"working dir:\t" + base_path);
		if (true)
		{
			show_splash(L"c:\\IdeaProjects\\sjl\\launchers\\windows\\build\\sample.bmp");
			Sleep(2000);
			hide_current_splash();			
		}
		wstring update_dir = sjl_path + L"\\update";
		wstring update_script_file_name = update_dir + L"\\update.script";
		if (is_file_exist(update_script_file_name))
		{
			debug("found update script");
			FILE *f;
			vector<wstring> updateLines;
			//ManualSynchronization on 24.12.23 07:36
			//int fo_res = fopen_s(&f, to_string(update_script_file_name).c_str(), "rt");
			//if (!fo_res)
			//{

			//	wchar_t buffer[4096];
			//	while (fgetws(buffer, sizeof(buffer), f))
			//	{
			//		trim_line(buffer);
			//		wstring line = buffer;
			//		updateLines.push_back(line);
			//	}
			//	fclose(f);
			//}
			//else
			//{
			//	debug("unable to open update file, code = " + fo_res);
			//	throw_exception(L"unable to open update file");
			//}
			//ManualSynchronization ends
			int mode = UPDATE_STATE_READING_TYPE;
			wstring sourceFile;
			for (int n = 0; n < updateLines.size(); n++)
			{
				wstring line = updateLines[n];
				if (mode == UPDATE_STATE_READING_TYPE)
				{
					if (line.compare(L"file-move:") == 0)
					{
						debug("current command is file-move");
						mode = UPDATE_STATE_READING_MOVE_SOURCE;
						continue;
					}
					if (line.compare(L"file-delete:") == 0)
					{
						debug("current command is file-move");
						mode = UPDATE_STATE_READING_DELETE_FILE;
						continue;
					}
					if (line.compare(L"sleep:") == 0)
					{
						debug("current command is sleep");
						mode = UPDATE_STATE_READING_SLEEP_TIMEOUT;
						continue;
					}
					if (line.compare(L"show-splash:") == 0)
					{
						debug("current command is show splash");
						mode = UPDATE_STATE_READING_SPLASH_FILE;
						continue;
					}
					if (line.compare(L"hide-splash:") == 0)
					{
						debug("current command is hide splash");
						hide_current_splash();
						mode = UPDATE_STATE_READING_TYPE;
						continue;
					}
					debug(L"undefined update mode " + line);
					throw_exception(L"undefined mode " + line);
				}
				if (mode == UPDATE_STATE_READING_DELETE_FILE)
				{
					debug("mode is file-delete");
					if (is_file_exist(line))
					{
						debug(L"file exists :" + line + L" trying to delete it");
						if (is_directory(line))
						{
							debug("deleting directory");
							remove_directory(line);
						}
						else if (!DeleteFileW(line.c_str()))
						{
							debug("unable to delete file");
							throw_exception(L"unable to delete file: " + line);
						}
					}
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_SPLASH_FILE)
				{
					debug(L"splash file is set to " + line);
					show_splash(line);
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_MOVE_SOURCE)
				{
					sourceFile = line;
					debug(L"source file is set to " + line);
					debug("mode changed to UPDATE_STATE_READING_MOVE_TARGET");
					mode = UPDATE_STATE_READING_MOVE_TARGET;
					continue;
				}
				if (mode == UPDATE_STATE_READING_MOVE_TARGET)
				{
					wstring target_file = line;
					debug(L"copying " + sourceFile + L" to " + target_file);
					if (is_file_exist(target_file))
					{
						debug(L"target file exists, trying to delete it");
						if (is_directory(target_file))
						{
							debug("deleting direcotry");
							remove_directory(target_file);
						}
						else if (!DeleteFileW(target_file.c_str()))
						{
							debug(L"unable to delete " + target_file);
							throw_exception(L"unable to delete " + target_file);
						}
					}
					if (is_directory(sourceFile))
					{
						debug("copy directory");
						copy_directory(sourceFile, target_file);
					}
					else if (!CopyFileW(sourceFile.c_str(), target_file.c_str(), false))
					{
						debug(L"unable to copy " + sourceFile + L" to " + target_file);
						throw_exception(L"unable to copy " + sourceFile + L" to " + target_file);
					}
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
				if (mode == UPDATE_STATE_READING_SLEEP_TIMEOUT)
				{
					int sleep = stoi(line);
					debug(L"sleep timeout: " + line);
					Sleep(sleep);
					debug("mode changed to UPDATE_STATE_READING_TYPE");
					mode = UPDATE_STATE_READING_TYPE;
					continue;
				}
			}
			debug(L"removing update directory " + update_dir);
			remove_directory(update_dir);
		}

		vector<string> vmOptionLines;
		string path = get_string_param(IDS_VM_OPTIONS_FILE);
		if (!path.empty())
		{
			debug("reading options file " + path);
			//ManualSynchronization on 24.12.23 07:35
			//FILE *f;
			//if (is_file_exist(to_wstring(path)))
			//{
			//	debug("file exists");
			//	int res = fopen_s(&f, path.c_str(), "rt");
			//	if (!res)
			//	{

			//		char buffer[4096];
			//		while (fgets(buffer, sizeof(buffer), f))
			//		{
			//			trim_line(buffer);
			//			if (strlen(buffer) > 0 && buffer[0] != '#' && strcmp(buffer, "-server") != 0)
			//			{
			//				string line(buffer);
			//				debug("added options line " + line);
			//				vmOptionLines.push_back(line);
			//			}
			//		}
			//		fclose(f);
			//	}
			//	else
			//	{
			//		//ManualSynchronization on 23.12.23 14:34 - error 4 ( to_wstring not working with integer )
			//		//ManualSynchronization on 24.12.23 07:35
			//		//debug(L"unable to open file, code " + std::to_wstring(res));
			//		//throw_exception(L"unable to open options file " + to_wstring(path));
			//		//ManualSynchronization
			//	}
			//}
			//ManualSynchronization ends
		}

		string predefinedOptions = get_string_param(IDS_VM_OPTIONS);
		if (!predefinedOptions.empty())
		{
			debug("predefined options exist");
			string delimiter = "|";
			size_t pos = 0;
			std::string token;
			while ((pos = predefinedOptions.find(delimiter)) != std::string::npos)
			{
				token = predefinedOptions.substr(0, pos);
				add_option(token, vmOptionLines);

				predefinedOptions.erase(0, pos + delimiter.length());
			}
			add_option(predefinedOptions, vmOptionLines);
		}

		wstring binDir = jvm_path + L"\\bin";
		wstring dllName = binDir + L"\\server\\jvm.dll";
		if (!is_file_exist(dllName))
		{
			debug(L"file does not exist " + dllName);
			dllName = binDir + L"\\client\\jvm.dll";
			if (!is_file_exist(dllName))
			{
				debug(L"file does not exist " + dllName);
				debug(L"unable to locate jvm.dll in " + binDir);
				throw_exception(L"unable to locate jvm.dll");
			}
		}

		SetEnvironmentVariableW(L"JAVA_HOME", binDir.c_str());
		debug(L"JAVA_HOME is set to " + binDir);
		//ManualSynchronization on 23.12.23 14:34 - error 6
		//SetDllDirectoryW(nullptr);
		SetCurrentDirectoryW(base_path.c_str());
		debug(L"current directory is set to " + base_path);
		HMODULE hJVM = LoadLibraryW(dllName.c_str());
		if (!hJVM)
		{
			debug(L"unable to load jvm from " + dllName);
			throw_exception(L"unable to load jvm from " + dllName);
		}
		JNI_createJavaVM pCreateJavaVM = NULL;
		if (hJVM)
		{
			pCreateJavaVM = (JNI_createJavaVM)GetProcAddress(hJVM, "JNI_CreateJavaVM");
		}
		if (!pCreateJavaVM)
		{
			debug("unable to find address of JNI_CreateJavaVM");
			throw_exception(L"unable to find address of JNI_CreateJavaVM");
		}
		int vmOptionsCount = 2 + vmOptionLines.size();
		JavaVMOption *vmOptions = (JavaVMOption *)calloc(vmOptionsCount, sizeof(JavaVMOption));
		vmOptions[0].optionString = (char *)"exit";
		vmOptions[0].extraInfo = (void *)jniExitHook;
		string cp = get_string_param(IDS_CLASS_PATH);
		if (cp.empty())
		{
			debug("classpath is not defined");
			throw_exception(L"classpath is not defined");
		}
		string cpOption = "-Djava.class.path=" + cp;
		debug("class path is set to " + cp);
		vmOptions[1].optionString = (char *)cpOption.c_str();
		for (int i = 0; i < vmOptionLines.size(); i++)
		{
			string opt = vmOptionLines[i];

			vmOptions[i + 2].optionString = _strdup(opt.c_str());
			vmOptions[i + 2].extraInfo = NULL;
		}

		JavaVMInitArgs vmArgs;
		vmArgs.version = JNI_VERSION_1_2;
		vmArgs.nOptions = vmOptionsCount;
		vmArgs.options = vmOptions;
		vmArgs.ignoreUnrecognized = JNI_TRUE;

		JNIEnv *jenv = NULL;
		JavaVM *jvm = NULL;
		int result = pCreateJavaVM(&jvm, &jenv, &vmArgs);
		for (int i = 1; i < vmOptionsCount; i++)
		{
			free(vmOptions[i].optionString);
		}
		free(vmOptions);
		vmOptions = NULL;
		if (result != JNI_OK)
		{
			debug("unablet to create jvm");
			throw_exception(L"unable to create jvm");
		}
		debug("jvm was created");
		jthrowable jtExcptn;
		const std::string mainClassName = get_string_param(IDS_MAIN_CLASS);
		debug("main class is " + mainClassName);
		jclass mainClass = jenv->FindClass(mainClassName.c_str());
		if (!mainClass)
		{
			debug("unable to fine main class " + mainClassName);
			throw_exception(L"unable to find main class ");
		}

		jmethodID mainMethod = jenv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
		if (!mainMethod)
		{
			debug("unable to fine main method");
			throw_exception(L"unable to find main method ");
		}
		jclass stringClass = jenv->FindClass("java/lang/String");
		jobjectArray args = jenv->NewObjectArray(0, stringClass, NULL);

		jenv->CallStaticVoidMethod(mainClass, mainMethod, args);
		jthrowable exc = jenv->ExceptionOccurred();
		if (exc)
		{
			debug("error occurred invoking main method");
			throw_exception(L"Error invoking main method");
		}
		activate_window();
	    jvm->DestroyJavaVM();
		debug("jvm was destroyed");
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}
	catch (...)
	{
		debug(L"error occurred");
		close_log_file_handle();
		show_last_error();
		return 1;
	}
}

void show_last_error()
{
	MessageBoxW(NULL, exception_message.empty() ? L"Unknown error" : exception_message.c_str(),
				L"Error occurred", MB_OK);
}
//ManualSynchronization on 24.12.23 20:25
void debug_args(wchar_t* format, ...)
{
	if (debug_flag)
	{
		
		va_list args;
		va_start(args, format);

		wchar_t buf[1024] = { 0, };
		_vsnwprintf(buf, sizeof(buf), format, args);
		va_end(args);

		char temp_file_name[256] = "debug_output_u.txt";
		wchar_t temp_time_data[256] = { 0 };
		wchar_t	new_line_str[256] = L"\r\n";
		struct tm* tm_ptr = NULL;
		time_t raw;

		time(&raw);
		tm_ptr = localtime(&raw);

		wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", tm_ptr);

		FILE* pFile = fopen(temp_file_name, "a");
		fputws(new_line_str, pFile);
		fputws(temp_time_data, pFile);
		fputws(buf, pFile);
		fflush(pFile);

		va_end(args);
	}
}
void debug_args(char* format, ...)
{
	if (debug_flag)
	{
		WCHAR buf[1024] = { 0 };
		va_list args;
		va_start(args, format);

		char buf2[2048] = { 0, };
		_vsnprintf(buf2, sizeof(buf2), format, args);
		va_end(args);

		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);

		char temp_file_name[256] = "debug_output.txt";
		wchar_t temp_time_data[256] = { 0 };
		wchar_t	new_line_str[256] = L"\r\n";
		struct tm* tm_ptr = NULL;
		time_t raw;

		time(&raw);
		tm_ptr = localtime(&raw);

		wcsftime(temp_time_data, 256, L"'%Y-%m-%d %H:%M:%S' ", tm_ptr);

		FILE* pFile = fopen(temp_file_name, "a");
		fputws(new_line_str, pFile);
		fputws(temp_time_data, pFile);
		fputws(buf, pFile);
		fflush(pFile);

		va_end(args);
	}
}
void debug_(const char* format, ...)
{
	if (debug_flag)
	{
		WCHAR buf[1024] = { 0 };
		char  buf2[2048] = { 0 };

		va_list args;
		va_start(args, format);

		_vsnprintf(buf2, sizeof(buf2), format, args);
		va_end(args);
		//ManualSynchronization on 24.12.23 15:33
		int len = MultiByteToWideChar(CP_UTF8, 0, buf2, -1, 0, 0);
		wchar_t* wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
		MultiByteToWideChar(CP_UTF8, 0, buf2, -1, wstr, len + 2);
		std::wstring nresultwstr = wstr;
		free(wstr);
		//MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);
		//ManualSynchronization on 24.12.23 15:38
		//ManualSynchronization starts on 24.12.23 15:43
		//int len = MultiByteToWideChar(CP_ACP, 0, buf2, -1, 0, 0);
		//char* str = (char*)calloc(sizeof(char*), len + 2);
		//MultiByteToWideChar(CP_ACP, 0, buf2, -1, str, len + 2);
		//ManualSynchronization ends
		//ManualSynchronization starts on 24.15.23 15:08
		////manualSynchronization on 24.12.23 15:05
		//time_t* tv_sec=NULL;
		////ManualSynchronization on 24.12.23 14:59
		//struct tm systime = { 0 };
		//gmtime_s(systime, tv_sec);
		SYSTEMTIME systime2;
		GetSystemTime(&systime2);
		//ManualSynchronization starts on 24.12.23 15:12
		char buftime[256];
		sprintf(buftime, "[%04d-%02d-%02d %02d:%02d]", systime2.wYear, systime2.wMonth, systime2.wDay, systime2.wHour,
			systime2.wMinute);
		//ManualSynchronization starts on 24.12.23 18:08

		len = MultiByteToWideChar(CP_UTF8, 0, buftime, -1, 0, 0);
		wstr = (wchar_t*)calloc(sizeof(wchar_t), len + 2);
		MultiByteToWideChar(CP_UTF8, 0, buftime, -1, wstr, len + 2);
		std::wstring nresulttimewstr = wstr; //L"[";
		//nresulttimewstr +=wstr;
		nresulttimewstr += L" ";
		free(wstr);

		//ManualSynchroniaztion starts on 24.12.23 15:46
		wchar_t wbuftime[256] = { 0 };
		wsprintf(wbuftime, L"%04d-%02d-%02d %02d:%02d.%07d", systime2.wYear, systime2.wMonth, systime2.wDay, systime2.wHour,
			systime2.wMinute, systime2.wSecond, systime2.wMilliseconds * 10000);

		//ManualSynchronization ends
		//ManualSynchronization starts on 24.12.23 15:10

		//char buftime[256];
		//sprintf(buftime, "%04d-%02d-%02dT%02d:%02d.%07dZ", 1900 + systime.tm_year, systime.tm_mon + 1, systime.tm_mday, systime.tm_hour,
		//	systime.tm_min, systime.tm_sec, (int)(tv_usec * 10));
		//ManualSynchronization ends

		//ManualSynchronization on 24.12.23 14:41
		//wprintf(L"%s", buf);
		//ManualSynchronization starts on 24.12.23 14:56
		//DWORD nmBytesWritten;
		DWORD nmBytesWritten = 0;
		LPOVERLAPPED lpOverlapped;
		DWORD Access = GENERIC_WRITE;
		DWORD WinFlags = FILE_SHARE_WRITE;
		DWORD CREATE = CREATE_ALWAYS;
		HANDLE hFile = CreateFileA("debug_output.txt", Access, WinFlags, NULL, CREATE,
			FILE_ATTRIBUTE_NORMAL, NULL);
		//ManualSynchronization on 24.12.23 17:44
		//
		////ManualSynchronization on 24.12.23 14:52
		////WriteFile(hFile, buf2, sizeof(buf2), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 15:15
		////strcat(buftime,buf2 );
		////ManualSynchronization on 24.12.23 15:20
		//std::string outputstr = "[";
		//outputstr += buftime;
		//outputstr += "] ";
		////ManualSynchronization on 24.12.23 15:37
		////ManualSynchronization on 24.12.23 15:45
		////outputstr += nresultwstr;
		////outputstr += buf2;

		////ManualSynchronization on 24.12.23 15:21
		////WriteFile(hFile, buf2, strlen(buf2), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 15:43
		////WriteFile(hFile, outputstr.data(), outputstr.size(), &nmBytesWritten, NULL);
		////ManualSynchronization starts on 24.12.23 15:48
		////ManualSynchronization starts on 24.12.23 16:47
		//std::wstring woutputstr = L"[";
		//woutputstr += wbuftime;
		//woutputstr += L"] ";
		//woutputstr += nresultwstr;
		////ManualSynchronization ends
		////ManualSynchronization on 24.12.23 16:35
		//std::string strbuftime(buftime,strlen(buftime));
		//std::wstring wstrbuftime(strbuftime.begin(), strbuftime.end());
		////ManualSynchronization on 24.12.23 16:44
		////std::wstring woutputstr;
		////woutputstr = L"["+ wstrbuftime+L"]" + nresultwstr;
		////WriteFile(hFile, woutputstr.c_str(), woutputstr.size(), &nmBytesWritten, NULL);
		////ManualSynchronization on 24.12.23 16:36
		////WriteFile(hFile, wstrbuftime.c_str(), wstrbuftime.size(), &nmBytesWritten, NULL);
		////WriteFile(hFile, nresultwstr.data(), nresultwstr.size(), &nmBytesWritten, NULL);
		//WriteFile(hFile, woutputstr.c_str(), woutputstr.size(), &nmBytesWritten, NULL);
		//CloseHandle(hFile);
		//ManualSynchronization starts on 24.12.23 17:57
		//wchar_t* timewidestring = L"24.12.23 17:46 ";
		//WriteFile(hFile, timewidestring, wcslen(timewidestring)*2, &nmBytesWritten, NULL);
		//ManualSynchronization ends

		WriteFile(hFile, nresulttimewstr.c_str(), nresulttimewstr.size() * 2, &nmBytesWritten, NULL);
		WriteFile(hFile, nresultwstr.c_str(), nresultwstr.size(), &nmBytesWritten, NULL);
		CloseHandle(hFile);
		va_end(args);
	}
}

void trim_line(char *line)
{
	char *p = line + strlen(line) - 1;
	if (p >= line && *p == '\n')
	{
		*p-- = '\0';
	}
	while (p >= line && (*p == ' ' || *p == '\t'))
	{
		*p-- = '\0';
	}
}

void trim_line(wchar_t *line)
{
	wchar_t *p = line + wcslen(line) - 1;
	if (p >= line && *p == '\n')
	{
		*p-- = '\0';
	}
	while (p >= line && (*p == ' ' || *p == '\t'))
	{
		*p-- = '\0';
	}
}

void add_option(string token, vector<string> &vmOptionLines)
{
	debug("analyzing predefined option " + token);
	int markerPos = token.find("??");
	if (markerPos != string::npos)
	{
		debug("marker ?? exists");
		bool found = false;
		string start = token.substr(0, markerPos);
		for (int n = 0; n < vmOptionLines.size(); n++)
		{
			if (vmOptionLines[n].find(start) == 0)
			{
				debug("found existing option " + vmOptionLines[n]);
				found = true;
				break;
			}
		}
		if (!found)
		{
			string option = token.substr(0, markerPos) + token.substr(markerPos + 2);
			debug("adding option " + option);
			vmOptionLines.push_back(option);
		}
	}
	else
	{
		debug("option added " + token);
		vmOptionLines.push_back(token);
	}
}

void debug(wstring message)
{
	/*
	VA_ARGS -wikipedia , chatgtp
	ARG_LIST - wikipedia, chatgpt

	*/
	if (debug_flag)
	{
		wstring result = message + L"\n";
		DWORD dwBytesWritten = 0;
		//ManualSynchronization on 24.12.23 07:34
		//string strBuf = to_string(result);
		//WriteFile(log_file, strBuf.c_str(), strBuf.size(), &dwBytesWritten,
		//		  NULL);
		//ManualSynchronization ends
	}
}

void debug(string message)
{
	if (debug_flag)
	{
		string result = message + "\n";
		DWORD dwBytesWritten = 0;
		WriteFile(log_file, result.c_str(), result.size(), &dwBytesWritten,
				  NULL);
	}
}
//ManualSynchronization starts on 24.12.23 06:21
//string to_string(const wstring &str)
//{
//	const int cbANSI = WideCharToMultiByte(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0, NULL, NULL);
//	if (cbANSI <= 0)
//		return string();
//
//	char *ansiBuf = new char[cbANSI];
//	WideCharToMultiByte(CP_ACP, 0, str.c_str(), (int)str.size(), ansiBuf, cbANSI, NULL, NULL);
//	string result(ansiBuf, cbANSI);
//	delete[] ansiBuf;
//	return result;
//}
//ManualSynchronization ends
void init_paths()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	wstring pathStr = path;
	executable_path = path;
	size_t position = pathStr.find_last_of(L"\\");
	base_path = pathStr.substr(0, position);
	debug(L"base path is " + base_path);
	jvm_path = get_wstring_param(IDS_JVM_PATH);
	debug(L"jvm path is " + jvm_path);
}

wstring get_wstring_param(int id)
{
	wchar_t *buf = NULL;
	int len = LoadStringW(hInst, id, reinterpret_cast<LPWSTR>(&buf), 0);
	return len ? std::wstring(buf, len) : L"";
}

void init_debug_file()
{
	if (debug_flag)
	{
		init_sjl_dir();
		wstring log_file_name = sjl_path + L"\\sjl.log";
		log_file = init_utf8_file(log_file_name);
	}
}

bool is_directory(wstring path)
{
	DWORD attrib = GetFileAttributesW(path.c_str());

	return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0 && (GetLastError() != ERROR_FILE_NOT_FOUND);
}

bool is_file_exist(wstring path)
{
	LPCWSTR szPath = path.c_str();
	DWORD dwAttrib = GetFileAttributesW(szPath);

	return dwAttrib != INVALID_FILE_ATTRIBUTES && (GetLastError() != ERROR_FILE_NOT_FOUND);
}

HANDLE init_utf8_file(wstring file_name)
{
	bool exists = is_file_exist(file_name);

	HANDLE handle = exists ? CreateFileW(file_name.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
										 OPEN_EXISTING, 0, NULL)
						   : CreateFileW(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
										 CREATE_ALWAYS, 0, NULL);
	if (!exists)
	{
		unsigned char smarker[3];
		DWORD bytesWritten;

		smarker[0] = 0xEF;
		smarker[1] = 0xBB;
		smarker[2] = 0xBF;
		WriteFile(handle, smarker, 3, &bytesWritten, NULL);
	}
	else
	{
		SetFilePointer(handle, 0l, nullptr, FILE_END);
	}
	return handle;
}

void init_sjl_dir()
{
	sjl_path = base_path + L"\\" + get_wstring_param(IDS_SJL_DIR);
	if (!is_file_exist(sjl_path) && !CreateDirectoryW(sjl_path.c_str(), NULL))
	{
		throw_exception(L"unable to create directory " + sjl_path);
	}
}

void throw_exception(wstring message)
{
	exception_message = message;
	throw exception();
}

void create_mutex()
{
	wstring mutexName = get_wstring_param(IDS_MUTEX_NAME);

	if (!mutexName.empty())
	{
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.bInheritHandle = TRUE;
		security.lpSecurityDescriptor = NULL;
		mutex = CreateMutexW(&security, FALSE, mutexName.c_str());

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			debug("Instance already exists.");
			throw_exception(L"application instance is already launched");
		}
		debug(L"Create mutex:\t" + mutexName);
	}
}

string get_string_param(int id)
{
	wchar_t *buf = NULL;
	int len = LoadStringW(hInst, id, reinterpret_cast<LPWSTR>(&buf), 0);
	//ManualSynchronization on 24.12.23 07:31
	//return len ? to_string(wstring(buf, len)) : "";
	return "";
	//ManualSynchronization
}

void close_log_file_handle()
{
	if (log_file)
	{
		CloseHandle(log_file);
	}
}
//ManualSynchronization starts on 23.12.23 14:47 - error 4
//wstring to_wstring(string s)
//{
//	std::wstring ws(s.size(), L' '); // Overestimate number of code points.
//	ws.resize(std::mbstowcs(&ws[0], s.c_str(), s.size()));
//	return ws;
//}
//ManualSynchronization ends
void remove_directory(wstring dir)
{
	dir.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		L"",
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L""};
	int result = SHFileOperationW(&file_op);
	if (result)
	{
		debug(L"unable to delete directory " + dir + L" code=" + std::to_wstring(result));
		throw_exception(L"unable to delete directory " + dir);
	}
}

void copy_directory(wstring source, wstring target)
{
	source.push_back(L'\0');
	target.push_back(L'\0');
	SHFILEOPSTRUCTW file_op = {
		NULL,
		FO_COPY,
		source.c_str(),
		target.c_str(),
		FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
		false,
		0,
		L""};
	int result = SHFileOperationW(&file_op);
	if (result)
	{
		auto pstr_test = std::to_string(result);
		debug(L"unable to copy " + source + L" to " + target + L" code=" );
		throw_exception(L"unable to copy " + source + L" to " + target);
	}
}

BOOL CALLBACK enumwndfn(HWND hwnd, LPARAM lParam)
{
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (process_id == processId)
	{
		BringWindowToTop(hwnd);
		ShowWindow(hwnd, SW_NORMAL);
		return FALSE;
	}
	return TRUE;
}


DWORD WINAPI show_splash_function(LPVOID lpParam)
{
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = splash_window_procedure;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = SPLASH_WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	RegisterClassW(&wc);

	int x = (GetSystemMetrics(SM_CXSCREEN) - splash_bm.bmWidth) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - splash_bm.bmHeight) / 2;

	splash_window_handle = CreateWindowW(wc.lpszClassName, L"",
										 (WS_BORDER),
										 x, y, splash_bm.bmWidth, splash_bm.bmHeight, NULL, NULL, hInst, NULL);
	SetWindowLong(splash_window_handle, GWL_STYLE, 0); // remove all window styles, check MSDN for details

	ShowWindow(splash_window_handle, SW_SHOW);
	UpdateWindow(splash_window_handle);
	MSG msg;
	while (GetMessage(&msg, splash_window_handle, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_CLOSE)
		{
			break;
		}
	}
	debug("splash is hidden");
	return 0;
}
void show_splash(wstring image_path)
{
	h_splash_bitmap = (HBITMAP)LoadImageW(hInst, image_path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!h_splash_bitmap)
	{
		debug(L"unable to load bitmap image from " + image_path);
		throw_exception(L"unable to load bitmap image from " + image_path);
	}
	GetObject(h_splash_bitmap, sizeof(BITMAP), &splash_bm);
	DWORD threadId;
	splash_screen_thread = CreateThread(
		NULL,
		0,
		show_splash_function,
		NULL,
		0,
		&threadId);
	debug(L"splash thread is created");
}

void hide_current_splash()
{
	debug("hiding splash");
	PostMessage(splash_window_handle, WM_CLOSE, 0, 0);
	WaitForSingleObject(splash_screen_thread, INFINITE);
	UnregisterClassW(SPLASH_WINDOW_CLASS_NAME, hInst);
	debug("splash screen is hidden");
}

LRESULT CALLBACK splash_window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		h_dc = GetDC(hWnd);
		h_dc_mem = CreateCompatibleDC(h_dc);
		h_splash_old_bitmap = (HBITMAP)SelectObject(h_dc_mem, h_splash_bitmap);
		ReleaseDC(hWnd, h_dc);
		return 0;
	case WM_PAINT:
		h_dc = BeginPaint(hWnd, &splash_ps);

		GetClientRect(hWnd, &splash_rect);
		SetStretchBltMode(h_dc, STRETCH_HALFTONE);
		StretchBlt(h_dc, 0, 0, splash_rect.right, splash_rect.bottom,
				   h_dc_mem, 0, 0, splash_bm.bmWidth, splash_bm.bmHeight, SRCCOPY);

		EndPaint(hWnd, &splash_ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteDC(h_dc_mem);
		DeleteObject(h_splash_bitmap);
		DeleteObject(h_splash_old_bitmap);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI activate_window_function(LPVOID lpParam)
{
	Sleep(50);
	EnumWindows(enumwndfn, 1);
	return false;
}

void activate_window()
{
	DWORD threadId;
	CreateThread(
		NULL,
		0,
		activate_window_function,
		NULL,
		0,
		&threadId);
}