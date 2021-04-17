#include <algorithm>
#include <csignal>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>

static volatile std::sig_atomic_t         g_sig_counters[16] = { 0 };
static std::unique_ptr<std::stringstream> g_statistics;

extern "C" void custom_signal_handler(int signal_index);

void print_signal_statistics(std::string_view prefix);
int  select_action_or_signal();
int  user_select_signal_or_action(int user_selection);
void experiment_with_signal(int user_select_signal);

int main()
{
    using namespace std;

    g_statistics = make_unique<stringstream>();

    cout << "start" << endl;

    print_signal_statistics("during start");

    signal(SIGSEGV, SIG_IGN);

    int* bad_ptr = reinterpret_cast<int*>(0xBAD);
    // here HW(processor) will signal OS about illegal address access
    // then OS will generate signal for glibc(or std lib "C")
    // and finally we got signal to our handler (and do nothing)
    // Our signal handler do not terminate program, so
    // standard library just go to next instruction
    cout << "just before bad pointer access" << endl;
    int value = *bad_ptr;
    cout << "right after bad pointer access" << endl;

    cout << "value from *bad_ptr " << value << endl;

    bool continue_test{ true };

    while (continue_test)
    {
        int user_select_signal{ select_action_or_signal() };

        if (user_select_signal > 6)
        {
            continue_test = false;
        }
        else
        {
            int sig_index = user_select_signal_or_action(user_select_signal);
            experiment_with_signal(sig_index);
        }
        cout << "finish test" << endl;
    }

    print_signal_statistics("before exit");

    cout << g_statistics->str();

    return 0;
}

void experiment_with_signal(int user_select_signal)
{
    using namespace std;
    cout << "Do you want to test default or custom signal handler:\n"
         << "1: default\n"
         << "2: custom\n"
         << ">";

    int handler_type;
    cin >> handler_type;
    cout << endl;

    if (handler_type == 2)
    {
        auto prev_handler =
            std::signal(user_select_signal, custom_signal_handler);
        if (prev_handler == SIG_ERR)
        {
            throw std::runtime_error("error: install custom handler failed");
        }
        cout << "custom signal handler installed" << endl;
    }
    else
    {
        std::signal(user_select_signal, SIG_DFL);
    }

    cout << "rising signal!" << endl;

    std::raise(user_select_signal);

    cout << "after rising signal" << endl;
}

int user_select_signal_or_action(int user_selection)
{
    int user_select_signal{};

    switch (user_selection)
    {
        case 1:
            user_select_signal = SIGABRT;
            break;
        case 2:
            user_select_signal = SIGFPE;
            break;
        case 3:
            user_select_signal = SIGILL;
            break;
        case 4:
            user_select_signal = SIGINT;
            break;
        case 5:
            user_select_signal = SIGSEGV;
            break;
        case 6:
            user_select_signal = SIGTERM;
            break;
        default:
            break;
    }
    return user_select_signal;
}

int select_action_or_signal()
{
    std::cout << "Select signal to test:\n"
              << "1: SIGABRT\n"
              << "2: SIGFPE\n"
              << "3: SIGILL\n"
              << "4: SIGINT\n"
              << "5: SIGSEGV\n"
              << "6: SIGTERM\n"
              << ">";

    int selection;
    std::cin >> selection;
    return selection;
}

void print_signal_statistics(std::string_view prefix)
{
    using namespace std;

    *g_statistics << "statistics for: " << prefix << endl;

    auto print_counter = [](int         signal_index,
                            string_view name) -> pair<string_view, size_t> {
        auto num_of_triggers =
            static_cast<size_t>(g_sig_counters[signal_index]);
        return make_pair(name, num_of_triggers);
    };

    auto print_stream = [](ostream&                  out,
                           pair<string_view, size_t> p) -> ostream& {
        out << p.first << ": " << p.second << std::endl;
        return out;
    };

    int32_t     signals[]{ SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM };
    string_view names[]{ "SIGABRT", "SIGFPE",  "SIGILL",
                         "SIGINT",  "SIGSEGV", "SIGTERM" };

    ostream& ref_to_stream = *g_statistics;

    [[maybe_unused]] ostream& out = inner_product(begin(signals),
                                                  end(signals),
                                                  begin(names),
                                                  std::ref(ref_to_stream),
                                                  print_stream,
                                                  print_counter);
}

extern "C" void custom_signal_handler(int signal_index)
{
    if (signal_index < 16)
    {
        g_sig_counters[signal_index]++;
    }
}
