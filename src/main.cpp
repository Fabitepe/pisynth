#include <iostream>
#include <cmath>
#include "miniaudio.h"

#include <fcntl.h>
#include <unistd.h>
#define MIDI_INPUT


#ifndef MIDI_INPUT
#include <linux/input.h>
#endif



#include "banner.h"
#include "noteoscillator.h"
#include "instrument.h"
#include "notes.h"


const int SAMPLE_RATE = 48000;
const int CHANNELS = 2;
const ma_format SAMPLE_FORMAT = ma_format::ma_format_s16;


ma_device device;

ma_context_config context_config;
ma_context context;

NoteOscillator osc1(Oscillator::Waveform::Sine, SAMPLE_RATE, 0.5f, 400.0f, 0.2f);
NoteOscillator osc2(Oscillator::Waveform::Saw, SAMPLE_RATE, 0.5f, 400.0f, 0.2f);
NoteOscillator osc3(Oscillator::Waveform::Saw, SAMPLE_RATE, 5.5f, 100.0f, 0.0f);

Instrument inst;


#ifdef MIDI_INPUT
#include "RtMidi.h"

void midiCallback(double deltatime, std::vector<unsigned char>* message, void *userData) {
    if(!message->empty())
    {
        if(message->size() != 3)
        {
            return;
        }

        bool pressed = message->at(2) == 80;
        if(!pressed)
        {
            inst.endNote();
            return;
        }

        int key = message->at(1);
        Note note = static_cast<Note>(key % 12);
        int octave = key / 12;

        inst.triggerNote(note, octave);
    }
}

#endif

float master_vol = 1.0f;


void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    int16_t* output = (int16_t*) pOutput;
    for (ma_uint32 i = 0; i < frameCount; i++) {
        float sample = ((inst.nextSample()) * 32767.0f) * master_vol;

        int16_t rounded_sample = (int16_t) sample;

        // Stereo output (duplicate sample)
        for (int ch = 0; ch < CHANNELS; ch++) {
            *output++ = rounded_sample;
        }
    }
}


int main() {

    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        std::cerr << "could not get context" << std::endl;
    }

    ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;
    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
        std::cerr << "could not get context devices" << std::endl;
    }

    for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
        printf("%d - %s\n", iDevice, pPlaybackInfos[iDevice].name);
    }

    ma_context_uninit(&context);

    int chosenPlaybackDeviceIndex = 14;

    ma_device_config config   = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = &pPlaybackInfos[chosenPlaybackDeviceIndex].id;
    config.playback.format    = SAMPLE_FORMAT;
    config.playback.channels  = CHANNELS;
    config.sampleRate         = SAMPLE_RATE;
    config.dataCallback       = data_callback;

    ma_result result = ma_device_init(NULL, &config, &device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback device: " << result << std::endl;
        return 1;
    }

    std::cout << "Using device sample rate: " << device.sampleRate;
    std::cout << " and format " << device.playback.format;
    std::cout << " on " << device.playback.channels << " channels" << std::endl;

    inst.addOscillator(&osc1, 0);
    inst.addOscillator(&osc2, 0, 0.2);
    inst.addOscillator(&osc3, -1, 0.2);


    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        return 1;
    }

    std::cout << BANNER << std::endl;


#ifndef MIDI_INPUT

    const char *inputDevice = "/dev/input/event7"; // Change event2 to your keyboard event file
    int fd = open(inputDevice, O_RDONLY);
    if (fd < 0) {
        std::cerr << "Failed to open device: " << inputDevice << std::endl;
        return 1;
    }

    struct input_event ev;
    while (read(fd, &ev, sizeof(struct input_event)) > 0) {
        if (ev.type != EV_KEY) {
            continue;
        }
        std::cout << "Key " << ev.code 
            << " " << (ev.value ? "Pressed" : "Released") 
                    << std::endl;
        if(!ev.value)
        {
            inst.endNote();
            continue;
        }

        Note note;
        switch(ev.code)
        {
        case 44:
            note = Note::B;
            break;
        case 45:
            note = Note::C;
            break;
        case 46:
            note = Note::D;
            break;
        case 47:
            note = Note::E;
            break;
        case 48:
            note = Note::E;
            break;
        case 49:
            note = Note::F;
            break;
        case 50:
            note = Note::G;
            break;
        default:
            continue;
        }
        inst.triggerNote(note, 3);
    }
#endif

#ifdef MIDI_INPUT

    try {
        RtMidiIn midiIn;

        unsigned int numPorts = midiIn.getPortCount();
        if (numPorts == 0) {
            std::cout << "No MIDI input devices found!" << std::endl;
            return 1;
        }

        std::cout << "Available MIDI Ports:\n";
        for (unsigned int i = 0; i < numPorts; i++) {
            std::cout << i << ": " << midiIn.getPortName(i) << std::endl;
        }

        // Open first available MIDI port
        midiIn.openPort(1);
        midiIn.setCallback(&midiCallback);
        midiIn.ignoreTypes(false, false, false);

        std::cout << "Listening for MIDI input. Press Ctrl+C to quit." << std::endl;

        // Keep the program running
        while (true) {
            usleep(100000); // Sleep for 100ms
        }
    }
    catch (RtMidiError &error) {
        error.printMessage();
        return 1;
    }


#endif

    ma_device_uninit(&device);
    return 0;
}
