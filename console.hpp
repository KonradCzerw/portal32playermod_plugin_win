#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <functional>
#include "utils.hpp"

#define TIER0 "tier0"
#define CONCOLORMSG_SYMBOL "?ConColorMsg@@YAXABVColor@@PBDZZ"
#define DEVMSG_SYMBOL "?DevMsg@@YAXPBDZZ"
#define DEVWARNINGMSG_SYMBOL "?DevWarning@@YAXPBDZZ"

#define MSG_SYMBOL "Msg"
#define WARNING_SYMBOL "Warning"

#define DEFAULT_COLOR Color(1, 240, 111)

class Console {
public:
	using _Msg = void(*)(const char* pMsgFormat, ...);
	using _Warning = void(*)(const char* pMsgFormat, ...);
	using _ColorMsg = void(*)(const Color& clr, const char* pMsgFormat, ...);
	using _DevMsg = void(*)(const char* pMsgFormat, ...);
	using _DevWarning = void(*)(const char* pMsgFormat, ...);
	using _LoggingSystem_RegisterLoggingListener = void (*)(ILoggingListener* listener);
	using _LoggingSystem_PushLoggingState = void (*)(bool threadLocal, bool clearState);
	using _LoggingSystem_PopLoggingState = void (*)(bool threadLocal);
	using _LoggingSystem_HasTag = bool (*)(int channelID, const char* tag);
	using _LoggingSystem_SetChannelSpewLevelByTag = void (*)(const char* tag, LoggingSeverity severity);

	_Msg Msg = nullptr;
	_ColorMsg ColorMsg = nullptr;
	_Warning Warning = nullptr;
	_DevMsg DevMsg = nullptr;
	_DevWarning DevWarning = nullptr;
	_LoggingSystem_PushLoggingState LoggingSystem_PushLoggingState = nullptr;
	_LoggingSystem_PopLoggingState LoggingSystem_PopLoggingState = nullptr;
	_LoggingSystem_RegisterLoggingListener LoggingSystem_RegisterLoggingListener = nullptr;
	_LoggingSystem_HasTag LoggingSystem_HasTag = nullptr;
	_LoggingSystem_SetChannelSpewLevelByTag LoggingSystem_SetChannelSpewLevelByTag = nullptr;

public:
	template <typename... T> void Print(const char* fmt, T... args) {
		this->ColorMsg(DEFAULT_COLOR, fmt, args...);
	}

	void DumpHex(const void* data, size_t size);

	virtual ~Console() = default;
	bool Init();
	void Shutdown();
	const char* Name() { return MODULE(TIER0); }
};

class ConsoleListener : private ILoggingListener {
public:
	ConsoleListener(std::function<void (const char*)> cbk);
	virtual ~ConsoleListener();

private:
	virtual void Log(const LoggingContext* ctx, const char* msg);
	std::function<void (const char*)> cbk;
};

extern Console* console;

#endif // CONSOLE_HPP
