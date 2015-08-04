#ifndef CROSS_PLATFORM_DETAILS_H_H_H
#define CROSS_PLATFORM_DETAILS_H_H_H

#include "CPLog_Classes.h"

#ifdef USE_CPLOG

#ifdef WIN32
#define CPLOG_FUNCTION_NAME __FUNCTION__
#else
//#define CPLOG_FUNCTION_NAME __PRETTY_FUNCTION__
#define CPLOG_FUNCTION_NAME __func__
#endif

#include <iostream>
#include <fstream>
#include <deque>

namespace CPLog
{

#ifdef WIN32
#include <windows.h>


	struct CPLog_ConsoleOut_Win32:public CPLog_Out
	{
		CPLog_ConsoleOut_Win32()
		{
			AllocConsole();
			freopen("conout$", "w+t", stdout);
			SetConsoleTitleA("Debug Console");
			setlocale(LC_ALL, "");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}

		virtual void printLog(const std::string &text, long type, bool bChangeLine)
		{
			static const WORD colorArray[] = { 
				FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,	//DEAULT_TYPE
				FOREGROUND_GREEN | FOREGROUND_INTENSITY,								//FUNCTION_TYPE
				FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,				//MESSAGE_TYPE
				FOREGROUND_RED | FOREGROUND_INTENSITY,									//ERROR_TYPE
				FOREGROUND_BLUE | FOREGROUND_INTENSITY,									//INDENTATION_TYPE
				FOREGROUND_GREEN,														//DESCRIBE_TYPE
				FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY	//SPACE_TYPE
				
			};
			//assert(type > 0 && type < 4);
			WORD colorType = colorArray[type];
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorType);
			if (bChangeLine)
			{
				std::cout << text << std::endl;
			}
			else
			{
				std::cout<<text<<std::flush;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorArray[0]);
		}

	private:
		
	};

	struct CPLog_TXTOut_WIN32 :public CPLog_Out
	{
		CPLog_TXTOut_WIN32(const std::string &txtName)
		:_file(txtName)
		{

		}
		virtual void printLog(const std::string &text, long type, bool bChangeLine)
		{
			if (bChangeLine)
			{
				_file << text << std::endl;
			}
			else
			{
				_file << text << std::flush;
			}
		}
	private:
		std::ofstream _file;
	};

#define CPLOG_TEXT_OUT_TYPE CPLog_TXTOut_WIN32
	
#define CPLOG_CONSOLE_OUT_TYPE CPLog_ConsoleOut_Win32

#endif



