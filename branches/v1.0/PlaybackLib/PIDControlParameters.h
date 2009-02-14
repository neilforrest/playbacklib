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
		virtual ~CPIDControlParameters(void);

		// Some preset values
		// ..................

		// PHANToM Omni
		void SetDevicePhantomOmni ( );

		// PHANToM Premium
		void SetDevicePhantomPremium ( );

		// ..................

		// Assignment operator
		CPIDControlParameters operator = ( CPIDControlParameters p );

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
}