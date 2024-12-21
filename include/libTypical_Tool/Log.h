#pragma once

#include "pch.h"

#ifndef _LOG_H
#define _LOG_H

using namespace std;

#define _CRT_SECURE_NO_WARNINGS


//控制台字符颜色
#ifdef _ANSIESC_CONSOLE_CHAR_COLOR
#define ANSIESC_RESET "\033[0m"
#define ANSIESC_GREEN "\033[32m"
#define ANSIESC_YELLOW "\033[33m"
#define ANSIESC_RED "\033[31m"

#else
#define ANSIESC_RESET ""
#define ANSIESC_GREEN ""
#define ANSIESC_YELLOW ""
#define ANSIESC_RED ""

#endif

//#define _CONSOLE
#define _Thread
//#undef _WINDOWS


#ifndef _WINDOWS
	// 定义虚拟类型
	using DWORD = unsigned long;
	using WORD = unsigned short;
	using HANDLE = void*;
#endif

namespace Typical_Tool
{
	enum LogMessage
	{
		ts, //提示: Tips [INFO] | 绿色
		wr, //警告: Warning [WARRING] | 黄色
		er, //错误: Error [ERROR] | 红色
		tx, //文本
		ed  //结束: 还原 ANSIESC颜色
	};
	typedef LogMessage lm;
	LogMessage tip();
	LogMessage war();
	LogMessage err();
	LogMessage end();


#ifndef _WCHAR
	#define Tchar char
	#define Tstr string
	#define To_string to_string
	#define Tstrlen strlen
	#define Tostringstream ostringstream
	#define Tfopen_s fopen_s
	#define Tfputs fputs
	#define Tfgets fgets

#else
	#define Tchar wchar_t
	#define Tstr wstring
	#define To_string to_wstring
	#define Tstrlen wcslen
	#define Tostringstream wostringstream
	#define Tfopen_s _wfopen_s
	#define Tfputs fputws
	#define Tfgets fgetws

#endif

#ifndef _WINDOWS
#ifndef _WCHAR
	// ""
	#define _T(x) x
#else
	// L""
	#define _T(x) L ## x
#endif
#endif


#ifdef _WINDOWS
#define PATH_BACKSLASH "\\"

#else
#define PATH_BACKSLASH "/"
#endif

#define Log_ts "[INFO]"
#define Log_wr "[WARNING]"
#define Log_er "[ERROR]"


	class Log {
	public:
		static LogMessage LastMessage;
		ostream& out;
		ostream& err;

	private:
		static bool init; //初始化

		bool CMD; //控制台
		bool Release; //发行版

		bool ShowTime; //显示时间
		bool ShowLog; //显示日志


#ifdef _WINDOWS
		static HWND hConsole; //控制台句柄
#endif

		static ofstream LogFileStream_Out; //日志文件流 输出
		static bool IsLogFileWrite; //是否 写入日志文件
		static bool IsLogAllOutput; //是否 输出所有日志

#ifdef _Thread
		static mutex mutex_LogFileStream_Out;
		static queue<Tstr> IsLogFileWrite_Queue; //日志 WriteConfigFile队列
		static thread LogFileProcessing; //日志文件处理: 主要是输出到{./Log/时间_程序名.txt}文件
		static atomic<bool> IsLogFileWriteThreadStop; //是否 停止日志文件写入线程
		static condition_variable cv_LogFileQueue; //通知 日志文件队列
#endif

	public:
#ifndef _WCHAR
		Log(bool cmd, bool release, ostream& _out = cout, ostream& _err = cerr)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true), out(_out), err(_err)
		{
			Init();
		}
#else
		Log(bool cmd, bool release, ostream& _out = wcout, ostream& _err = wcerr)
			: CMD(cmd), Release(release), ShowTime(true), ShowLog(true), out(_out), err(_err)
		{
			Init();
		}
#endif


