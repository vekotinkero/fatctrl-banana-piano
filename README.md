# Banana Piano

I wanted to make a simple MIDI controller and then I found about [MakeyMakey](http://makeymakey.com/). I thought the touch sensing keys was fun and I tried to replicate that. 

First, I did a version with a transistor working as a switch. I wasn't happy with the noise my circuit produced so I tried to filter some of it off with components. Eventually I decided to take care of noise in the code. 

Soon I discovered that MakeyMakey had a pretty nice noise filtering code. Well, I'll be damned, but I just copied that and used it for my own filthy purposes.

## My controller

Banana Piano uses nine resistive touch sensors that can be hooked basically to anything conductive with a banana cables and alligator clips. Then there's the clanky ground cable that's used to finish the circuit. The cable is connected to your wrist with a cheap Chinese grounding bracelet. So the idea is pretty much the same as in MakeyMakey.

I even use an [Arduino Leonardo](http://arduino.cc/en/Main/arduinoBoardLeonardo) board, where MIDI and HID applications are made easy. I can recommend that for everybody who wants to make something quickly and easily that helps living things interact with computers.

In my controller there's also one potentiometer for Control Changes and a so-called scale selector for selecting different pre-set scales. And obviously the octave buttons as well. The scale selector is handy thing to have, because you can change your controller setup from chromatic drum machine to a lovely melancholic E minor scale and play some tunes. 

It's even possible to add few lines of code and assign the pre-set scales with different MIDI channels. Then you'd be able to change from the drum to piano on the fly.

When the code and the circuits were ready I installed them to an old suitcase. Then I made holes into the front to get the cables out and some tweaks here and there to complete my fancy nagic suitcase.

I've tested the design on a gig and in few parties. It's a killer. It's magic!