#ifdef COCOS2D_VERSION

	struct CPLog_CocosOutData
	{
		std::string _lineString;
		int _dataID;
		int _type;
		bool _bChangeLine;
	};

	class CPLog_Cocos2dOut:public CPLog_Out
	{
	private:
		CPLog_Cocos2dOut()
			:_maxLineCount(40),_dataID_top(0),_curLineCount(0)
		{

		}

	public:
		static CPLog_Cocos2dOut *getInstance()
		{
			static CPLog_Cocos2dOut *pInstance=new CPLog_Cocos2dOut;
			return pInstance;
		}
		void setMaxLineNum(int maxLineCount)
		{
			_maxLineCount=maxLineCount;
		}

		void clearLines()
		{
			_deque.clear();
			_curLineCount=0;
		}

		//TODO printLog(...)
		virtual void printLog(const std::string &text, long type, bool bChangeLine) override
		{
			if(bChangeLine)
			{
				++_curLineCount;
				if(_curLineCount>_maxLineCount)
				{
					for(;;)
					{
						bool bChangeLine=_deque.front()._bChangeLine;
						_deque.pop_front();
						if(bChangeLine)
						{
							--_curLineCount;
							if(_curLineCount<=_maxLineCount)
							{
								break;
							}
						}
					}
				}
			}
			_deque.push_back({text,++_dataID_top,type,bChangeLine});
			
		}

		std::deque<CPLog_CocosOutData> &getDataDeque()
		{
			return _deque;
		}

	private:
		int _maxLineCount;
		int _curLineCount;

		int _dataID_top;

		std::deque<CPLog_CocosOutData> _deque;
	};

	class CPLog_CocosOutNode:public cocos2d::Node
	{
	public:
		static CPLog_CocosOutNode *create()
		{
			auto pRes=new CPLog_CocosOutNode;
			if(pRes && pRes->init())
			{
				pRes->autorelease();
				return pRes;
			}
			CC_SAFE_DELETE(pRes);
			return nullptr;
		}

		virtual bool init() override
		{
			_pBaseNode=cocos2d::Node::create();
			addChild(_pBaseNode);
			return true;
		}
		virtual void onEnter() override
		{
			cocos2d::Node::onEnter();

			_out=CPLog_Cocos2dOut::getInstance();
			scheduleUpdate();
			update(0.0f);
		}

		virtual void update(float) override
		{
			auto &dataDeque=_out->getDataDeque();
			int minID=0;
			int maxID=0;
			if(!dataDeque.empty())
			{
				minID=dataDeque.front()._dataID;
				maxID=dataDeque.back()._dataID;
			}
			if(minID>_minDataID)
			{
				removeLabelWithUpperDataID(minID);
				_minDataID=minID;
			}
			if(_maxDataID<maxID)
			{
				auto it=dataDeque.begin();
				for(;it!=dataDeque.end();++it)
				{
					if(_maxDataID < it->_dataID)
					{
						break;
					}
				}
				for(;it!=dataDeque.end();++it)
				{
					addLabelWithOutData(*it);
				}
				_maxDataID=maxID;
			}
		}

		void addLabelWithOutData(CPLog_CocosOutData &data)
		{
			static const cocos2d::Color4B colorArray[]={
				{255,255,255,128},
				{0,255,0,222},
				{255,255,0,222},
				{255,64,64,255},
				{255,192,0,222},
				{0,128,0,222},
				{255,255,255,222}
			};
			
			//cocos2d::LabelTTF *pTTF=cocos2d::LabelTTF::create(data._lineString,"",16);
			cocos2d::Label *pTTF=cocos2d::Label::createWithSystemFont(data._lineString,"ºÚÌå",20);
			pTTF->setTextColor(colorArray[data._type]);
			pTTF->setAnchorPoint({0.0f,0.0f});

			pTTF->setPosition(_drawPosition);
			pTTF->setTag(data._dataID);

			if(data._bChangeLine)
			{
				_drawPosition.x=0.0f;
				_drawPosition.y-=_lineSpace;
				_pBaseNode->setPosition(_pBaseNode->getPosition()+cocos2d::Point(0.0f,_lineSpace));

			}
			else
			{
				_drawPosition.x+=pTTF->getContentSize().width;
			}
			_pBaseNode->addChild(pTTF);
		}

		void removeLabelWithUpperDataID(int upperDataID)
		{
			std::vector<cocos2d::Node *> vecForRemove;
			auto &childrenVec=_pBaseNode->getChildren();
			for(int i=0;i<_pBaseNode->getChildrenCount();++i)
			{
				cocos2d::Node *pNode=childrenVec.at(i);				
				
				if(pNode->getTag()<upperDataID)
				{
					vecForRemove.push_back(pNode);
				}
			}

			for(unsigned int i=0;i<vecForRemove.size();++i)
			{
				vecForRemove[i]->removeFromParent();
			}
		}


	private:
		CPLog_Cocos2dOut *_out=nullptr;

		int _minDataID=0;
		int _maxDataID=0;

		int _lineSpace=20;

		cocos2d::Point _drawPosition={0.0f,0.0f};

		cocos2d::Node *_pBaseNode=nullptr;
	};

#endif

#if defined(CC_TARGET_PLATFORM) && CC_PLATFORM_ANDROID==CC_TARGET_PLATFORM

#include "platform/android/CCFileUtils-android.h"
	struct CPLog_TXTOut_Cocos :public CPLog_Out
	{
		CPLog_TXTOut_Cocos(const std::string &txtName)
		{
			std::string path=cocos2d::FileUtilsAndroid::getInstance()->getWritablePath()+txtName;
			_file.open(path);
			//_file.open(txtName);
		}
		~CPLog_TXTOut_Cocos()
		{
			
		}
		virtual void printLog(const std::string &text, long type, bool bChangeLine)
		{
			if (bChangeLine)
			{
				_file << text << std::endl;
			}
			else
			{
				_file << text << std::flush;
			}
		}
	private:
		std::ofstream _file;
	};

#define CPLOG_TEXT_OUT_TYPE CPLog_TXTOut_Cocos

#endif


	void CPLog_Manager::init()
	{
#if defined(CPLOG_CONSOLE_OUT_TYPE)
		auto pStream1 = new CPLOG_CONSOLE_OUT_TYPE;
		addStream(pStream1);
#endif

#if defined(CPLOG_TXT_FILE_NAME)  &&  defined(CPLOG_TEXT_OUT_TYPE)
		auto pStream2 = new CPLOG_TEXT_OUT_TYPE(CPLOG_TXT_FILE_NAME);
		addStream(pStream2);
#endif

#if defined(COCOS2D_VERSION)
		auto pStream3=CPLog_Cocos2dOut::getInstance();
		addStream(pStream3);
#endif
	}
}

#endif

#endif