		template<class T = bool>
		ostream& operator<<(LogMessage (*_lm)())
		{
			//输出模式: out || err
			switch (_lm()) {
			case LogMessage::ts: {
				if (Log::LastMessage == lm::ed) {
					Log::LastMessage = lm::ts;
					out << ANSIESC_GREEN << Log_ts;
				}

				break;
			}
			case LogMessage::wr: {
				if (Log::LastMessage == lm::ed) {
					Log::LastMessage = lm::wr;
					err << ANSIESC_YELLOW << Log_wr;
				}

				break;
			}
			case LogMessage::er: {
				if (Log::LastMessage == lm::ed) {
					Log::LastMessage = lm::er;
					err << ANSIESC_RED << Log_er;
				}

				break;
			}
			case LogMessage::ed: {
				if (Log::LastMessage != lm::ed) {
					if (Log::LastMessage == lm::ts) {
						Log::LastMessage = lm::ed;
						out << ANSIESC_RESET; //还原颜色
					}
					else {
						Log::LastMessage = lm::ed;
						err << ANSIESC_RESET; //还原颜色
						return err;
					}
				}

				break;
			}
			}

			return out;
		}

		~Log();

	private:
		wstring StringToWstring(const string& str);
		wstring StringToWstring(string&& str);
#define stow StringToWstring
		string WstringToString(const wstring& wStr);
		string WstringToString(wstring&& wStr);
#define wtos WstringToString

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<class Temp = bool>
		void Init()
		{
			//控制台初始化
			if (!init) {
				// 设置全局区域为 "zh_CN.UTF-8"
				locale::global(locale("zh_CN.UTF-8"));

				// 使用Config进行格式化
				out.imbue(locale()); // 使用全局Config
				err.imbue(locale());

				//out << "Log Config: zh_CN.UTF-8 完成!\n";

#ifdef _WINDOWS
					//Windows 控制台编码修改: UTF-8

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);

#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					MessageBoxW(0, stow("log: 分离控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + To_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					out << "Log: 分离控制台\n" << lm::ed;
				}


				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					MessageBoxW(0, stow("log: 分配控制台失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + To_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
				}
				else {
					out << "Log: 分配控制台\n" << lm::ed;
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (FilePtr == nullptr) {
					MessageBoxW(0, stow("日志: 重定向标准输出流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (FilePtr == nullptr) {
					MessageBoxW(0, stow("日志: 重定向标准错误流失败!").c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					MessageBoxW(0, stow("错误代码: " + to_string(GetLastError())).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
					return;
				}

				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#endif

				// 获取控制台窗口的句柄
				hConsole = GetConsoleWindow();
#endif
				if (IsLogFileWrite) {
					//err << "Log 日志WriteConfigFile: 开始\n";

					//获取 当前路径/Log/Log文件名.txt 
					//创建文件夹 ./Log  .
					Tstr Log_FolderName = (Tstr)"." + PATH_BACKSLASH + "Log";
					if (CreateFolder(Log_FolderName)) {
					}
					else {
						err << wr
							<< "Log: 文件夹[" + Log_FolderName + "]创建失败!\n"
							<< ed;

						Log_FolderName = (Tstr)".";
					}

					//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
					Tstr Log_FileName = GetFormattingTime("%Y-%m-%d_%H-%M-%S", "", "") + ".txt";
					// ./Log/时间.txt  ||  ./时间.txt
					Tstr Log_FilePath = Log_FolderName + PATH_BACKSLASH + Log_FileName; //总是添加 /

					//打开文件
					err << "Log: 日志输出文件名 " << Log_FilePath << "\n" << endl;
					LogFileStream_Out = move(ofstream(Log_FilePath, ios::out));
					if (!LogFileStream_Out) {
						err << lm::er
							<< "Log: " << Log_FilePath << " 打开文件失败!\n"
							<< lm::ed;
						return;
					}
					err << lm::ts
						<< "Log: " + Log_FilePath + " 打开文件成功!\n"
						<< lm::ed;
					//err << "Log: 文件 开始写入!\n";
#ifdef _Thread
					//初始化: 日志WriteConfigFile线程
					LogFileProcessing = thread(&Log::LogWirteToFile,
						this);
					LogFileProcessing.detach(); //分离线程
					err << "Log: 日志写入线程 分离到后台\n";
#endif
				}

				//完成初始化
				init = true;
			}
		}


	private:
		void LogWirteToFile()
		{
#ifdef _Thread
			try {
				//不是停止线程时
				while (!IsLogFileWriteThreadStop.load()) {
					//锁 日志文件流
					unique_lock<mutex> queueLock(mutex_LogFileStream_Out); // 上锁
					cv_LogFileQueue.wait(queueLock, [this] {
						return !IsLogFileWrite_Queue.empty() || IsLogFileWriteThreadStop.load();
					});

					if (IsLogFileWriteThreadStop.load() && IsLogFileWrite_Queue.empty()) {
						break; // 退出循环
					}
					
					//日志队列非空
					if (!IsLogFileWrite_Queue.empty()) {
						LogFileStream_Out << IsLogFileWrite_Queue.front();
						IsLogFileWrite_Queue.pop();
					}
					else { //日志为空: 等待 200ms
						queueLock.unlock();
						this_thread::sleep_for(chrono::milliseconds(200));
					}

				}
			}
			catch (const exception& e) {
				err << "Typical_Tool::Log::LogWriteToFile() - " << e.what() << "\n";
			}
			catch (...) {
				err << "Typical_Tool::Log::LogWirteToFile()\n";
			}
#endif
		}

		template<class T = bool>
		void StopLogWrite()
		{
#ifdef _Thread
			IsLogFileWriteThreadStop.store(true);
			cv_LogFileQueue.notify_one(); // 通知线程退出
			out << "Log: 日志写入线程 退出\n" << endl;
#endif
		}

	private:

		template<class T = bool>
		void SetConsoleTextColor_Error(const Tstr& _ANSIESC)
		{
			err << _ANSIESC;
		}
		template<class T = bool>
		void ConsoleOutput_Error(const Tstr& _text)
		{
			err << _text;
		}
		template<class T = bool>
		void ReSetConsoleTextColor_Error()
		{
			err << ANSIESC_RESET;
		}

		template<class T = bool>
		void SetConsoleTextColor(const Tstr& _ANSIESC)
		{
			out << _ANSIESC;
		}
		template<class T = bool>
		void ConsoleOutput(const Tstr& _text)
		{
			out << _text;
		}
		template<class T = bool>
		void ReSetConsoleTextColor()
		{
			out << ANSIESC_RESET;
		}

		// _WINDOWS || _CONSOLE
		template<class T = bool>
		void Logs_ustr(const Tstr& text, const LogMessage& lm)
		{
			switch (lm) {
			case LogMessage::ts: {
				if (CMD) {
					Tstr temp;
					SetConsoleTextColor(ANSIESC_GREEN); //在时间输出之前
					if (ShowTime) {
						out << GetFormattingTime();
					}
					temp = (Tstr)Log_ts + text;
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					if (IsLogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (IsLogAllOutput) {
								IsLogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (IsLogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_ts).c_str(), MB_OK);
#else
					MessageBoxW(NULL, text.c_str(), Log_ts, MB_OK);
#endif
#endif
				}

				break;
			}
			case LogMessage::wr: {
				if (CMD) {
					Tstr temp;
					SetConsoleTextColor(ANSIESC_YELLOW); //在时间输出之前
					if (ShowTime) {
						out << GetFormattingTime();
					}
					temp = (Tstr)Log_wr + text + "\n";
					ConsoleOutput(temp);
					ReSetConsoleTextColor();

					//WriteConfigFile log日志
					if (IsLogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (IsLogAllOutput) {
								IsLogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (IsLogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_wr).c_str(), MB_ICONWARNING);
#else
					MessageBoxW(NULL, text.c_str(), Log_wr, MB_ICONWARNING);
#endif
#endif
				}

				break;
			}
			case LogMessage::er: {
				if (CMD) {
					Tstr temp;
					SetConsoleTextColor_Error(ANSIESC_RED); //在时间输出之前
					if (ShowTime) {
						err << GetFormattingTime();
					}
					temp = (Tstr)Log_er + text + "\n";
					ConsoleOutput_Error(temp);
					ReSetConsoleTextColor_Error();

					//WriteConfigFile log日志
					if (IsLogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁
							IsLogFileWrite_Queue.push(temp);
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
					}
				}
				else {
#ifdef _WINDOWS
#ifndef _WCHAR
					MessageBoxW(NULL, stow(text).c_str(), stow(Log_er).c_str(), MB_ICONSTOP);
#else
					MessageBoxW(NULL, text.c_str(), Log_er, MB_ICONSTOP);
#endif
#endif
				}

				break;
			}
			case LogMessage::tx: {
				if (CMD) {
					if (ShowTime) {
						out << GetFormattingTime();
					}
					Tstr temp = text + '\n';
					ConsoleOutput(temp);

					//WriteConfigFile log日志
					if (IsLogFileWrite) {
#ifdef _Thread
						{
							//锁 日志WriteConfigFile队列
							lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁

							if (IsLogAllOutput) {
								IsLogFileWrite_Queue.push(temp);
							}
						}
						cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
						if (IsLogAllOutput) {
							if (ShowTime) {
								LogFileStream_Out << GetFormattingTime();
							}
							LogFileStream_Out << temp;
						}
					}
					return;
				}
			}
			}
		}
		template<class T = bool>
		void Logs_lgm()
		{
			if (CMD) {
				if (ShowTime) {
					out << GetFormattingTime();
				}
				Tstr temp = "\n";
				ConsoleOutput(temp);

				//WriteConfigFile log日志
				if (IsLogFileWrite) {
#ifdef _Thread
					{
						//锁 日志WriteConfigFile队列
						lock_guard<mutex> lock(mutex_LogFileStream_Out); // 上锁

						if (IsLogAllOutput) {
							IsLogFileWrite_Queue.push(temp);
						}
					}
					cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
					if (IsLogAllOutput) {
						if (ShowTime) {
							LogFileStream_Out << GetFormattingTime();
						}
						LogFileStream_Out << temp;
					}
				}
				return;
			}
		}


		// _DEBUG || NDEBUG
		template<class T = bool>
		void Logs()
		{
#ifdef _DEBUG
			Logs_lgm();
			return;
#endif
			if (Release) {
				Logs_lgm();
			}
		}

	public:

		//显示/隐藏 Log消息
		template<class T = bool>
		void operator()(const Tstr& text, const LogMessage& lm = lm::tx)
		{
			if (ShowLog) {
				Logs_ustr(text, lm);
			}
		}
		template<class T = bool>
		void operator()(Tstr&& text, const LogMessage& lm = lm::tx)
		{
			if (ShowLog) {
				Logs_ustr(text, lm);
			}
		}
		template<class T = bool>
		void operator()()
		{
			if (ShowLog) {
				Logs();
			}
		}


	private:

		// _WINDOWS: Win32 API
		template<class T = bool>
		bool CreateFolder(const Tstr& folderPath)
		{
#ifdef _WINDOWS
			DWORD attributes = GetFileAttributes(folderPath.c_str());

			// 检查路径是否存在且不是目录  
			if (attributes == INVALID_FILE_ATTRIBUTES) {
				// 路径不存在或出错，尝试创建目录  
				if (CreateDirectory(folderPath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
					out << "Log: 文件夹[" + folderPath + "]创建成功!" + "\n";
					// 创建成功
					return true;
				}
				err << "Log: 文件夹[" + folderPath + "]创建失败!" + "\n";
				// 创建失败且不是因为路径已存在  
				return false;
			}
			else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
				err << "Log: 文件夹[" + folderPath + "]已存在" + "\n";
				// 路径已经是一个目录  
				return true;
			}
			// 路径存在但不是目录（可能是一个文件）  
			err << "Log: 文件夹[" + folderPath + "]创建失败(路径存在, 但不是目录)!" + "\n";
#else
			err << "Log: 文件夹[" + folderPath + "]创建失败(#ifndef _WINDOWS)!" + "\n";
#endif
			return false;
		}


	public:

		//设置日志显示
		void SetShowLog(bool showLog);
		//设置时间显示
		void SetShowTime(bool showTime);
		//设置控制台显示: _WINDOWS(Win32 API)
		void SetShowConsole(bool showConsole);

	private:
		template<class Temp = bool>
		Tstr GetFormattingTime(const Tstr& timeFormat = "%Y-%m-%d %H:%M:%S",
			const Tstr& textLeftFormat = "[", const Tstr& textRigthFormat = "]")
		{
			chrono::system_clock::time_point now = chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			time_t times = chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（tm结构）
			tm* now_tm = localtime(&times);

			// 使用 put_time 格式化时间
			Tostringstream oss;
			oss << put_time(now_tm, timeFormat.c_str()); // 自定义时间格式

			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (textLeftFormat == "" && textRigthFormat == "") {
				return oss.str();
			}
			return textLeftFormat + oss.str() + textRigthFormat;
		}

	public:

		/* 设置所有日志写入
		* level:
		* er: Error log level output
		* !=3: All(lm: ts/wr/tx) log level output
		*/
		template<class T = bool>
		static void SetAllIsLogFileWrite(bool logFileWrite, int logLevel = lm::er)
		{
			IsLogFileWrite = logFileWrite;

			//输出所有级别
			if (logLevel != lm::er) {
				IsLogAllOutput = true;

				out << ANSIESC_YELLOW 
					<< "Log: File Output level >> All <<"
					<< ANSIESC_RESET << endl;
			}
			else {
				IsLogAllOutput = false;

				out << ANSIESC_YELLOW
					<< "Log: File Output level >> Error <<"
					<< ANSIESC_RESET << endl;
			}
		}
	};

	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, err
	*/
	static Log lg(false, false);
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, err
	*/
	static Log lgc(true, false);
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, err
	*/
	static Log lgr(false, true);
	/* 模式 mode: (_CONSOLE) && #ifndef _DEBUG
	* tip, war, err
	*/
	static Log lgcr(true, true);


	template<class T = bool>
	Log& operator<<(Log & _Log, const Tstr & _str)
	{
		if (Log::LastMessage == lm::er) {
			_Log.err << _str;
		}
		else {
			_Log.out << _str;
		}

		if (Log::IsLogFileWrite) {
#ifdef _Thread
			{
				//锁 日志WriteConfigFile队列
				if (Log::IsLogAllOutput) {
					lock_guard<mutex> lock(Log::mutex_LogFileStream_Out); // 上锁
					Log::IsLogFileWrite_Queue.push(_str);
				}
			}
			Log::cv_LogFileQueue.notify_one(); // 通知线程有新消息
#endif
			if (Log::IsLogAllOutput) {
				if (Log::ShowTime) {
					Log::LogFileStream_Out << Log::GetFormattingTime();
				}
				Log::LogFileStream_Out << _str;
			}
		}
	}
	template<class T = bool>
	ostream& operator<<(ostream& _ostream, LogMessage (*_lm)())
	{
		//输出模式: out || err
		switch (_lm()) {
		case LogMessage::ts: {
			if (Log::LastMessage == lm::ed) {
				Log::LastMessage = lm::ts;
				_ostream << ANSIESC_GREEN << Log_ts;
			}

			break;
		}
		case LogMessage::wr: {
			if (Log::LastMessage == lm::ed) {
				Log::LastMessage = lm::wr;
				_ostream << ANSIESC_YELLOW << Log_wr;
			}

			break;
		}
		case LogMessage::er: {
			if (Log::LastMessage == lm::ed) {
				Log::LastMessage = lm::er;
				_ostream << ANSIESC_RED << Log_er;
			}

			break;
		}
		case LogMessage::ed: {
			if (Log::LastMessage != lm::ed) {
				Log::LastMessage = lm::ed;
				_ostream << ANSIESC_RESET; //还原颜色
			}

			break;
		}
		}

		return _ostream;
	}

	template<class Temp = bool>
	void Log_README()
	{
		lgcr <<
		"#define _ANSIESC_CONSOLE_CHAR_COLOR: 控制台转义字符修改字符颜色\n\
\n\
		#ifndef _DEBUG: _WINDOWS/_CONSOLE\n\
		lgr("文件打开失败!", err); \n\
		_DEBUG: _WINDOWS/_CONSOLE\n\
		lg("关闭窗口", tip); \n\
\n\
		#ifndef _DEBUG: _CONSOLE\n\
		lgcr(\"Code: 404\", err); \n\
		lgcr << tip << \"关闭文件!\'n\'\" << end; \n\
		//_DEBUG: _CONSOLE\n\
		lgc(\"Log.h\", lm::tx); \n\
		lgc << war << \"没有对应文件!\"\n"
		<< end;
	}
}


#endif
