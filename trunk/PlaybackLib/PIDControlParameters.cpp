#include "StdAfx.h"
#include "PIDControlParameters.h"

using namespace PlaybackLib;

CPIDControlParameters::CPIDControlParameters(void)
{
	// By default, load omni settings
	*this= CPIDControlPhantomOmni();
}