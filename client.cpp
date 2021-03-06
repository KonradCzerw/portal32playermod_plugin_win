#include "client.hpp"

#include "offsets.hpp"
#include "command.hpp"
#include "console.hpp"
#include "server.hpp"
#include "vscript.hpp"

#include <string>
#include <memory>

REDECL(Client::MsgFunc_SayText2);

DETOUR(Client::MsgFunc_SayText2, bf_read &msg) {
	bf_read pre = msg;

	int id = (int)msg.ReadUnsigned(8);

	while(msg.ReadUnsigned(8));
	while(msg.ReadUnsigned(8));

	auto buf = std::make_unique<char[]>(256);
	char* temp = buf.get();
	while (char c = (char)(uint8_t)msg.ReadUnsigned(8)) {
		*temp = (c != '\n') ? c : 0;
		temp++;
	}

	vscript->DoChatCallbacks(id, buf.get());

	msg = pre;

	return Client::MsgFunc_SayText2(thisptr, msg);
}

Client::Client() {}

bool Client::Init() {
	this->g_ClientDLL = Interface::Create(this->Name(), "VClient016");
	if(this->g_ClientDLL) {
		auto leaderboard = Command("+leaderboard");
		if(!!leaderboard) {
			using _GetHud = void* (*)(int unk);
			using _FindElement = void* (__thiscall*)(void* thisptr, const char* pName);
			
			auto cc_leaderboard_enable = (uintptr_t)leaderboard.ThisPtr()->m_pCommandCallback;
			auto GetHud = Memory::Read<_GetHud>(cc_leaderboard_enable + Offsets::GetHud);
			auto FindElement = Memory::Read<_FindElement>(cc_leaderboard_enable + Offsets::FindElement);

			auto CHudChat = FindElement(GetHud(-1), "CHudChat");
			if((this->g_HudChat = Interface::Create(CHudChat)) != nullptr) {
				this->ChatPrintf = g_HudChat->Original<_ChatPrintf>(Offsets::ChatPrintf);
				this->g_HudChat->Hook(Client::MsgFunc_SayText2_Hook, Client::MsgFunc_SayText2, Offsets::MsgFunc_SayText2);
			}
		}
	}

	return this->g_ClientDLL && this->g_HudChat;
}

void Client::Shutdown() {
	this->g_HudChat->Unhook(Offsets::MsgFunc_SayText2);
	Interface::Delete(this->g_HudChat);
}

Client* client;