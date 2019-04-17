#include "FWMain.h"
FWMain*	 FWMain::instance = NULL;
FWMain::FWMain() : FWWindows("Simple Chess",510,530)
{
	Entry(0, new Chess);
	Warp(0);
}
FWMain::~FWMain()
{

}
