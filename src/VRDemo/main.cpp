#include "VRDemoGame.h"

/*================================================================================
*
*	Developer: Stuart D. Adams
*	B00265262
*	@stuartdadams
*	https://uk.linkedin.com/in/stuartdadams
*
*	Advanced Graphics Programming,
*	Bsc. (Hons) Computer Games Technology,
*	University of the West of Scotland,
*	2017
*
*================================================================================*/

#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

int main(int argc, char* argv[])
{
    VRDemoGame vr_demo_game;
    return vr_demo_game.Run();
}
