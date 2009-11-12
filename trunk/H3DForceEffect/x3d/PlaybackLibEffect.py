from H3DInterface import *

playbackEffect = references.getValue()[0];

class KeyboardHandlerField( AutoUpdate( SFString ) ):
  def update( self, event ):
  
    if event.getValue() == 'c':
      playbackEffect.moveToPoint.setValue ( Vec3f ( 0, 0, 0 ) );
      
    if event.getValue() == 'q':
      playbackEffect.cancel.setValue ( True );

    if event.getValue() == 'r':
      playbackEffect.recordTo.setValue ( "haptic_recording.csv" );
      
    if event.getValue() == 'p':
      playbackEffect.playFrom.setValue ( "haptic_recording.csv" );      
      
    return ''

keyboardHandler = KeyboardHandlerField()