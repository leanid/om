#include "opengles30.hxx"

#ifdef __ANDROID__
#include <android/log.h>

class android_redirected_buf : public std::streambuf
{
public:
    android_redirected_buf() = default;

private:
    // This android_redirected_buf buffer has no buffer. So every character
    // "overflows" and can be put directly into the teed buffers.
    virtual int overflow(int c)
    {
        if (c == EOF)
        {
            return !EOF;
        }
        else
        {
            if (c == '\n')
            {
                // android log function add '\n' on every print itself
                __android_log_print(ANDROID_LOG_ERROR, "OM", "%s",
                                    message.c_str());
                message.clear();
            }
            else
            {
                message.push_back(static_cast<char>(c));
            }
            return c;
        }
    }

    virtual int sync() { return 0; }

    std::string message;
};

struct global_redirect_handler
{
    android_redirected_buf logcat;
    std::streambuf*        clog_buf = nullptr;

    global_redirect_handler()
    {
        using namespace std;

        clog_buf = clog.rdbuf();
        clog.rdbuf(&logcat);
    }
    ~global_redirect_handler()
    {
        using namespace std;
        clog.rdbuf(clog_buf);
    }
} global_var;
#endif // __ANDROID__
