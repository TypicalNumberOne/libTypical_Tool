#pragma once

//工具配置文件
#ifndef _TOOLSCONFIGFILE_H
#define _TOOLSCONFIGFILE_H

//#include "Typical_Tool.h"

#include "pch.h"
#include "CommonTools.h"
#include "ConfigFileTextManage.h"
#include "Log.h"
#include "Shell.h"

using namespace Typical_Tool::StringManage;
using Typical_Tool::WindowsSystem::WindowShell;

namespace Typical_Tool {
	//工具箱配置文件
	class  ToolsConfigFile
	{
	private:
		Tchar 程序所在路径[MAX_PATH] = { 0 };

		// "C:\\Users\\22793\\source\\repos\\Tools\\Debug\\Tools.exe"
		Tstr 程序_路径名 = "";
		// "C:\\Users\\22793\\source\\repos\\Tools\\Debug"
		Tstr 程序父文件夹_路径名 = "";
		// "C:\\Users\\22793\\source\\repos\\Tools\\Debug\\config\\ToolsConfig.ini"
		Tstr 工具箱配置文件_路径名 = "";

	private:
		//工具箱程序 文件名: \\Tools
		Tstr 程序_名 = "";
		//工具箱配置 文件名: \\ToolsConfig
		Tstr 工具箱配置文件_名 = "\\ToolsConfig";

	public:
		ToolsConfigFile() {}

		template<class T = bool>
		void 初始化(Tstr 程序_名)
		{
			this->程序_名 = "\\" + 程序_名;

			//获取当前程序的全路径
			GetModuleFileName(NULL, this->程序所在路径, MAX_PATH);

			//工具箱程序所在路径
			this->程序_路径名 = this->程序所在路径;

			//程序父文件夹路径名
			int 工具箱文件_路径名长度;
			工具箱文件_路径名长度 = (this->程序_名 + ".exe").size(); //Tools.exe 长度
			int temp程序父文件夹_路径名长度;
			temp程序父文件夹_路径名长度 = this->程序_路径名.size() - 工具箱文件_路径名长度; //程序父文件夹路径 = 程序全路径 - 程序名
			Tstr temp程序父文件夹_路径名(" ", temp程序父文件夹_路径名长度);
			for (int i = 0; i < temp程序父文件夹_路径名长度; i++)
			{
				temp程序父文件夹_路径名[i] = this->程序_路径名[i];
			}
			this->程序父文件夹_路径名 = temp程序父文件夹_路径名;

			//配置文件
			this->工具箱配置文件_路径名 = this->程序父文件夹_路径名 + "\\Config" + "\\ToolsConfig.ini";
		}

	public:
		//获取 工具箱配置文件路径
		template<class T = bool>
		Tstr Get工具箱配置文件路径() const
		{
			return this->工具箱配置文件_路径名;
		}
		template<class T = bool>
		Tstr Get程序父文件夹路径() const
		{
			return this->程序父文件夹_路径名;
		}
		template<class T = bool>
		Tstr Get程序路径() const
		{
			return this->程序_路径名;
		}

	public:
		template<class T = bool>
		void 打开配置文件()
		{
			auto temp = WindowShell::ExecuteAnalyze("打开配置文件", "打开文件", this->工具箱配置文件_路径名);
			if (temp.IsSucceed()) {
				lgc("成功: 打开配置文件", lm::wr);
			}
		}
	};
}
#endif
