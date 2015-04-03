# Haptic Playback Library #

This is a re-working of the [Playback Library](http://www.dcs.gla.ac.uk/~ac/pidlib.html) developed by Andrew Crossan and John Williamson at the University of Glasgow. It facilitates the recording and playback of trajectories via a haptic device.

### Please use the links on the right ('Featured Downloads') to download the latest release library, example application and documentation ###
Alternately, the development verson can be checked-out via svn.

## Update ##
22 May 09:
  * Added zip file containing latest version of library (v0.3) and example application

27 Jan 08:
  * New Application Programmers Guide added. [(Download)](http://playbacklib.googlecode.com/files/PlaybackLibraryDoc.pdf)

## Background ##
This specific incarnation of the library adds, amongst other things, the following functionality:


  * **B-Spline Interpolation**

> Recorded sample points are used as control points in an interpolating B-Spline curve that is used for playback. This means much fewer sample points are needed to reproduce a smooth playback trajectory.


  * **Threading Support**

> The library takes on the responsibility of synchronising data between a client and the haptic rendering thread that was previously up to the application. Operations can be added to the processing queue and their status monitored from the client thread.