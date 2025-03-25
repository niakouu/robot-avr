from mido import MidiFile, tick2second
import sys

def main(argv):
    if len(sys.argv) < 2:
        print(f'Usage: {__file__} path/to/song.midi [<TRACK_ID> <path/to/output.txt>]')
        return 1

    mid = MidiFile(sys.argv[1])
    if len(sys.argv) != 4:
        for i, track in enumerate(mid.tracks):
            print('Track {}: {}'.format(i, track.name))
        
        return 0

    track = mid.tracks[int(sys.argv[2])]
    instructions = ['dbt;']

    tempo = 500000
    for t in mid.tracks:
        for m in t:
            if m.type == 'set_tempo':
                tempo = m.tempo
                break

    for msg in track:
        dt = tick2second(msg.time, mid.ticks_per_beat, tempo)
        if msg.type == 'note_on':
            if msg.note < 45:
                print(f'too small! {msg.note}')
            elif msg.note > 81:
                print(f'too big! {msg.note}')

            instructions.append(f'att {int(dt // 0.025)};')
            instructions.append(f'sgo {msg.note};')
        elif msg.type == 'note_off':
            instructions.append(f'att {int(dt // 0.025)};')
            instructions.append('sar;')

    instructions.append('fin;')

    with open(sys.argv[3], 'w') as f:
        f.write('\n'.join(instructions))
    
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))