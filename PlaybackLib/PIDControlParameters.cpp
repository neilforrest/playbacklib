#include "StdAfx.h"
#include "PIDControlParameters.h"

using namespace PlaybackLib;

CPIDControlParameters::CPIDControlParameters(void)
{
	// By default load omni settings
	SetDevicePhantomOmni ( );
}

CPIDControlParameters::~CPIDControlParameters(void)
{
}

// Some preset values
// ..................

// PHANToM Omni
void CPIDControlParameters::SetDevicePhantomOmni ( )
{//
	m_p = 0.3;
	m_i = 0.0;
	m_d = 15.0;
	m_f = 0.09;
	m_out_filter = 0.2;
	m_gain = 0.4;
	m_sat_low = -5.0;
	m_sat_high = 5.0;
	m_dead_zone = 0.0;

	//// PID control parameters
	//m_p = 0.3;
	//m_i = 0.0005;
	//m_d = 20.0;
	//m_f = 0.09;
	//m_out_filter = 0.09;
	//m_gain = 0.2;
	//m_sat_low = -3.0;
	//m_sat_high = 3.0;
	//m_dead_zone = 0.0;
}

// PHANToM Premium
void CPIDControlParameters::SetDevicePhantomPremium ( )
{
	// PID control parameters
	m_p = 0.3;
	m_i = 0.0;
	m_d = 15.0;
	m_f = 0.09;
	m_out_filter = 0.2;
	m_gain = 0.3;
	m_sat_low = -5.0;
	m_sat_high = 5.0;
	m_dead_zone = 0.0;
}

// Assignment operator
CPIDControlParameters CPIDControlParameters::operator = ( CPIDControlParameters p )
{
	m_p= p.m_p;
	m_i= p.m_i;
	m_d= p.m_d;
	m_f= p.m_f;
	m_out_filter= p.m_out_filter;
	m_gain= p.m_gain;
	m_sat_low= p.m_sat_low;
	m_sat_high= p.m_sat_high;
	m_dead_zone= p.m_dead_zone;

	return *this;
}