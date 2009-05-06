#pragma once

#ifdef PB_IMPORT
#define PB_API __declspec(dllimport)
#else
#define PB_API __declspec(dllexport)
#endif

namespace PlaybackLib
{

	class PB_API CPIDControlParameters
	{
	public:
		CPIDControlParameters(void);

		double m_p;
		double m_i;
		double m_d;
		double m_f;
		double m_out_filter;
		double m_gain;
		double m_sat_low;
		double m_sat_high;
		double m_dead_zone;

	};

	// Some preset values
	// ..................

	// PHANToM Omni
	class PB_API CPIDControlPhantomOmni : public CPIDControlParameters
	{
	public:
		CPIDControlPhantomOmni ()
		{
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
	};

	// PHANToM Premium
	class PB_API CPIDControlPhantomPremium : public CPIDControlParameters
	{
	public:
		CPIDControlPhantomPremium ()
		{
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
	};

	// Novint Falcon
	class PB_API CPIDControlFalcon : public CPIDControlParameters
	{
	public:
		CPIDControlFalcon ()
		{
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
	};
}