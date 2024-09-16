#include <shlobj_core.h>

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    wchar_t szPath[MAX_PATH + 1] = { 0 };

    const HRESULT result_tmp = SHGetFolderPathW(
        nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, szPath);

    int result = MessageBoxW(0, szPath, L"Your current user directory is:", 0);
    return result;
}
