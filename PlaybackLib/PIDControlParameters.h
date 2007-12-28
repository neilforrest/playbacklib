#pragma once

class CPIDControlParameters
{
public:
	CPIDControlParameters(void);
	virtual ~CPIDControlParameters(void);

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
