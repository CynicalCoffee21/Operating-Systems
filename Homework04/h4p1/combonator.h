#ifndef __COMBONATOR_H__
#define __COMBONATOR_H__

#include <stdbool.h>

/** Interface for the jazz combo monitor.  If I was programming in
    C++, this would all be wrapped up in a class.  Since we're using
    C, it's just a collection of functions, with an initialization
    function to init the state of the whole monitor and a destroy
    function to free its resources. */

/** Initialize the monitor. */
void initCombonator();

/** Destroy the monitor, freeing any resources it uses.  When the
    monitor is destroyed, it gvies a summary of how long each type of
    ensemble was playing (either silence, a solo, a duet or a
    trio). */
void destroyCombonator();

/** Called by a thread when it wants to start playing.  If the stage
    is full (already has three musicians) or if there's already
    another thread playing the given instrument, then the calling
    thread will wait in this function until it can take the stage. */
void startPlaying( char const *instrument );

/** Called by a thread when it wants to stop playing and leave the stage. */
void stopPlaying( char const *instrument );

#endif