#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include <SDL3/SDL.h>

constexpr int32_t AUDIO_FORMAT = AUDIO_S16LSB;

static void audio_callback(void* userdata, uint8_t* stream, int len);

#pragma pack(push, 1)
struct audio_buff
{
    uint8_t* start       = nullptr;
    size_t   size        = 0;
    size_t   current_pos = 0;

    struct
    {
        size_t frequncy = 0;
        double time     = 0.0;
        bool   use_note = false;
    } note;
};
#pragma pack(pop)

std::ostream& operator<<(std::ostream& o, const SDL_AudioSpec& spec);

static auto     start         = std::chrono::high_resolution_clock::now();
static auto     finish        = start;
static uint32_t default_delay = 0;

enum class play_channel
{
    left,
    right,
    left_and_right
};

static play_channel channels{ play_channel::left_and_right };

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    clog << "start sdl init" << endl;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        cerr << "error: can't init audio: " << SDL_GetError();
        return EXIT_FAILURE;
    }

    const char* file_name =
        "01-basic-game-dev/07-1-sdl-sound-example/highlands.wav";

    clog << "read file: " << file_name << endl;

    SDL_RWops* file = SDL_RWFromFile(file_name, "rb");
    if (file == nullptr)
    {
        cerr << "error: can't open file: " << file_name << "\n";
        return EXIT_FAILURE;
    }

    SDL_AudioSpec audio_spec_from_file{};
    const int32_t auto_delete_file            = 1;
    uint8_t*      sample_buffer_from_file     = nullptr;
    uint32_t      sample_buffer_len_from_file = 0;

    clog << "loading sample buffer from file: " << file_name << endl;

    SDL_AudioSpec* audio_spec = SDL_LoadWAV_RW(file,
                                               auto_delete_file,
                                               &audio_spec_from_file,
                                               &sample_buffer_from_file,
                                               &sample_buffer_len_from_file);

    if (audio_spec == nullptr)
    {
        cerr << "error: can't parse and load audio samples from file\n";
        return EXIT_FAILURE;
    }

    clog << "loaded file audio spec:\n" << audio_spec_from_file << endl;

    // clang-format off
    audio_buff loaded_audio_buff
    {
        .start = sample_buffer_from_file,
        .size = sample_buffer_len_from_file,
        .current_pos = 0,
        .note = {
         .frequncy = 0,
         .time = 0,
         .use_note = false
        }
    };
    // clang-format on

    clog << "audio buffer from file size: " << sample_buffer_len_from_file
         << " B (" << sample_buffer_len_from_file / double(1024 * 1024)
         << ") MB" << endl;

    const char*   device_name       = nullptr; // device name or nullptr
    const int32_t is_capture_device = 0; // 0 - play device, 1 - microphone
    SDL_AudioSpec disired{ .freq     = 48000,
                           .format   = AUDIO_FORMAT,
                           .channels = 2, // stereo
                           .silence  = 0,
                           .samples  = 4096, // must be power of 2
                           .padding  = 0,
                           .size     = 0,
                           .callback = audio_callback,
                           .userdata = &loaded_audio_buff };

    clog << "prepare disired audio specs for output device:\n"
         << disired << endl;

    SDL_AudioSpec returned{};

    const int32_t allow_changes = 0;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        device_name, is_capture_device, &disired, &returned, allow_changes);
    if (audio_device_id == 0)
    {
        cerr << "error: failed to open audio device: " << SDL_GetError()
             << std::endl;
        return EXIT_FAILURE;
    }

    clog << "returned audio spec for output device:\n" << returned << endl;

    if (disired.format != returned.format ||
        disired.channels != returned.channels || disired.freq != returned.freq)
    {
        cerr << "error: disired != returned audio device settings!";
        return EXIT_FAILURE;
    }

    // start playing audio thread
    // now callback is firing
    SDL_PlayAudioDevice(audio_device_id);

    clog << "unpause audio device (start audio thread)" << endl;

    bool continue_loop = true;
    while (continue_loop)
    {
        clog << "1. stop and exit\n"
             << "2. print current music position\n"
             << "3. print music time (length)\n"
             << "4. print device buffer play length\n"
             << "5. experimental device buffer play length(time between "
                "callbacks)\n"
             << "6. set default delay for audio callback(current val: "
             << default_delay << " ms)\n"
             << "7. play note(current val: " << loaded_audio_buff.note.frequncy
             << ")\n"
             << "8. play note on channel (0 - left, 1 - right, 2 "
                "left_and_right)\n"
             << ">" << flush;

        int choise = 0;
        cin >> choise;
        switch (choise)
        {
            case 1:
                continue_loop = false;
                break;
            case 2:
                clog << "current music pos: "
                     << loaded_audio_buff.current_pos /
                            double(loaded_audio_buff.size) * 100
                     << " %" << endl;
                break;
            case 3:
            {
                size_t format_size =
                    audio_spec_from_file.format == AUDIO_FORMAT ? 2 : 0;

                double time_in_minutes = double(loaded_audio_buff.size) /
                                         audio_spec_from_file.channels /
                                         format_size /
                                         audio_spec_from_file.freq / 60;

                double minutes;
                double rest_minute = modf(time_in_minutes, &minutes);
                double seconds = rest_minute * 60; // 60 seconds in one minute
                clog << "music length: " << time_in_minutes << " minutes or ("
                     << minutes << ":" << seconds << ")" << endl;
                break;
            }
            case 4:
            {
                clog << "device buffer play length: "
                     << returned.samples / double(returned.freq) << " seconds"
                     << endl;
                break;
            }
            case 5:
            {
                double elapsed_seconds =
                    std::chrono::duration_cast<std::chrono::duration<double>>(
                        finish - start)
                        .count();
                clog << "time between last two audio_callbacks: "
                     << elapsed_seconds << " seconds" << endl;
                break;
            }
            case 6:
            {
                clog << "input delay in milliseconds:>" << flush;
                cin >> default_delay;
                break;
            }
            case 7:
            {
                clog << "input note frequncy: " << flush;
                cin >> loaded_audio_buff.note.frequncy;
            }
            break;
            case 8:
            {
                clog << "input play_channel: " << flush;
                int i{};
                cin >> i;
                channels = static_cast<play_channel>(i);
            }
            default:
                break;
        }
    }

    clog << "pause audio device (stop audio thread)" << endl;
    // stop audio device and stop thread call our callback function
    SDL_PauseAudioDevice(audio_device_id);

    clog << "close audio device" << endl;

    SDL_CloseAudioDevice(audio_device_id);

    SDL_free(loaded_audio_buff.start);

    SDL_Quit();

    return 0;
}

