#include "Global.h"

Global::Global(Context *contex) :
	Object(contex)
{

}

Audio* audio = nullptr; 
ResourceCache* cache = nullptr; 
Engine* engine = nullptr; 
Graphics* graphics = nullptr; 
Input* input = nullptr; 
Localization* localization = nullptr; 
Renderer* renderer = nullptr; 
Time* time = nullptr; 
UI* ui = nullptr; 

Console* console = nullptr; 
DebugHud* debugHud = nullptr; 

Global* global = nullptr; 

void InitGlobal(Context* context)
{
	audio = context->GetSubsystem<Audio>();
	cache = context->GetSubsystem<ResourceCache>();
	engine = context->GetSubsystem<Engine>();
	graphics = context->GetSubsystem<Graphics>();
	localization = context->GetSubsystem<Localization>();
	renderer = context->GetSubsystem<Renderer>();
	time = context->GetSubsystem<Time>();
	ui = context->GetSubsystem<UI>();

	XMLFile* style = GET_XML_FILE("UI/DefaultStyle.xml");

	console = engine->CreateConsole();
	console->SetDefaultStyle(style); 
	console->GetBackground()->SetOpacity(0.8f);

	debugHud = engine->CreateDebugHud();
	debugHud->SetDefaultStyle(style); 

	context->RegisterSubsystem(global = new Global(context)); 
}