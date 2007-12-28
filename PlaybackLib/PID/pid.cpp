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

#include "stdafx.h"
#include "pid.h"
#include <math.h>
#include <stdlib.h>


/* Initialize a new PID controller 

p: proportional control gain
i: integral control gain
d: derivative control gain
f: input filter (for deriv. estimation)
out_filter: output filtering
gain: overall gain
sat_low: minimum saturation point
sat_high: maximum_saturation point
*/
control_state *init_state(double p, double i, double d, double f, 
						  double out_filter, double gain, 
						  double dead_zone, double sat_low, double sat_high)
{
  control_state *state;
  state = (control_state *)calloc(sizeof(*state), 1);
  state->last_action = 0;
  state->p = state->i = state->d = state->filter = 0;
    state->soft_saturate = 0;
  state->soft_saturate_scale = 1.0;
  state->x = state->target_x = 0;
  state->sat_low = sat_low;
  state->sat_high = sat_high;
  state->gain = gain;
  state->cn_p = p;  
  state->cn_i = i; 
  state->cn_d = d;  
  state->cn_f = f; 
  state->offset = 0;
  state->dead_zone = dead_zone;
  state->out_filter = out_filter;
  state->input = NULL;
  state->output = NULL;
  state->float_in = 0;
  state->float_out = 0;
  state->saturate = 0;
  state->max_integrator = 0.0;
  state->integrator_decay = 1.0;
  state->integrator_max_decay = 1.0;
  state->max_change = 0;
  state->integrator_kill = 0;
  state->input_filter = 1.0;
  return state;
}


void set_integrator_kill(control_state *state, double kill)
{

		state->integrator_kill = kill;
}


void set_input_filter(control_state *state, double f)
{

	state->input_filter = f;
}

/* Limit the change in control values to max_change */
void set_rate_limiter(control_state *state, double max_change)
{
	state->max_change = max_change;

}

/* Set PID controller DC offset */
void set_dc(control_state *state, double dc)
{
        state->offset = dc;
}

/* Clip a value to a given range */
double clip(double x, double a, double b)
{
        if(x<a)
                x=a;
        if(x>b)
                x=b;
        return x;
}

/* Limits the integrator. 
I is multiplied by decay each time step
When integrator reaches max_decay, 
it is multiplied by max_decay instead
*/
void set_integrator_decay(control_state *state, double max, double decay, double max_decay)
{
	state->max_integrator = max;
	state->integrator_decay = decay;
	state->integrator_max_decay = max_decay;
}

/* Do PID control */
double control(control_state *state, double x, double target)
{
  double  filter, action;


  
  state->x = x;
  state->target_x = state->input_filter * target  + (1-state->input_filter) * state->target_x;


  if(fabs(x-target) > state->dead_zone)
  {
	state->p = state->target_x - state->x;

	if(state->max_change>0.0)
	{
	if(state->p > state->max_change)
		state->p = state->max_change;
	if(state->p < -state->max_change)
		state->p = -state->max_change;
	}

	filter = state->cn_f * state->p + (1-state->cn_f) * state->filter;

	if(fabs(state->p) < state->integrator_kill)
		state->i = 0;

	state->i += state->p;

	state->i *= (state->i>state->max_integrator) ? state->integrator_max_decay : state->integrator_decay;

	state->d = filter - state->filter;
	state->filter = filter;
	action = state->p * state->cn_p + state->cn_i * state->i + state->d * state->cn_d;
	action *= state->gain;



	action = (state->out_filter * action) + (1-state->out_filter) * state->last_action;
	action += state->offset;

	/* Apply soft limiting */
	if(state->soft_saturate)
		action = ((1.0/(1.0+exp(-action * state->soft_saturate_scale))) ) * 
					(state->sat_high - state->sat_low) + state->sat_low;

	

	action = clip(action, state->sat_low, state->sat_high);


	if(action==state->sat_low || action==state->sat_high)
		  state->saturate = 1;
	else
		  state->saturate = 0;

	state->last_action = action;
  }
  else
  {
		action = state->last_action;
  }
   return action;
}



/* Enable soft saturation with given scale; disables if scale = 0 */
void set_soft_saturation(control_state *state, double scale)
{
	if(scale==0)
		state->soft_saturate = 0;
	else
		{
			state->soft_saturate = 1;
			state->soft_saturate_scale = scale;
		}
}


void reset_state(control_state *state)
{
	state->i = 0;
	state->d = 0;
	state->p = 0;
	state->last_action = 0;
	state->filter = 0;
	//state->input_filter = 0;
}

// Clean up memory
void PIDClean (control_state *state)
{
	free ( state );
}