static void audio_callback(void* userdata, uint8_t* stream, int len)
{
    static bool first_time = true;
    if (first_time)
    {
        std::clog << "start audio_callback!" << std::endl;
        first_time = false;
    }

    // simulate long calculation (just test)
    SDL_Delay(default_delay);

    // experimental check two continius callback delta time
    start  = finish;
    finish = std::chrono::high_resolution_clock::now();

    size_t stream_len = static_cast<size_t>(len);
    // silence
    std::memset(stream, 0, static_cast<size_t>(len));

    audio_buff* audio_buff_data = reinterpret_cast<audio_buff*>(userdata);
    assert(audio_buff_data != nullptr);

    if (audio_buff_data->note.frequncy != 0)
    {
        size_t num_samples = stream_len / 2 / 2;
        double dt          = 1.0 / 48000.0;

        int16_t* output = reinterpret_cast<int16_t*>(stream);

        for (size_t sample = 0; sample < num_samples; ++sample)
        {
            double omega_t = audio_buff_data->note.time * 2.0 * 3.1415926 *
                             audio_buff_data->note.frequncy;
            double curr_sample =
                std::numeric_limits<int16_t>::max() * sin(omega_t);
            int16_t curr_val = static_cast<int16_t>(curr_sample);

            switch (channels)
            {
                case play_channel::left_and_right:
                {
                    *output = curr_val;
                    output++;
                    *output = curr_val;
                    output++;
                    break;
                }
                case play_channel::left:
                {
                    *output = curr_val;
                    output++;
                    *output = 0;
                    output++;
                    break;
                }
                case play_channel::right:
                {
                    *output = 0;
                    output++;
                    *output = curr_val;
                    output++;
                    break;
                }
            };

            audio_buff_data->note.time += dt;
        }
    }
    else
    {
        while (stream_len > 0)
        {
            // copy data from loaded buffer into output stream
            const uint8_t* current_sound_pos =
                audio_buff_data->start + audio_buff_data->current_pos;

            const size_t left_in_buffer =
                audio_buff_data->size - audio_buff_data->current_pos;

            if (left_in_buffer > stream_len)
            {
                SDL_MixAudioFormat(
                    stream, current_sound_pos, AUDIO_FORMAT, len, 128);
                audio_buff_data->current_pos += stream_len;
                break;
            }
            else
            {
                // first copy rest from buffer and repeat sound from begining
                SDL_MixAudioFormat(stream,
                                   current_sound_pos,
                                   AUDIO_FORMAT,
                                   left_in_buffer,
                                   128);
                // start buffer from begining
                audio_buff_data->current_pos = 0;
                stream_len -= left_in_buffer;
            }
        }
    }
}

std::ostream& operator<<(std::ostream& o, const SDL_AudioSpec& spec)
{
    std::string tab(4, ' ');
    o << tab << "freq: " << spec.freq << '\n'
      << tab << "format: " << std::hex << spec.format << '\n'
      << tab << "channels: " << std::dec << int(spec.channels) << '\n'
      << tab << "silence: " << int(spec.silence) << '\n'
      << tab << "samples: " << spec.samples << '\n'
      << tab << "size: " << spec.size << '\n'
      << tab << "callback: " << reinterpret_cast<const void*>(spec.callback)
      << '\n'
      << tab << "userdata: " << spec.userdata;
    return o;
}
