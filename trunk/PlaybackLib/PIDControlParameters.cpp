#include "StdAfx.h"
#include "PIDControlParameters.h"

using namespace PlaybackLib;

CPIDControlParameters::CPIDControlParameters(void)
{
	// Some default settings
	m_p = 0.3;
	m_i = 0.0;
	m_d = 15.0;
	m_f = 0.09;
	m_out_filter = 0.2;
	m_gain = 0.4;
	m_sat_low = -5.0;
	m_sat_high = 5.0;
	m_dead_zone = 0.0;
}