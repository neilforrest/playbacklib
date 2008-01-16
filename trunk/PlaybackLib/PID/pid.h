/* 

     The contents of this file are subject to the Mozilla Public License
	 Version 1.1 (the "License"); you may not use this file except in
     compliance with the License. You may obtain a copy of the License at
     http://www.mozilla.org/MPL/

     Software distributed under the License is distributed on an "AS IS"
     basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
     License for the specific language governing rights and limitations
     under the License.

     The Original Code is the PID trajectory playback library.

     The Initial Developers of the Original Code are Andrew Crossan (ac@dcs.gla.ac.uk) 
	 and John Williamson (jhw@dcs.gla.ac.uk) from University of Glasgow.
	 
     Portions created by Andrew Crossan and John Williamson are Copyright (C) 2006.
     All Rights Reserved.

     Contributor(s): ______________________________________.

*/


/* Header file for a PID Controller
 *
 * Developed by John Williamson (jhw@dcs.gla.ac.uk) at University of Glasgow
 * 
 */

#pragma once

namespace PlaybackLib
{

	/* Structure for PID controller */
	typedef struct ctrl_state{
	  double x, target_x, last_action, offset;
	  double p, i , d, filter, out_filter, gain, sat_low, sat_high;
	  double cn_p, cn_i, cn_d, cn_f;
	  char *input, *output;
	  float *float_in, *float_out;
	  int saturate;
	  double dead_zone;
	  int soft_saturate; 	// Use soft saturation
	  double soft_saturate_scale;  // x-scale of saturation; higher values mean more step like response
	   double integrator_decay; 	    // Integrator exp. decay values
	  double integrator_max_decay; 	// Integrator exp. decay value after reaching max_integrator
	  double max_integrator; 	    // Threshold at which integrator_max_decay takes effect.
	  double integrator_kill;		// Zone at which integrator deactivated
	  double max_change;		// Max step value in input
	  double input_filter; // Input filtering
	} ctrl_state;

	/* Limits the integrator. 
	 I is multiplied by decay each time step
	When integrator reaches max_decay, 
	it is multiplied by max_decay instead.
	By default, integrator protection is not enabled.
	*/
	void set_integrator_decay(ctrl_state *state, double max, double decay, double max_decay);
	void set_rate_limiter(ctrl_state *state, double max_change);
	void set_integrator_kill(ctrl_state *state, double kill);
	ctrl_state *init_state(double p, double i, double d, double f, double out_filter, double gain, double dead_zone,
							  double sat_low, double sat_high);
	double control(ctrl_state *state, double x, double target);
	void set_soft_saturation(ctrl_state *state, double scale);
	void reset_state(ctrl_state *state);
	void set_input_filter(ctrl_state *state, double f);
	/* Set PID controller DC offset */
	void set_dc(ctrl_state *state, double dc);

	/* Clip a value to a given range */
	double clip(double x, double a, double b);

	// Clean up memory
	void PIDClean (ctrl_state *state);
}