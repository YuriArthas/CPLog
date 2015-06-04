#ifndef CROSS_PLATFORM_CLASSES_H_H_H
#define CROSS_PLATFORM_CLASSES_H_H_H

#include <string>
#include <vector>

#ifdef USE_CPLOG






namespace CPLog
{

	enum { DEFAULT_TYPE, FUNCTION_TYPE, MESSAGE_TYPE, ERROR_TYPE, INDENTATION_TYPE, DESCRIBE_TYPE, SPACE_TYPE };

	struct CPLog_Log_Base
	{
		inline void printLog(const std::string &text, long type,bool bChangeLine);
		inline void printOffset();
		inline void addOffsetStep();
		inline void subOffsetStep();

		
	};

	struct CPLog_Block_Enter :public CPLog_Log_Base
	{
		
		CPLog_Block_Enter(const std::string &block_info,const std::string &discribe)
			:_block_info(block_info), _discribe(discribe)
		{
			onLogEnter();
		}

		~CPLog_Block_Enter()
		{
			onLogExit();
		}


	private:
		void onLogEnter()
		{
			printOffset();
			printLog(_block_info, FUNCTION_TYPE, false);
			printLog("  //", SPACE_TYPE, false);
			printLog(_discribe, DESCRIBE_TYPE, true);
			printOffset();
			printLog("{", INDENTATION_TYPE, true);
			addOffsetStep();
		}
		void onLogExit()
		{
			subOffsetStep();
			printOffset();
			printLog("}", INDENTATION_TYPE, true);
		}

		std::string _block_info;
		std::string _discribe;
		
	};
	struct CPLog_Message :public CPLog_Log_Base
	{
		CPLog_Message(const std::string &message)
		{
			printOffset();
			printLog(message, MESSAGE_TYPE, true);
		}
	};



	struct CPLog_Data
	{
		//int _offset = 0;			//缩进
		int _stepOffset = 4;		//每次缩进步长
		int _initOffset = 0;		//初始缩进

		int _stepCount = 0;

		CPLog_Data(int setpOffset,int initOffset)
			: _stepOffset(setpOffset), _initOffset(initOffset){}

		CPLog_Data(const CPLog_Data &data) = default;

		//忽略data中的_stepCount
		//按照this->_stepCount换算出_offset,_stepOffset,_initOffset
		void setDataInfo(const CPLog_Data &data)
		{	
			_stepOffset = data._stepOffset;
			_initOffset = data._initOffset;
		}
	};

	struct CPLog_Out
	{
		virtual ~CPLog_Out(){}
		virtual void printLog(const std::string &text, long type, bool bChangeLine) = 0;
	};
	
	class CPLog_Manager
	{
	private:
		
		CPLog_Data _data = { 4, 0 };

		
		std::vector<CPLog_Out *> _outVec;
	public:

		static CPLog_Manager &getInstance()
		{
			static CPLog_Manager s_mgr;
			return s_mgr;
		}

		void setCPLogData(const CPLog_Data &data)
		{
			_data = data;
		}
		CPLog_Data getCPLogData() const
		{
			return _data;
		}

		int getOffset() const
		{
			return _data._initOffset + _data._stepCount*_data._stepOffset;
		}

		void addOffset()
		{
			_data._stepCount++;
		}
		void subOffset()
		{
			_data._stepCount--;
		}
		void printOffset()
		{
			std::string s(getOffset(), ' ');
			printLog(s, SPACE_TYPE, false);
		}


		void printLog(const std::string &text, long type, bool bChangeLine)
		{
			for (auto p : _outVec)
			{
				p->printLog(text, type, bChangeLine);
			}
		}

		//只有add没有remove
		void addStream(CPLog_Out *s)
		{
			_outVec.push_back(s);
		}
		

	private:
		//virtual void _printLog(const std::string &text, long type, bool bChangeLine) = 0;

		CPLog_Manager()
		{
			init();
		}
		virtual ~CPLog_Manager()
		{ 
			for (auto streamPointer : _outVec) 
			{
				delete streamPointer;
			}
		}

		//在CPLog_Details里面实现,返回真正的实例
		inline void init();
	};

	void CPLog_Log_Base::printLog(const std::string &text, long type,bool bChangeLine)
	{
		CPLog_Manager::getInstance().printLog(text, type, bChangeLine);
	}

	void CPLog_Log_Base::subOffsetStep()
	{
		CPLog_Manager::getInstance().subOffset();

	}

	void CPLog_Log_Base::addOffsetStep()
	{
		CPLog_Manager::getInstance().addOffset();
	}

	void CPLog_Log_Base::printOffset()
	{
		CPLog_Manager::getInstance().printOffset();
	}

	
}

#endif

namespace CPLog{
	static const char *printToBuff(const char *pFormat, ...)
	{
		static char s_buff[4096] = {};
		va_list args;
		va_start(args, pFormat);
		vsprintf(s_buff, pFormat, args);
		va_end(args);
		return s_buff;
	}
}
#endif