#include "App.h"

USING_NS_WF;

int main(int argc, char **argv)
{
	App& app = App::GetInstance();
	if (!app.Init(argc, argv))
	{
		return 0;
	}
	app.Run();
	return 0;
}