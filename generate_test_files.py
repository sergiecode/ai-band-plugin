#!/usr/bin/env python3
"""
Simple MIDI Test File Generator for AI Band Plugin
Creates basic bass and drum MIDI files for testing the plugin.
"""

import sys
import os
from pathlib import Path

try:
    import mido
except ImportError:
    print("Error: mido library not found.")
    print("Install with: pip install mido")
    sys.exit(1)

def create_bass_track(tempo=120, duration_bars=4):
    """Create a simple bass MIDI track."""
    # Create new MIDI file
    mid = mido.MidiFile()
    track = mido.MidiTrack()
    mid.tracks.append(track)
    
    # Set tempo
    track.append(mido.MetaMessage('set_tempo', tempo=mido.bpm2tempo(tempo)))
    
    # Set time signature (4/4)
    track.append(mido.MetaMessage('time_signature', numerator=4, denominator=4))
    
    # Bass notes (C2, F2, G2, C2 pattern)
    bass_notes = [36, 41, 43, 36]  # C2, F2, G2, C2
    ticks_per_beat = mid.ticks_per_beat
    
    time = 0
    for bar in range(duration_bars):
        for i, note in enumerate(bass_notes):
            # Note on
            track.append(mido.Message('note_on', channel=0, note=note, velocity=80, time=time))
            time = 0  # Reset time for subsequent events
            
            # Note off after one beat
            track.append(mido.Message('note_off', channel=0, note=note, velocity=0, time=ticks_per_beat))
    
    return mid

def create_drum_track(tempo=120, duration_bars=4):
    """Create a simple drum MIDI track."""
    # Create new MIDI file
    mid = mido.MidiFile()
    track = mido.MidiTrack()
    mid.tracks.append(track)
    
    # Set tempo
    track.append(mido.MetaMessage('set_tempo', tempo=mido.bpm2tempo(tempo)))
    
    # Set time signature (4/4)
    track.append(mido.MetaMessage('time_signature', numerator=4, denominator=4))
    
    # Drum mapping (General MIDI standard)
    kick = 36    # Bass Drum 1
    snare = 38   # Acoustic Snare
    hihat = 42   # Closed Hi-Hat
    
    ticks_per_beat = mid.ticks_per_beat
    ticks_per_eighth = ticks_per_beat // 2
    
    # Simple pattern: Kick on 1,3 - Snare on 2,4 - Hi-hat on eighths
    for bar in range(duration_bars):
        time = 0
        
        # Beat 1: Kick + Hi-hat
        track.append(mido.Message('note_on', channel=9, note=kick, velocity=100, time=time))
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=60, time=0))
        track.append(mido.Message('note_off', channel=9, note=kick, velocity=0, time=ticks_per_eighth))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=0))
        
        # Off-beat hi-hat
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=40, time=0))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=ticks_per_eighth))
        
        # Beat 2: Snare + Hi-hat
        track.append(mido.Message('note_on', channel=9, note=snare, velocity=90, time=0))
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=60, time=0))
        track.append(mido.Message('note_off', channel=9, note=snare, velocity=0, time=ticks_per_eighth))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=0))
        
        # Off-beat hi-hat
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=40, time=0))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=ticks_per_eighth))
        
        # Beat 3: Kick + Hi-hat
        track.append(mido.Message('note_on', channel=9, note=kick, velocity=100, time=0))
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=60, time=0))
        track.append(mido.Message('note_off', channel=9, note=kick, velocity=0, time=ticks_per_eighth))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=0))
        
        # Off-beat hi-hat
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=40, time=0))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=ticks_per_eighth))
        
        # Beat 4: Snare + Hi-hat
        track.append(mido.Message('note_on', channel=9, note=snare, velocity=90, time=0))
        track.append(mido.Message('note_on', channel=9, note=hihat, velocity=60, time=0))
        track.append(mido.Message('note_off', channel=9, note=snare, velocity=0, time=ticks_per_eighth))
        track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=0))
        
        # Off-beat hi-hat (except last to avoid overlap)
        if bar < duration_bars - 1:
            track.append(mido.Message('note_on', channel=9, note=hihat, velocity=40, time=0))
            track.append(mido.Message('note_off', channel=9, note=hihat, velocity=0, time=ticks_per_eighth))
        else:
            time = ticks_per_eighth  # Final note timing
    
    return mid

def main():
    """Generate test MIDI files."""
    # Create output directory
    output_dir = Path("test_midi_files")
    output_dir.mkdir(exist_ok=True)
    
    print("Generating test MIDI files...")
    
    # Generate files with different tempos and lengths
    test_configs = [
        {"tempo": 120, "bars": 4, "suffix": "120bpm_4bars"},
        {"tempo": 100, "bars": 8, "suffix": "100bpm_8bars"},
        {"tempo": 140, "bars": 2, "suffix": "140bpm_2bars"},
    ]
    
    for config in test_configs:
        tempo = config["tempo"]
        bars = config["bars"]
        suffix = config["suffix"]
        
        # Generate bass track
        bass_file = output_dir / f"bass_{suffix}.mid"
        bass_midi = create_bass_track(tempo, bars)
        bass_midi.save(bass_file)
        print(f"Created: {bass_file}")
        
        # Generate drum track
        drum_file = output_dir / f"drum_{suffix}.mid"
        drum_midi = create_drum_track(tempo, bars)
        drum_midi.save(drum_file)
        print(f"Created: {drum_file}")
    
    print(f"\nTest files created in: {output_dir.absolute()}")
    print("\nTo test with AI Band Plugin:")
    print("1. Load the plugin in your DAW")
    print("2. Point the plugin to the test_midi_files folder")
    print("3. Load bass and drum files")
    print("4. Press Play to test playback")

if __name__ == "__main__":
    main()
