
module;

#include <SDL3/SDL.h>
#include <type_traits>

export module sdl.SDL;

#define REGULAR_ENUM(ty)                                                       \
    constexpr bool operator==(std::underlying_type_t<ty> a, ty b) noexcept     \
    {                                                                          \
        return a == static_cast<std::underlying_type_t<ty>>(b);                \
    }                                                                          \
    constexpr bool operator==(ty a, std::underlying_type_t<ty> b) noexcept     \
    {                                                                          \
        return static_cast<std::underlying_type_t<ty>>(a) == b;                \
    }

#define BITFLAG_ENUM(ty)                                                       \
    constexpr ty operator|(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) |    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator&(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) &    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator^(ty a, ty b) noexcept                                \
    {                                                                          \
        return static_cast<ty>(static_cast<std::underlying_type_t<ty>>(a) ^    \
                               static_cast<std::underlying_type_t<ty>>(b));    \
    }                                                                          \
    constexpr ty operator~(ty a) noexcept                                      \
    {                                                                          \
        return static_cast<ty>(~static_cast<std::underlying_type_t<ty>>(a));   \
    }                                                                          \
    constexpr ty& operator|=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a | b;                                                      \
    }                                                                          \
    constexpr ty& operator&=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a & b;                                                      \
    }                                                                          \
    constexpr ty& operator^=(ty& a, ty b) noexcept                             \
    {                                                                          \
        return a = a ^ b;                                                      \
    }

export namespace sdl
{

constexpr auto FUNCTION()
{
    return "???";
}

constexpr auto NULL_WHILE_LOOP_CONDITION()
{
    return (0);
}

enum class AssertState
{
    ASSERTION_RETRY         = SDL_ASSERTION_RETRY,
    ASSERTION_BREAK         = SDL_ASSERTION_BREAK,
    ASSERTION_ABORT         = SDL_ASSERTION_ABORT,
    ASSERTION_IGNORE        = SDL_ASSERTION_IGNORE,
    ASSERTION_ALWAYS_IGNORE = SDL_ASSERTION_ALWAYS_IGNORE,
};
REGULAR_ENUM(AssertState);

using AssertData = SDL_AssertData;

SDL_AssertState ReportAssertion(SDL_AssertData* data,
                                const char*     func,
                                const char*     file,
                                int             line)
{
    return SDL_ReportAssertion(data, func, file, line);
}

void SetAssertionHandler(SDL_AssertionHandler handler, void* userdata)
{
    SDL_SetAssertionHandler(handler, userdata);
}

SDL_AssertionHandler GetDefaultAssertionHandler(void)
{
    return SDL_GetDefaultAssertionHandler();
}

SDL_AssertionHandler GetAssertionHandler(void** puserdata)
{
    return SDL_GetAssertionHandler(puserdata);
}

const SDL_AssertData* GetAssertionReport(void)
{
    return SDL_GetAssertionReport();
}

void ResetAssertionReport(void)
{
    SDL_ResetAssertionReport();
}

using SDL_AsyncIO = SDL_AsyncIO;

enum class AsyncIOTaskType
{
    TASK_READ  = SDL_ASYNCIO_TASK_READ,
    TASK_WRITE = SDL_ASYNCIO_TASK_WRITE,
    TASK_CLOSE = SDL_ASYNCIO_TASK_CLOSE,
};
REGULAR_ENUM(AsyncIOTaskType);

enum class AsyncIOResult
{
    COMPLETE = SDL_ASYNCIO_COMPLETE,
    FAILURE  = SDL_ASYNCIO_FAILURE,
    CANCELED = SDL_ASYNCIO_CANCELED,
};
REGULAR_ENUM(AsyncIOResult);

using AsyncIOOutcome = SDL_AsyncIOOutcome;

using SDL_AsyncIOQueue = SDL_AsyncIOQueue;

SDL_AsyncIO* AsyncIOFromFile(const char* file, const char* mode)
{
    return SDL_AsyncIOFromFile(file, mode);
}

Sint64 GetAsyncIOSize(SDL_AsyncIO* asyncio)
{
    return SDL_GetAsyncIOSize(asyncio);
}

bool ReadAsyncIO(SDL_AsyncIO*      asyncio,
                 void*             ptr,
                 Uint64            offset,
                 Uint64            size,
                 SDL_AsyncIOQueue* queue,
                 void*             userdata)
{
    return SDL_ReadAsyncIO(asyncio, ptr, offset, size, queue, userdata);
}

bool WriteAsyncIO(SDL_AsyncIO*      asyncio,
                  void*             ptr,
                  Uint64            offset,
                  Uint64            size,
                  SDL_AsyncIOQueue* queue,
                  void*             userdata)
{
    return SDL_WriteAsyncIO(asyncio, ptr, offset, size, queue, userdata);
}

bool CloseAsyncIO(SDL_AsyncIO*      asyncio,
                  bool              flush,
                  SDL_AsyncIOQueue* queue,
                  void*             userdata)
{
    return SDL_CloseAsyncIO(asyncio, flush, queue, userdata);
}

SDL_AsyncIOQueue* CreateAsyncIOQueue(void)
{
    return SDL_CreateAsyncIOQueue();
}

void DestroyAsyncIOQueue(SDL_AsyncIOQueue* queue)
{
    SDL_DestroyAsyncIOQueue(queue);
}

bool GetAsyncIOResult(SDL_AsyncIOQueue* queue, SDL_AsyncIOOutcome* outcome)
{
    return SDL_GetAsyncIOResult(queue, outcome);
}

bool WaitAsyncIOResult(SDL_AsyncIOQueue*   queue,
                       SDL_AsyncIOOutcome* outcome,
                       Sint32              timeoutMS)
{
    return SDL_WaitAsyncIOResult(queue, outcome, timeoutMS);
}

void SignalAsyncIOQueue(SDL_AsyncIOQueue* queue)
{
    SDL_SignalAsyncIOQueue(queue);
}

bool LoadFileAsync(const char* file, SDL_AsyncIOQueue* queue, void* userdata)
{
    return SDL_LoadFileAsync(file, queue, userdata);
}

using SpinLock = int;

bool TryLockSpinlock(SDL_SpinLock* lock)
{
    return SDL_TryLockSpinlock(lock);
}

void LockSpinlock(SDL_SpinLock* lock)
{
    SDL_LockSpinlock(lock);
}

void UnlockSpinlock(SDL_SpinLock* lock)
{
    SDL_UnlockSpinlock(lock);
}

void CompilerBarrier(void)
{
    SDL_CompilerBarrier();
}

void MemoryBarrierReleaseFunction(void)
{
    SDL_MemoryBarrierReleaseFunction();
}

void MemoryBarrierAcquireFunction(void)
{
    SDL_MemoryBarrierAcquireFunction();
}

void CPUPauseInstruction(void)
{
    SDL_CPUPauseInstruction();
}

using AtomicInt = SDL_AtomicInt;

bool CompareAndSwapAtomicInt(SDL_AtomicInt* a, int oldval, int newval)
{
    return SDL_CompareAndSwapAtomicInt(a, oldval, newval);
}

int SetAtomicInt(SDL_AtomicInt* a, int v)
{
    return SDL_SetAtomicInt(a, v);
}

int GetAtomicInt(SDL_AtomicInt* a)
{
    return SDL_GetAtomicInt(a);
}

int AddAtomicInt(SDL_AtomicInt* a, int v)
{
    return SDL_AddAtomicInt(a, v);
}

using AtomicU32 = SDL_AtomicU32;

bool CompareAndSwapAtomicU32(SDL_AtomicU32* a, Uint32 oldval, Uint32 newval)
{
    return SDL_CompareAndSwapAtomicU32(a, oldval, newval);
}

Uint32 SetAtomicU32(SDL_AtomicU32* a, Uint32 v)
{
    return SDL_SetAtomicU32(a, v);
}

Uint32 GetAtomicU32(SDL_AtomicU32* a)
{
    return SDL_GetAtomicU32(a);
}

bool CompareAndSwapAtomicPointer(void** a, void* oldval, void* newval)
{
    return SDL_CompareAndSwapAtomicPointer(a, oldval, newval);
}

void* SetAtomicPointer(void** a, void* v)
{
    return SDL_SetAtomicPointer(a, v);
}

void* GetAtomicPointer(void** a)
{
    return SDL_GetAtomicPointer(a);
}

constexpr auto LIL_ENDIAN()
{
    return 1234;
}

// constexpr auto BIG_ENDIAN()
// {
//     return 4321;
// }

constexpr auto BUILTIN_BSWAP16()
{
    return 0;
}

constexpr auto BUILTIN_BSWAP32()
{
    return 0;
}

constexpr auto BUILTIN_BSWAP64()
{
    return 0;
}

constexpr auto BROKEN_BSWAP()
{
    return 0;
}

bool OutOfMemory(void)
{
    return SDL_OutOfMemory();
}

const char* GetError(void)
{
    return SDL_GetError();
}

bool ClearError(void)
{
    return SDL_ClearError();
}

using PropertiesID = Uint32;

enum class PropertyType
{
    TYPE_INVALID = SDL_PROPERTY_TYPE_INVALID,
    TYPE_POINTER = SDL_PROPERTY_TYPE_POINTER,
    TYPE_STRING  = SDL_PROPERTY_TYPE_STRING,
    TYPE_NUMBER  = SDL_PROPERTY_TYPE_NUMBER,
    TYPE_FLOAT   = SDL_PROPERTY_TYPE_FLOAT,
    TYPE_BOOLEAN = SDL_PROPERTY_TYPE_BOOLEAN,
};
REGULAR_ENUM(PropertyType);

SDL_PropertiesID GetGlobalProperties(void)
{
    return SDL_GetGlobalProperties();
}

SDL_PropertiesID CreateProperties(void)
{
    return SDL_CreateProperties();
}

bool CopyProperties(SDL_PropertiesID src, SDL_PropertiesID dst)
{
    return SDL_CopyProperties(src, dst);
}

bool LockProperties(SDL_PropertiesID props)
{
    return SDL_LockProperties(props);
}

void UnlockProperties(SDL_PropertiesID props)
{
    SDL_UnlockProperties(props);
}

bool SetPointerPropertyWithCleanup(SDL_PropertiesID            props,
                                   const char*                 name,
                                   void*                       value,
                                   SDL_CleanupPropertyCallback cleanup,
                                   void*                       userdata)
{
    return SDL_SetPointerPropertyWithCleanup(
        props, name, value, cleanup, userdata);
}

bool SetPointerProperty(SDL_PropertiesID props, const char* name, void* value)
{
    return SDL_SetPointerProperty(props, name, value);
}

bool SetStringProperty(SDL_PropertiesID props,
                       const char*      name,
                       const char*      value)
{
    return SDL_SetStringProperty(props, name, value);
}

bool SetNumberProperty(SDL_PropertiesID props, const char* name, Sint64 value)
{
    return SDL_SetNumberProperty(props, name, value);
}

bool SetFloatProperty(SDL_PropertiesID props, const char* name, float value)
{
    return SDL_SetFloatProperty(props, name, value);
}

bool SetBooleanProperty(SDL_PropertiesID props, const char* name, bool value)
{
    return SDL_SetBooleanProperty(props, name, value);
}

bool HasProperty(SDL_PropertiesID props, const char* name)
{
    return SDL_HasProperty(props, name);
}

SDL_PropertyType GetPropertyType(SDL_PropertiesID props, const char* name)
{
    return SDL_GetPropertyType(props, name);
}

void* GetPointerProperty(SDL_PropertiesID props,
                         const char*      name,
                         void*            default_value)
{
    return SDL_GetPointerProperty(props, name, default_value);
}

const char* GetStringProperty(SDL_PropertiesID props,
                              const char*      name,
                              const char*      default_value)
{
    return SDL_GetStringProperty(props, name, default_value);
}

Sint64 GetNumberProperty(SDL_PropertiesID props,
                         const char*      name,
                         Sint64           default_value)
{
    return SDL_GetNumberProperty(props, name, default_value);
}

float GetFloatProperty(SDL_PropertiesID props,
                       const char*      name,
                       float            default_value)
{
    return SDL_GetFloatProperty(props, name, default_value);
}

bool GetBooleanProperty(SDL_PropertiesID props,
                        const char*      name,
                        bool             default_value)
{
    return SDL_GetBooleanProperty(props, name, default_value);
}

bool ClearProperty(SDL_PropertiesID props, const char* name)
{
    return SDL_ClearProperty(props, name);
}

bool EnumerateProperties(SDL_PropertiesID                props,
                         SDL_EnumeratePropertiesCallback callback,
                         void*                           userdata)
{
    return SDL_EnumerateProperties(props, callback, userdata);
}

void DestroyProperties(SDL_PropertiesID props)
{
    SDL_DestroyProperties(props);
}

using SDL_Thread = SDL_Thread;

using ThreadID = Uint64;

using TLSID = SDL_AtomicInt;

enum class ThreadPriority
{
    PRIORITY_LOW           = SDL_THREAD_PRIORITY_LOW,
    PRIORITY_NORMAL        = SDL_THREAD_PRIORITY_NORMAL,
    PRIORITY_HIGH          = SDL_THREAD_PRIORITY_HIGH,
    PRIORITY_TIME_CRITICAL = SDL_THREAD_PRIORITY_TIME_CRITICAL,
};
REGULAR_ENUM(ThreadPriority);

enum class ThreadState
{
    UNKNOWN  = SDL_THREAD_UNKNOWN,
    ALIVE    = SDL_THREAD_ALIVE,
    DETACHED = SDL_THREAD_DETACHED,
    COMPLETE = SDL_THREAD_COMPLETE,
};
REGULAR_ENUM(ThreadState);

SDL_Thread* CreateThreadRuntime(SDL_ThreadFunction  fn,
                                const char*         name,
                                void*               data,
                                SDL_FunctionPointer pfnBeginThread,
                                SDL_FunctionPointer pfnEndThread)
{
    return SDL_CreateThreadRuntime(
        fn, name, data, pfnBeginThread, pfnEndThread);
}

SDL_Thread* CreateThreadWithPropertiesRuntime(
    SDL_PropertiesID    props,
    SDL_FunctionPointer pfnBeginThread,
    SDL_FunctionPointer pfnEndThread)
{
    return SDL_CreateThreadWithPropertiesRuntime(
        props, pfnBeginThread, pfnEndThread);
}

constexpr auto PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER()
{
    return "SDL.thread.create.entry_function";
}

constexpr auto PROP_THREAD_CREATE_NAME_STRING()
{
    return "SDL.thread.create.name";
}

constexpr auto PROP_THREAD_CREATE_USERDATA_POINTER()
{
    return "SDL.thread.create.userdata";
}

constexpr auto PROP_THREAD_CREATE_STACKSIZE_NUMBER()
{
    return "SDL.thread.create.stacksize";
}

const char* GetThreadName(SDL_Thread* thread)
{
    return SDL_GetThreadName(thread);
}

SDL_ThreadID GetCurrentThreadID(void)
{
    return SDL_GetCurrentThreadID();
}

SDL_ThreadID GetThreadID(SDL_Thread* thread)
{
    return SDL_GetThreadID(thread);
}

bool SetCurrentThreadPriority(ThreadPriority priority)
{
    return SDL_SetCurrentThreadPriority((SDL_ThreadPriority)(priority));
}

void WaitThread(SDL_Thread* thread, int* status)
{
    SDL_WaitThread(thread, status);
}

SDL_ThreadState GetThreadState(SDL_Thread* thread)
{
    return SDL_GetThreadState(thread);
}

void DetachThread(SDL_Thread* thread)
{
    SDL_DetachThread(thread);
}

void* GetTLS(SDL_TLSID* id)
{
    return SDL_GetTLS(id);
}

bool SetTLS(SDL_TLSID*                id,
            const void*               value,
            SDL_TLSDestructorCallback destructor)
{
    return SDL_SetTLS(id, value, destructor);
}

void CleanupTLS(void)
{
    SDL_CleanupTLS();
}

using SDL_Mutex = SDL_Mutex;

SDL_Mutex* CreateMutex(void)
{
    return SDL_CreateMutex();
}

void DestroyMutex(SDL_Mutex* mutex)
{
    SDL_DestroyMutex(mutex);
}

using SDL_RWLock = SDL_RWLock;

SDL_RWLock* CreateRWLock(void)
{
    return SDL_CreateRWLock();
}

void DestroyRWLock(SDL_RWLock* rwlock)
{
    SDL_DestroyRWLock(rwlock);
}

using SDL_Semaphore = SDL_Semaphore;

SDL_Semaphore* CreateSemaphore(Uint32 initial_value)
{
    return SDL_CreateSemaphore(initial_value);
}

void DestroySemaphore(SDL_Semaphore* sem)
{
    SDL_DestroySemaphore(sem);
}

void WaitSemaphore(SDL_Semaphore* sem)
{
    SDL_WaitSemaphore(sem);
}

bool TryWaitSemaphore(SDL_Semaphore* sem)
{
    return SDL_TryWaitSemaphore(sem);
}

bool WaitSemaphoreTimeout(SDL_Semaphore* sem, Sint32 timeoutMS)
{
    return SDL_WaitSemaphoreTimeout(sem, timeoutMS);
}

void SignalSemaphore(SDL_Semaphore* sem)
{
    SDL_SignalSemaphore(sem);
}

Uint32 GetSemaphoreValue(SDL_Semaphore* sem)
{
    return SDL_GetSemaphoreValue(sem);
}

using SDL_Condition = SDL_Condition;

SDL_Condition* CreateCondition(void)
{
    return SDL_CreateCondition();
}

void DestroyCondition(SDL_Condition* cond)
{
    SDL_DestroyCondition(cond);
}

void SignalCondition(SDL_Condition* cond)
{
    SDL_SignalCondition(cond);
}

void BroadcastCondition(SDL_Condition* cond)
{
    SDL_BroadcastCondition(cond);
}

void WaitCondition(SDL_Condition* cond, SDL_Mutex* mutex)
{
    SDL_WaitCondition(cond, mutex);
}

bool WaitConditionTimeout(SDL_Condition* cond,
                          SDL_Mutex*     mutex,
                          Sint32         timeoutMS)
{
    return SDL_WaitConditionTimeout(cond, mutex, timeoutMS);
}

enum class InitStatus
{
    STATUS_UNINITIALIZED  = SDL_INIT_STATUS_UNINITIALIZED,
    STATUS_INITIALIZING   = SDL_INIT_STATUS_INITIALIZING,
    STATUS_INITIALIZED    = SDL_INIT_STATUS_INITIALIZED,
    STATUS_UNINITIALIZING = SDL_INIT_STATUS_UNINITIALIZING,
};
REGULAR_ENUM(InitStatus);

using InitState = SDL_InitState;

bool ShouldInit(SDL_InitState* state)
{
    return SDL_ShouldInit(state);
}

bool ShouldQuit(SDL_InitState* state)
{
    return SDL_ShouldQuit(state);
}

void SetInitialized(SDL_InitState* state, bool initialized)
{
    SDL_SetInitialized(state, initialized);
}

enum class IOStatus
{
    STATUS_READY     = SDL_IO_STATUS_READY,
    STATUS_ERROR     = SDL_IO_STATUS_ERROR,
    STATUS_EOF       = SDL_IO_STATUS_EOF,
    STATUS_NOT_READY = SDL_IO_STATUS_NOT_READY,
    STATUS_READONLY  = SDL_IO_STATUS_READONLY,
    STATUS_WRITEONLY = SDL_IO_STATUS_WRITEONLY,
};
REGULAR_ENUM(IOStatus);

enum class IOWhence
{
    SEEK_SET = SDL_IO_SEEK_SET,
    SEEK_CUR = SDL_IO_SEEK_CUR,
    SEEK_END = SDL_IO_SEEK_END,
};
REGULAR_ENUM(IOWhence);

using IOStreamInterface = SDL_IOStreamInterface;

using SDL_IOStream = SDL_IOStream;

SDL_IOStream* IOFromFile(const char* file, const char* mode)
{
    return SDL_IOFromFile(file, mode);
}

constexpr auto PROP_IOSTREAM_WINDOWS_HANDLE_POINTER()
{
    return "SDL.iostream.windows.handle";
}

constexpr auto PROP_IOSTREAM_STDIO_FILE_POINTER()
{
    return "SDL.iostream.stdio.file";
}

constexpr auto PROP_IOSTREAM_FILE_DESCRIPTOR_NUMBER()
{
    return "SDL.iostream.file_descriptor";
}

constexpr auto PROP_IOSTREAM_ANDROID_AASSET_POINTER()
{
    return "SDL.iostream.android.aasset";
}

SDL_IOStream* IOFromMem(void* mem, size_t size)
{
    return SDL_IOFromMem(mem, size);
}

constexpr auto PROP_IOSTREAM_MEMORY_POINTER()
{
    return "SDL.iostream.memory.base";
}

constexpr auto PROP_IOSTREAM_MEMORY_SIZE_NUMBER()
{
    return "SDL.iostream.memory.size";
}

constexpr auto PROP_IOSTREAM_MEMORY_FREE_FUNC_POINTER()
{
    return "SDL.iostream.memory.free";
}

SDL_IOStream* IOFromConstMem(const void* mem, size_t size)
{
    return SDL_IOFromConstMem(mem, size);
}

SDL_IOStream* IOFromDynamicMem(void)
{
    return SDL_IOFromDynamicMem();
}

constexpr auto PROP_IOSTREAM_DYNAMIC_MEMORY_POINTER()
{
    return "SDL.iostream.dynamic.memory";
}

constexpr auto PROP_IOSTREAM_DYNAMIC_CHUNKSIZE_NUMBER()
{
    return "SDL.iostream.dynamic.chunksize";
}

SDL_IOStream* OpenIO(const SDL_IOStreamInterface* iface, void* userdata)
{
    return SDL_OpenIO(iface, userdata);
}

bool CloseIO(SDL_IOStream* context)
{
    return SDL_CloseIO(context);
}

SDL_PropertiesID GetIOProperties(SDL_IOStream* context)
{
    return SDL_GetIOProperties(context);
}

SDL_IOStatus GetIOStatus(SDL_IOStream* context)
{
    return SDL_GetIOStatus(context);
}

Sint64 GetIOSize(SDL_IOStream* context)
{
    return SDL_GetIOSize(context);
}

Sint64 SeekIO(SDL_IOStream* context, Sint64 offset, IOWhence whence)
{
    return SDL_SeekIO(context, offset, (SDL_IOWhence)(whence));
}

Sint64 TellIO(SDL_IOStream* context)
{
    return SDL_TellIO(context);
}

size_t ReadIO(SDL_IOStream* context, void* ptr, size_t size)
{
    return SDL_ReadIO(context, ptr, size);
}

size_t WriteIO(SDL_IOStream* context, const void* ptr, size_t size)
{
    return SDL_WriteIO(context, ptr, size);
}

bool FlushIO(SDL_IOStream* context)
{
    return SDL_FlushIO(context);
}

void* LoadFile_IO(SDL_IOStream* src, size_t* datasize, bool closeio)
{
    return SDL_LoadFile_IO(src, datasize, closeio);
}

void* LoadFile(const char* file, size_t* datasize)
{
    return SDL_LoadFile(file, datasize);
}

bool SaveFile_IO(SDL_IOStream* src,
                 const void*   data,
                 size_t        datasize,
                 bool          closeio)
{
    return SDL_SaveFile_IO(src, data, datasize, closeio);
}

bool SaveFile(const char* file, const void* data, size_t datasize)
{
    return SDL_SaveFile(file, data, datasize);
}

bool ReadU8(SDL_IOStream* src, Uint8* value)
{
    return SDL_ReadU8(src, value);
}

bool ReadS8(SDL_IOStream* src, Sint8* value)
{
    return SDL_ReadS8(src, value);
}

bool ReadU16LE(SDL_IOStream* src, Uint16* value)
{
    return SDL_ReadU16LE(src, value);
}

bool ReadS16LE(SDL_IOStream* src, Sint16* value)
{
    return SDL_ReadS16LE(src, value);
}

bool ReadU16BE(SDL_IOStream* src, Uint16* value)
{
    return SDL_ReadU16BE(src, value);
}

bool ReadS16BE(SDL_IOStream* src, Sint16* value)
{
    return SDL_ReadS16BE(src, value);
}

bool ReadU32LE(SDL_IOStream* src, Uint32* value)
{
    return SDL_ReadU32LE(src, value);
}

bool ReadS32LE(SDL_IOStream* src, Sint32* value)
{
    return SDL_ReadS32LE(src, value);
}

bool ReadU32BE(SDL_IOStream* src, Uint32* value)
{
    return SDL_ReadU32BE(src, value);
}

bool ReadS32BE(SDL_IOStream* src, Sint32* value)
{
    return SDL_ReadS32BE(src, value);
}

bool ReadU64LE(SDL_IOStream* src, Uint64* value)
{
    return SDL_ReadU64LE(src, value);
}

bool ReadS64LE(SDL_IOStream* src, Sint64* value)
{
    return SDL_ReadS64LE(src, value);
}

bool ReadU64BE(SDL_IOStream* src, Uint64* value)
{
    return SDL_ReadU64BE(src, value);
}

bool ReadS64BE(SDL_IOStream* src, Sint64* value)
{
    return SDL_ReadS64BE(src, value);
}

bool WriteU8(SDL_IOStream* dst, Uint8 value)
{
    return SDL_WriteU8(dst, value);
}

bool WriteS8(SDL_IOStream* dst, Sint8 value)
{
    return SDL_WriteS8(dst, value);
}

bool WriteU16LE(SDL_IOStream* dst, Uint16 value)
{
    return SDL_WriteU16LE(dst, value);
}

bool WriteS16LE(SDL_IOStream* dst, Sint16 value)
{
    return SDL_WriteS16LE(dst, value);
}

bool WriteU16BE(SDL_IOStream* dst, Uint16 value)
{
    return SDL_WriteU16BE(dst, value);
}

bool WriteS16BE(SDL_IOStream* dst, Sint16 value)
{
    return SDL_WriteS16BE(dst, value);
}

bool WriteU32LE(SDL_IOStream* dst, Uint32 value)
{
    return SDL_WriteU32LE(dst, value);
}

bool WriteS32LE(SDL_IOStream* dst, Sint32 value)
{
    return SDL_WriteS32LE(dst, value);
}

bool WriteU32BE(SDL_IOStream* dst, Uint32 value)
{
    return SDL_WriteU32BE(dst, value);
}

bool WriteS32BE(SDL_IOStream* dst, Sint32 value)
{
    return SDL_WriteS32BE(dst, value);
}

bool WriteU64LE(SDL_IOStream* dst, Uint64 value)
{
    return SDL_WriteU64LE(dst, value);
}

bool WriteS64LE(SDL_IOStream* dst, Sint64 value)
{
    return SDL_WriteS64LE(dst, value);
}

bool WriteU64BE(SDL_IOStream* dst, Uint64 value)
{
    return SDL_WriteU64BE(dst, value);
}

bool WriteS64BE(SDL_IOStream* dst, Sint64 value)
{
    return SDL_WriteS64BE(dst, value);
}

constexpr auto AUDIO_MASK_BITSIZE()
{
    return (0xFFu);
}

constexpr auto AUDIO_MASK_FLOAT()
{
    return (1u << 8);
}

constexpr auto AUDIO_MASK_BIG_ENDIAN()
{
    return (1u << 12);
}

constexpr auto AUDIO_MASK_SIGNED()
{
    return (1u << 15);
}

enum class AudioFormat
{
    UNKNOWN = SDL_AUDIO_UNKNOWN,
    U8      = SDL_AUDIO_U8,
    S8      = SDL_AUDIO_S8,
    S16LE   = SDL_AUDIO_S16LE,
    S16BE   = SDL_AUDIO_S16BE,
    S32LE   = SDL_AUDIO_S32LE,
    S32BE   = SDL_AUDIO_S32BE,
    F32LE   = SDL_AUDIO_F32LE,
    F32BE   = SDL_AUDIO_F32BE,
    S16     = SDL_AUDIO_S16,
    S32     = SDL_AUDIO_S32,
    F32     = SDL_AUDIO_F32,
};
REGULAR_ENUM(AudioFormat);

using AudioDeviceID = Uint32;

constexpr auto AUDIO_DEVICE_DEFAULT_PLAYBACK()
{
    return ((SDL_AudioDeviceID)0xFFFFFFFFu);
}

constexpr auto AUDIO_DEVICE_DEFAULT_RECORDING()
{
    return ((SDL_AudioDeviceID)0xFFFFFFFEu);
}

using AudioSpec = SDL_AudioSpec;

using SDL_AudioStream = SDL_AudioStream;

int GetNumAudioDrivers(void)
{
    return SDL_GetNumAudioDrivers();
}

const char* GetAudioDriver(int index)
{
    return SDL_GetAudioDriver(index);
}

const char* GetCurrentAudioDriver(void)
{
    return SDL_GetCurrentAudioDriver();
}

SDL_AudioDeviceID* GetAudioPlaybackDevices(int* count)
{
    return SDL_GetAudioPlaybackDevices(count);
}

SDL_AudioDeviceID* GetAudioRecordingDevices(int* count)
{
    return SDL_GetAudioRecordingDevices(count);
}

const char* GetAudioDeviceName(SDL_AudioDeviceID devid)
{
    return SDL_GetAudioDeviceName(devid);
}

bool GetAudioDeviceFormat(SDL_AudioDeviceID devid,
                          SDL_AudioSpec*    spec,
                          int*              sample_frames)
{
    return SDL_GetAudioDeviceFormat(devid, spec, sample_frames);
}

int* GetAudioDeviceChannelMap(SDL_AudioDeviceID devid, int* count)
{
    return SDL_GetAudioDeviceChannelMap(devid, count);
}

SDL_AudioDeviceID OpenAudioDevice(SDL_AudioDeviceID    devid,
                                  const SDL_AudioSpec* spec)
{
    return SDL_OpenAudioDevice(devid, spec);
}

bool IsAudioDevicePhysical(SDL_AudioDeviceID devid)
{
    return SDL_IsAudioDevicePhysical(devid);
}

bool IsAudioDevicePlayback(SDL_AudioDeviceID devid)
{
    return SDL_IsAudioDevicePlayback(devid);
}

bool PauseAudioDevice(SDL_AudioDeviceID devid)
{
    return SDL_PauseAudioDevice(devid);
}

bool ResumeAudioDevice(SDL_AudioDeviceID devid)
{
    return SDL_ResumeAudioDevice(devid);
}

bool AudioDevicePaused(SDL_AudioDeviceID devid)
{
    return SDL_AudioDevicePaused(devid);
}

float GetAudioDeviceGain(SDL_AudioDeviceID devid)
{
    return SDL_GetAudioDeviceGain(devid);
}

bool SetAudioDeviceGain(SDL_AudioDeviceID devid, float gain)
{
    return SDL_SetAudioDeviceGain(devid, gain);
}

void CloseAudioDevice(SDL_AudioDeviceID devid)
{
    SDL_CloseAudioDevice(devid);
}

bool BindAudioStreams(SDL_AudioDeviceID devid,
                      SDL_AudioStream** streams,
                      int               num_streams)
{
    return SDL_BindAudioStreams(devid, streams, num_streams);
}

bool BindAudioStream(SDL_AudioDeviceID devid, SDL_AudioStream* stream)
{
    return SDL_BindAudioStream(devid, stream);
}

void UnbindAudioStreams(SDL_AudioStream** streams, int num_streams)
{
    SDL_UnbindAudioStreams(streams, num_streams);
}

void UnbindAudioStream(SDL_AudioStream* stream)
{
    SDL_UnbindAudioStream(stream);
}

SDL_AudioDeviceID GetAudioStreamDevice(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamDevice(stream);
}

SDL_AudioStream* CreateAudioStream(const SDL_AudioSpec* src_spec,
                                   const SDL_AudioSpec* dst_spec)
{
    return SDL_CreateAudioStream(src_spec, dst_spec);
}

SDL_PropertiesID GetAudioStreamProperties(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamProperties(stream);
}

constexpr auto PROP_AUDIOSTREAM_AUTO_CLEANUP_BOOLEAN()
{
    return "SDL.audiostream.auto_cleanup";
}

bool GetAudioStreamFormat(SDL_AudioStream* stream,
                          SDL_AudioSpec*   src_spec,
                          SDL_AudioSpec*   dst_spec)
{
    return SDL_GetAudioStreamFormat(stream, src_spec, dst_spec);
}

bool SetAudioStreamFormat(SDL_AudioStream*     stream,
                          const SDL_AudioSpec* src_spec,
                          const SDL_AudioSpec* dst_spec)
{
    return SDL_SetAudioStreamFormat(stream, src_spec, dst_spec);
}

float GetAudioStreamFrequencyRatio(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamFrequencyRatio(stream);
}

bool SetAudioStreamFrequencyRatio(SDL_AudioStream* stream, float ratio)
{
    return SDL_SetAudioStreamFrequencyRatio(stream, ratio);
}

float GetAudioStreamGain(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamGain(stream);
}

bool SetAudioStreamGain(SDL_AudioStream* stream, float gain)
{
    return SDL_SetAudioStreamGain(stream, gain);
}

int* GetAudioStreamInputChannelMap(SDL_AudioStream* stream, int* count)
{
    return SDL_GetAudioStreamInputChannelMap(stream, count);
}

int* GetAudioStreamOutputChannelMap(SDL_AudioStream* stream, int* count)
{
    return SDL_GetAudioStreamOutputChannelMap(stream, count);
}

bool SetAudioStreamInputChannelMap(SDL_AudioStream* stream,
                                   const int*       chmap,
                                   int              count)
{
    return SDL_SetAudioStreamInputChannelMap(stream, chmap, count);
}

bool SetAudioStreamOutputChannelMap(SDL_AudioStream* stream,
                                    const int*       chmap,
                                    int              count)
{
    return SDL_SetAudioStreamOutputChannelMap(stream, chmap, count);
}

bool PutAudioStreamData(SDL_AudioStream* stream, const void* buf, int len)
{
    return SDL_PutAudioStreamData(stream, buf, len);
}

bool PutAudioStreamDataNoCopy(SDL_AudioStream*                    stream,
                              const void*                         buf,
                              int                                 len,
                              SDL_AudioStreamDataCompleteCallback callback,
                              void*                               userdata)
{
    return SDL_PutAudioStreamDataNoCopy(stream, buf, len, callback, userdata);
}

bool PutAudioStreamPlanarData(SDL_AudioStream* stream,
                              const void**     channel_buffers,
                              int              num_channels,
                              int              num_samples)
{
    return SDL_PutAudioStreamPlanarData(
        stream, channel_buffers, num_channels, num_samples);
}

int GetAudioStreamData(SDL_AudioStream* stream, void* buf, int len)
{
    return SDL_GetAudioStreamData(stream, buf, len);
}

int GetAudioStreamAvailable(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamAvailable(stream);
}

int GetAudioStreamQueued(SDL_AudioStream* stream)
{
    return SDL_GetAudioStreamQueued(stream);
}

bool FlushAudioStream(SDL_AudioStream* stream)
{
    return SDL_FlushAudioStream(stream);
}

bool ClearAudioStream(SDL_AudioStream* stream)
{
    return SDL_ClearAudioStream(stream);
}

bool PauseAudioStreamDevice(SDL_AudioStream* stream)
{
    return SDL_PauseAudioStreamDevice(stream);
}

bool ResumeAudioStreamDevice(SDL_AudioStream* stream)
{
    return SDL_ResumeAudioStreamDevice(stream);
}

bool AudioStreamDevicePaused(SDL_AudioStream* stream)
{
    return SDL_AudioStreamDevicePaused(stream);
}

bool LockAudioStream(SDL_AudioStream* stream)
{
    return SDL_LockAudioStream(stream);
}

bool UnlockAudioStream(SDL_AudioStream* stream)
{
    return SDL_UnlockAudioStream(stream);
}

bool SetAudioStreamGetCallback(SDL_AudioStream*        stream,
                               SDL_AudioStreamCallback callback,
                               void*                   userdata)
{
    return SDL_SetAudioStreamGetCallback(stream, callback, userdata);
}

bool SetAudioStreamPutCallback(SDL_AudioStream*        stream,
                               SDL_AudioStreamCallback callback,
                               void*                   userdata)
{
    return SDL_SetAudioStreamPutCallback(stream, callback, userdata);
}

void DestroyAudioStream(SDL_AudioStream* stream)
{
    SDL_DestroyAudioStream(stream);
}

SDL_AudioStream* OpenAudioDeviceStream(SDL_AudioDeviceID       devid,
                                       const SDL_AudioSpec*    spec,
                                       SDL_AudioStreamCallback callback,
                                       void*                   userdata)
{
    return SDL_OpenAudioDeviceStream(devid, spec, callback, userdata);
}

bool SetAudioPostmixCallback(SDL_AudioDeviceID        devid,
                             SDL_AudioPostmixCallback callback,
                             void*                    userdata)
{
    return SDL_SetAudioPostmixCallback(devid, callback, userdata);
}

bool LoadWAV_IO(SDL_IOStream*  src,
                bool           closeio,
                SDL_AudioSpec* spec,
                Uint8**        audio_buf,
                Uint32*        audio_len)
{
    return SDL_LoadWAV_IO(src, closeio, spec, audio_buf, audio_len);
}

bool LoadWAV(const char*    path,
             SDL_AudioSpec* spec,
             Uint8**        audio_buf,
             Uint32*        audio_len)
{
    return SDL_LoadWAV(path, spec, audio_buf, audio_len);
}

bool MixAudio(
    Uint8* dst, const Uint8* src, AudioFormat format, Uint32 len, float volume)
{
    return SDL_MixAudio(dst, src, (SDL_AudioFormat)(format), len, volume);
}

bool ConvertAudioSamples(const SDL_AudioSpec* src_spec,
                         const Uint8*         src_data,
                         int                  src_len,
                         const SDL_AudioSpec* dst_spec,
                         Uint8**              dst_data,
                         int*                 dst_len)
{
    return SDL_ConvertAudioSamples(
        src_spec, src_data, src_len, dst_spec, dst_data, dst_len);
}

const char* GetAudioFormatName(AudioFormat format)
{
    return SDL_GetAudioFormatName((SDL_AudioFormat)(format));
}

int GetSilenceValueForFormat(AudioFormat format)
{
    return SDL_GetSilenceValueForFormat((SDL_AudioFormat)(format));
}

enum class BlendMode : Uint32
{
    NONE                = SDL_BLENDMODE_NONE,
    BLEND               = SDL_BLENDMODE_BLEND,
    BLEND_PREMULTIPLIED = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    ADD                 = SDL_BLENDMODE_ADD,
    ADD_PREMULTIPLIED   = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    MOD                 = SDL_BLENDMODE_MOD,
    MUL                 = SDL_BLENDMODE_MUL,
    INVALID             = SDL_BLENDMODE_INVALID,
};
BITFLAG_ENUM(BlendMode);

enum class BlendOperation
{
    ADD          = SDL_BLENDOPERATION_ADD,
    SUBTRACT     = SDL_BLENDOPERATION_SUBTRACT,
    REV_SUBTRACT = SDL_BLENDOPERATION_REV_SUBTRACT,
    MINIMUM      = SDL_BLENDOPERATION_MINIMUM,
    MAXIMUM      = SDL_BLENDOPERATION_MAXIMUM,
};
REGULAR_ENUM(BlendOperation);

enum class BlendFactor
{
    ZERO                = SDL_BLENDFACTOR_ZERO,
    ONE                 = SDL_BLENDFACTOR_ONE,
    SRC_COLOR           = SDL_BLENDFACTOR_SRC_COLOR,
    ONE_MINUS_SRC_COLOR = SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
    SRC_ALPHA           = SDL_BLENDFACTOR_SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    DST_COLOR           = SDL_BLENDFACTOR_DST_COLOR,
    ONE_MINUS_DST_COLOR = SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    DST_ALPHA           = SDL_BLENDFACTOR_DST_ALPHA,
    ONE_MINUS_DST_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
};
REGULAR_ENUM(BlendFactor);

SDL_BlendMode ComposeCustomBlendMode(BlendFactor    srcColorFactor,
                                     BlendFactor    dstColorFactor,
                                     BlendOperation colorOperation,
                                     BlendFactor    srcAlphaFactor,
                                     BlendFactor    dstAlphaFactor,
                                     BlendOperation alphaOperation)
{
    return SDL_ComposeCustomBlendMode((SDL_BlendFactor)(srcColorFactor),
                                      (SDL_BlendFactor)(dstColorFactor),
                                      (SDL_BlendOperation)(colorOperation),
                                      (SDL_BlendFactor)(srcAlphaFactor),
                                      (SDL_BlendFactor)(dstAlphaFactor),
                                      (SDL_BlendOperation)(alphaOperation));
}

constexpr auto ALPHA_OPAQUE()
{
    return 255;
}

constexpr auto ALPHA_OPAQUE_FLOAT()
{
    return 1.0f;
}

constexpr auto ALPHA_TRANSPARENT()
{
    return 0;
}

constexpr auto ALPHA_TRANSPARENT_FLOAT()
{
    return 0.0f;
}

enum class PixelType
{
    UNKNOWN  = SDL_PIXELTYPE_UNKNOWN,
    INDEX1   = SDL_PIXELTYPE_INDEX1,
    INDEX4   = SDL_PIXELTYPE_INDEX4,
    INDEX8   = SDL_PIXELTYPE_INDEX8,
    PACKED8  = SDL_PIXELTYPE_PACKED8,
    PACKED16 = SDL_PIXELTYPE_PACKED16,
    PACKED32 = SDL_PIXELTYPE_PACKED32,
    ARRAYU8  = SDL_PIXELTYPE_ARRAYU8,
    ARRAYU16 = SDL_PIXELTYPE_ARRAYU16,
    ARRAYU32 = SDL_PIXELTYPE_ARRAYU32,
    ARRAYF16 = SDL_PIXELTYPE_ARRAYF16,
    ARRAYF32 = SDL_PIXELTYPE_ARRAYF32,
    INDEX2   = SDL_PIXELTYPE_INDEX2,
};
REGULAR_ENUM(PixelType);

enum class BitmapOrder
{
    NONE  = SDL_BITMAPORDER_NONE,
    _4321 = SDL_BITMAPORDER_4321,
    _1234 = SDL_BITMAPORDER_1234,
};
REGULAR_ENUM(BitmapOrder);

enum class PackedOrder
{
    NONE = SDL_PACKEDORDER_NONE,
    XRGB = SDL_PACKEDORDER_XRGB,
    RGBX = SDL_PACKEDORDER_RGBX,
    ARGB = SDL_PACKEDORDER_ARGB,
    RGBA = SDL_PACKEDORDER_RGBA,
    XBGR = SDL_PACKEDORDER_XBGR,
    BGRX = SDL_PACKEDORDER_BGRX,
    ABGR = SDL_PACKEDORDER_ABGR,
    BGRA = SDL_PACKEDORDER_BGRA,
};
REGULAR_ENUM(PackedOrder);

enum class ArrayOrder
{
    NONE = SDL_ARRAYORDER_NONE,
    RGB  = SDL_ARRAYORDER_RGB,
    RGBA = SDL_ARRAYORDER_RGBA,
    ARGB = SDL_ARRAYORDER_ARGB,
    BGR  = SDL_ARRAYORDER_BGR,
    BGRA = SDL_ARRAYORDER_BGRA,
    ABGR = SDL_ARRAYORDER_ABGR,
};
REGULAR_ENUM(ArrayOrder);

enum class PackedLayout
{
    NONE     = SDL_PACKEDLAYOUT_NONE,
    _332     = SDL_PACKEDLAYOUT_332,
    _4444    = SDL_PACKEDLAYOUT_4444,
    _1555    = SDL_PACKEDLAYOUT_1555,
    _5551    = SDL_PACKEDLAYOUT_5551,
    _565     = SDL_PACKEDLAYOUT_565,
    _8888    = SDL_PACKEDLAYOUT_8888,
    _2101010 = SDL_PACKEDLAYOUT_2101010,
    _1010102 = SDL_PACKEDLAYOUT_1010102,
};
REGULAR_ENUM(PackedLayout);

enum class PixelFormat
{
    UNKNOWN       = SDL_PIXELFORMAT_UNKNOWN,
    INDEX1LSB     = SDL_PIXELFORMAT_INDEX1LSB,
    INDEX1MSB     = SDL_PIXELFORMAT_INDEX1MSB,
    INDEX2LSB     = SDL_PIXELFORMAT_INDEX2LSB,
    INDEX2MSB     = SDL_PIXELFORMAT_INDEX2MSB,
    INDEX4LSB     = SDL_PIXELFORMAT_INDEX4LSB,
    INDEX4MSB     = SDL_PIXELFORMAT_INDEX4MSB,
    INDEX8        = SDL_PIXELFORMAT_INDEX8,
    RGB332        = SDL_PIXELFORMAT_RGB332,
    XRGB4444      = SDL_PIXELFORMAT_XRGB4444,
    XBGR4444      = SDL_PIXELFORMAT_XBGR4444,
    XRGB1555      = SDL_PIXELFORMAT_XRGB1555,
    XBGR1555      = SDL_PIXELFORMAT_XBGR1555,
    ARGB4444      = SDL_PIXELFORMAT_ARGB4444,
    RGBA4444      = SDL_PIXELFORMAT_RGBA4444,
    ABGR4444      = SDL_PIXELFORMAT_ABGR4444,
    BGRA4444      = SDL_PIXELFORMAT_BGRA4444,
    ARGB1555      = SDL_PIXELFORMAT_ARGB1555,
    RGBA5551      = SDL_PIXELFORMAT_RGBA5551,
    ABGR1555      = SDL_PIXELFORMAT_ABGR1555,
    BGRA5551      = SDL_PIXELFORMAT_BGRA5551,
    RGB565        = SDL_PIXELFORMAT_RGB565,
    BGR565        = SDL_PIXELFORMAT_BGR565,
    RGB24         = SDL_PIXELFORMAT_RGB24,
    BGR24         = SDL_PIXELFORMAT_BGR24,
    XRGB8888      = SDL_PIXELFORMAT_XRGB8888,
    RGBX8888      = SDL_PIXELFORMAT_RGBX8888,
    XBGR8888      = SDL_PIXELFORMAT_XBGR8888,
    BGRX8888      = SDL_PIXELFORMAT_BGRX8888,
    ARGB8888      = SDL_PIXELFORMAT_ARGB8888,
    RGBA8888      = SDL_PIXELFORMAT_RGBA8888,
    ABGR8888      = SDL_PIXELFORMAT_ABGR8888,
    BGRA8888      = SDL_PIXELFORMAT_BGRA8888,
    XRGB2101010   = SDL_PIXELFORMAT_XRGB2101010,
    XBGR2101010   = SDL_PIXELFORMAT_XBGR2101010,
    ARGB2101010   = SDL_PIXELFORMAT_ARGB2101010,
    ABGR2101010   = SDL_PIXELFORMAT_ABGR2101010,
    RGB48         = SDL_PIXELFORMAT_RGB48,
    BGR48         = SDL_PIXELFORMAT_BGR48,
    RGBA64        = SDL_PIXELFORMAT_RGBA64,
    ARGB64        = SDL_PIXELFORMAT_ARGB64,
    BGRA64        = SDL_PIXELFORMAT_BGRA64,
    ABGR64        = SDL_PIXELFORMAT_ABGR64,
    RGB48_FLOAT   = SDL_PIXELFORMAT_RGB48_FLOAT,
    BGR48_FLOAT   = SDL_PIXELFORMAT_BGR48_FLOAT,
    RGBA64_FLOAT  = SDL_PIXELFORMAT_RGBA64_FLOAT,
    ARGB64_FLOAT  = SDL_PIXELFORMAT_ARGB64_FLOAT,
    BGRA64_FLOAT  = SDL_PIXELFORMAT_BGRA64_FLOAT,
    ABGR64_FLOAT  = SDL_PIXELFORMAT_ABGR64_FLOAT,
    RGB96_FLOAT   = SDL_PIXELFORMAT_RGB96_FLOAT,
    BGR96_FLOAT   = SDL_PIXELFORMAT_BGR96_FLOAT,
    RGBA128_FLOAT = SDL_PIXELFORMAT_RGBA128_FLOAT,
    ARGB128_FLOAT = SDL_PIXELFORMAT_ARGB128_FLOAT,
    BGRA128_FLOAT = SDL_PIXELFORMAT_BGRA128_FLOAT,
    ABGR128_FLOAT = SDL_PIXELFORMAT_ABGR128_FLOAT,
    YV12          = SDL_PIXELFORMAT_YV12,
    IYUV          = SDL_PIXELFORMAT_IYUV,
    YUY2          = SDL_PIXELFORMAT_YUY2,
    UYVY          = SDL_PIXELFORMAT_UYVY,
    YVYU          = SDL_PIXELFORMAT_YVYU,
    NV12          = SDL_PIXELFORMAT_NV12,
    NV21          = SDL_PIXELFORMAT_NV21,
    P010          = SDL_PIXELFORMAT_P010,
    EXTERNAL_OES  = SDL_PIXELFORMAT_EXTERNAL_OES,
    MJPG          = SDL_PIXELFORMAT_MJPG,
    RGBA32        = SDL_PIXELFORMAT_RGBA32,
    ARGB32        = SDL_PIXELFORMAT_ARGB32,
    BGRA32        = SDL_PIXELFORMAT_BGRA32,
    ABGR32        = SDL_PIXELFORMAT_ABGR32,
    RGBX32        = SDL_PIXELFORMAT_RGBX32,
    XRGB32        = SDL_PIXELFORMAT_XRGB32,
    BGRX32        = SDL_PIXELFORMAT_BGRX32,
    XBGR32        = SDL_PIXELFORMAT_XBGR32,
};
REGULAR_ENUM(PixelFormat);

enum class ColorType
{
    TYPE_UNKNOWN = SDL_COLOR_TYPE_UNKNOWN,
    TYPE_RGB     = SDL_COLOR_TYPE_RGB,
    TYPE_YCBCR   = SDL_COLOR_TYPE_YCBCR,
};
REGULAR_ENUM(ColorType);

enum class ColorRange
{
    RANGE_UNKNOWN = SDL_COLOR_RANGE_UNKNOWN,
    RANGE_LIMITED = SDL_COLOR_RANGE_LIMITED,
    RANGE_FULL    = SDL_COLOR_RANGE_FULL,
};
REGULAR_ENUM(ColorRange);

enum class ColorPrimaries
{
    PRIMARIES_UNKNOWN      = SDL_COLOR_PRIMARIES_UNKNOWN,
    PRIMARIES_BT709        = SDL_COLOR_PRIMARIES_BT709,
    PRIMARIES_UNSPECIFIED  = SDL_COLOR_PRIMARIES_UNSPECIFIED,
    PRIMARIES_BT470M       = SDL_COLOR_PRIMARIES_BT470M,
    PRIMARIES_BT470BG      = SDL_COLOR_PRIMARIES_BT470BG,
    PRIMARIES_BT601        = SDL_COLOR_PRIMARIES_BT601,
    PRIMARIES_SMPTE240     = SDL_COLOR_PRIMARIES_SMPTE240,
    PRIMARIES_GENERIC_FILM = SDL_COLOR_PRIMARIES_GENERIC_FILM,
    PRIMARIES_BT2020       = SDL_COLOR_PRIMARIES_BT2020,
    PRIMARIES_XYZ          = SDL_COLOR_PRIMARIES_XYZ,
    PRIMARIES_SMPTE431     = SDL_COLOR_PRIMARIES_SMPTE431,
    PRIMARIES_SMPTE432     = SDL_COLOR_PRIMARIES_SMPTE432,
    PRIMARIES_EBU3213      = SDL_COLOR_PRIMARIES_EBU3213,
    PRIMARIES_CUSTOM       = SDL_COLOR_PRIMARIES_CUSTOM,
};
REGULAR_ENUM(ColorPrimaries);

enum class TransferCharacteristics
{
    CHARACTERISTICS_UNKNOWN       = SDL_TRANSFER_CHARACTERISTICS_UNKNOWN,
    CHARACTERISTICS_BT709         = SDL_TRANSFER_CHARACTERISTICS_BT709,
    CHARACTERISTICS_UNSPECIFIED   = SDL_TRANSFER_CHARACTERISTICS_UNSPECIFIED,
    CHARACTERISTICS_GAMMA22       = SDL_TRANSFER_CHARACTERISTICS_GAMMA22,
    CHARACTERISTICS_GAMMA28       = SDL_TRANSFER_CHARACTERISTICS_GAMMA28,
    CHARACTERISTICS_BT601         = SDL_TRANSFER_CHARACTERISTICS_BT601,
    CHARACTERISTICS_SMPTE240      = SDL_TRANSFER_CHARACTERISTICS_SMPTE240,
    CHARACTERISTICS_LINEAR        = SDL_TRANSFER_CHARACTERISTICS_LINEAR,
    CHARACTERISTICS_LOG100        = SDL_TRANSFER_CHARACTERISTICS_LOG100,
    CHARACTERISTICS_LOG100_SQRT10 = SDL_TRANSFER_CHARACTERISTICS_LOG100_SQRT10,
    CHARACTERISTICS_IEC61966      = SDL_TRANSFER_CHARACTERISTICS_IEC61966,
    CHARACTERISTICS_BT1361        = SDL_TRANSFER_CHARACTERISTICS_BT1361,
    CHARACTERISTICS_SRGB          = SDL_TRANSFER_CHARACTERISTICS_SRGB,
    CHARACTERISTICS_BT2020_10BIT  = SDL_TRANSFER_CHARACTERISTICS_BT2020_10BIT,
    CHARACTERISTICS_BT2020_12BIT  = SDL_TRANSFER_CHARACTERISTICS_BT2020_12BIT,
    CHARACTERISTICS_PQ            = SDL_TRANSFER_CHARACTERISTICS_PQ,
    CHARACTERISTICS_SMPTE428      = SDL_TRANSFER_CHARACTERISTICS_SMPTE428,
    CHARACTERISTICS_HLG           = SDL_TRANSFER_CHARACTERISTICS_HLG,
    CHARACTERISTICS_CUSTOM        = SDL_TRANSFER_CHARACTERISTICS_CUSTOM,
};
REGULAR_ENUM(TransferCharacteristics);

enum class MatrixCoefficients
{
    COEFFICIENTS_IDENTITY    = SDL_MATRIX_COEFFICIENTS_IDENTITY,
    COEFFICIENTS_BT709       = SDL_MATRIX_COEFFICIENTS_BT709,
    COEFFICIENTS_UNSPECIFIED = SDL_MATRIX_COEFFICIENTS_UNSPECIFIED,
    COEFFICIENTS_FCC         = SDL_MATRIX_COEFFICIENTS_FCC,
    COEFFICIENTS_BT470BG     = SDL_MATRIX_COEFFICIENTS_BT470BG,
    COEFFICIENTS_BT601       = SDL_MATRIX_COEFFICIENTS_BT601,
    COEFFICIENTS_SMPTE240    = SDL_MATRIX_COEFFICIENTS_SMPTE240,
    COEFFICIENTS_YCGCO       = SDL_MATRIX_COEFFICIENTS_YCGCO,
    COEFFICIENTS_BT2020_NCL  = SDL_MATRIX_COEFFICIENTS_BT2020_NCL,
    COEFFICIENTS_BT2020_CL   = SDL_MATRIX_COEFFICIENTS_BT2020_CL,
    COEFFICIENTS_SMPTE2085   = SDL_MATRIX_COEFFICIENTS_SMPTE2085,
    COEFFICIENTS_CHROMA_DERIVED_NCL =
        SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_NCL,
    COEFFICIENTS_CHROMA_DERIVED_CL = SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_CL,
    COEFFICIENTS_ICTCP             = SDL_MATRIX_COEFFICIENTS_ICTCP,
    COEFFICIENTS_CUSTOM            = SDL_MATRIX_COEFFICIENTS_CUSTOM,
};
REGULAR_ENUM(MatrixCoefficients);

enum class ChromaLocation
{
    LOCATION_NONE    = SDL_CHROMA_LOCATION_NONE,
    LOCATION_LEFT    = SDL_CHROMA_LOCATION_LEFT,
    LOCATION_CENTER  = SDL_CHROMA_LOCATION_CENTER,
    LOCATION_TOPLEFT = SDL_CHROMA_LOCATION_TOPLEFT,
};
REGULAR_ENUM(ChromaLocation);

enum class Colorspace
{
    UNKNOWN        = SDL_COLORSPACE_UNKNOWN,
    SRGB           = SDL_COLORSPACE_SRGB,
    SRGB_LINEAR    = SDL_COLORSPACE_SRGB_LINEAR,
    HDR10          = SDL_COLORSPACE_HDR10,
    JPEG           = SDL_COLORSPACE_JPEG,
    BT601_LIMITED  = SDL_COLORSPACE_BT601_LIMITED,
    BT601_FULL     = SDL_COLORSPACE_BT601_FULL,
    BT709_LIMITED  = SDL_COLORSPACE_BT709_LIMITED,
    BT709_FULL     = SDL_COLORSPACE_BT709_FULL,
    BT2020_LIMITED = SDL_COLORSPACE_BT2020_LIMITED,
    BT2020_FULL    = SDL_COLORSPACE_BT2020_FULL,
    RGB_DEFAULT    = SDL_COLORSPACE_RGB_DEFAULT,
    YUV_DEFAULT    = SDL_COLORSPACE_YUV_DEFAULT,
};
REGULAR_ENUM(Colorspace);

using Color = SDL_Color;

using FColor = SDL_FColor;

using Palette = SDL_Palette;

using PixelFormatDetails = SDL_PixelFormatDetails;

const char* GetPixelFormatName(PixelFormat format)
{
    return SDL_GetPixelFormatName((SDL_PixelFormat)(format));
}

bool GetMasksForPixelFormat(PixelFormat format,
                            int*        bpp,
                            Uint32*     Rmask,
                            Uint32*     Gmask,
                            Uint32*     Bmask,
                            Uint32*     Amask)
{
    return SDL_GetMasksForPixelFormat(
        (SDL_PixelFormat)(format), bpp, Rmask, Gmask, Bmask, Amask);
}

SDL_PixelFormat GetPixelFormatForMasks(
    int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    return SDL_GetPixelFormatForMasks(bpp, Rmask, Gmask, Bmask, Amask);
}

const SDL_PixelFormatDetails* GetPixelFormatDetails(PixelFormat format)
{
    return SDL_GetPixelFormatDetails((SDL_PixelFormat)(format));
}

SDL_Palette* CreatePalette(int ncolors)
{
    return SDL_CreatePalette(ncolors);
}

bool SetPaletteColors(SDL_Palette*     palette,
                      const SDL_Color* colors,
                      int              firstcolor,
                      int              ncolors)
{
    return SDL_SetPaletteColors(palette, colors, firstcolor, ncolors);
}

void DestroyPalette(SDL_Palette* palette)
{
    SDL_DestroyPalette(palette);
}

Uint32 MapRGB(const SDL_PixelFormatDetails* format,
              const SDL_Palette*            palette,
              Uint8                         r,
              Uint8                         g,
              Uint8                         b)
{
    return SDL_MapRGB(format, palette, r, g, b);
}

Uint32 MapRGBA(const SDL_PixelFormatDetails* format,
               const SDL_Palette*            palette,
               Uint8                         r,
               Uint8                         g,
               Uint8                         b,
               Uint8                         a)
{
    return SDL_MapRGBA(format, palette, r, g, b, a);
}

void GetRGB(Uint32                        pixelvalue,
            const SDL_PixelFormatDetails* format,
            const SDL_Palette*            palette,
            Uint8*                        r,
            Uint8*                        g,
            Uint8*                        b)
{
    SDL_GetRGB(pixelvalue, format, palette, r, g, b);
}

void GetRGBA(Uint32                        pixelvalue,
             const SDL_PixelFormatDetails* format,
             const SDL_Palette*            palette,
             Uint8*                        r,
             Uint8*                        g,
             Uint8*                        b,
             Uint8*                        a)
{
    SDL_GetRGBA(pixelvalue, format, palette, r, g, b, a);
}

using Point = SDL_Point;

using FPoint = SDL_FPoint;

using Rect = SDL_Rect;

using FRect = SDL_FRect;

bool HasRectIntersection(const SDL_Rect* A, const SDL_Rect* B)
{
    return SDL_HasRectIntersection(A, B);
}

bool GetRectIntersection(const SDL_Rect* A, const SDL_Rect* B, SDL_Rect* result)
{
    return SDL_GetRectIntersection(A, B, result);
}

bool GetRectUnion(const SDL_Rect* A, const SDL_Rect* B, SDL_Rect* result)
{
    return SDL_GetRectUnion(A, B, result);
}

bool GetRectEnclosingPoints(const SDL_Point* points,
                            int              count,
                            const SDL_Rect*  clip,
                            SDL_Rect*        result)
{
    return SDL_GetRectEnclosingPoints(points, count, clip, result);
}

bool GetRectAndLineIntersection(
    const SDL_Rect* rect, int* X1, int* Y1, int* X2, int* Y2)
{
    return SDL_GetRectAndLineIntersection(rect, X1, Y1, X2, Y2);
}

bool HasRectIntersectionFloat(const SDL_FRect* A, const SDL_FRect* B)
{
    return SDL_HasRectIntersectionFloat(A, B);
}

bool GetRectIntersectionFloat(const SDL_FRect* A,
                              const SDL_FRect* B,
                              SDL_FRect*       result)
{
    return SDL_GetRectIntersectionFloat(A, B, result);
}

bool GetRectUnionFloat(const SDL_FRect* A,
                       const SDL_FRect* B,
                       SDL_FRect*       result)
{
    return SDL_GetRectUnionFloat(A, B, result);
}

bool GetRectEnclosingPointsFloat(const SDL_FPoint* points,
                                 int               count,
                                 const SDL_FRect*  clip,
                                 SDL_FRect*        result)
{
    return SDL_GetRectEnclosingPointsFloat(points, count, clip, result);
}

bool GetRectAndLineIntersectionFloat(
    const SDL_FRect* rect, float* X1, float* Y1, float* X2, float* Y2)
{
    return SDL_GetRectAndLineIntersectionFloat(rect, X1, Y1, X2, Y2);
}

enum class SurfaceFlags : Uint32
{
    PREALLOCATED = SDL_SURFACE_PREALLOCATED,
    LOCK_NEEDED  = SDL_SURFACE_LOCK_NEEDED,
    LOCKED       = SDL_SURFACE_LOCKED,
    SIMD_ALIGNED = SDL_SURFACE_SIMD_ALIGNED,
};
BITFLAG_ENUM(SurfaceFlags);

enum class ScaleMode
{
    INVALID  = SDL_SCALEMODE_INVALID,
    NEAREST  = SDL_SCALEMODE_NEAREST,
    LINEAR   = SDL_SCALEMODE_LINEAR,
    PIXELART = SDL_SCALEMODE_PIXELART,
};
REGULAR_ENUM(ScaleMode);

enum class FlipMode
{
    NONE       = SDL_FLIP_NONE,
    HORIZONTAL = SDL_FLIP_HORIZONTAL,
    VERTICAL   = SDL_FLIP_VERTICAL,
};
REGULAR_ENUM(FlipMode);

using SDL_Surface = SDL_Surface;

SDL_Surface* CreateSurface(int width, int height, PixelFormat format)
{
    return SDL_CreateSurface(width, height, (SDL_PixelFormat)(format));
}

SDL_Surface* CreateSurfaceFrom(
    int width, int height, PixelFormat format, void* pixels, int pitch)
{
    return SDL_CreateSurfaceFrom(
        width, height, (SDL_PixelFormat)(format), pixels, pitch);
}

void DestroySurface(SDL_Surface* surface)
{
    SDL_DestroySurface(surface);
}

SDL_PropertiesID GetSurfaceProperties(SDL_Surface* surface)
{
    return SDL_GetSurfaceProperties(surface);
}

constexpr auto PROP_SURFACE_SDR_WHITE_POINT_FLOAT()
{
    return "SDL.surface.SDR_white_point";
}

constexpr auto PROP_SURFACE_HDR_HEADROOM_FLOAT()
{
    return "SDL.surface.HDR_headroom";
}

constexpr auto PROP_SURFACE_TONEMAP_OPERATOR_STRING()
{
    return "SDL.surface.tonemap";
}

constexpr auto PROP_SURFACE_HOTSPOT_X_NUMBER()
{
    return "SDL.surface.hotspot.x";
}

constexpr auto PROP_SURFACE_HOTSPOT_Y_NUMBER()
{
    return "SDL.surface.hotspot.y";
}

bool SetSurfaceColorspace(SDL_Surface* surface, Colorspace colorspace)
{
    return SDL_SetSurfaceColorspace(surface, (SDL_Colorspace)(colorspace));
}

SDL_Colorspace GetSurfaceColorspace(SDL_Surface* surface)
{
    return SDL_GetSurfaceColorspace(surface);
}

SDL_Palette* CreateSurfacePalette(SDL_Surface* surface)
{
    return SDL_CreateSurfacePalette(surface);
}

bool SetSurfacePalette(SDL_Surface* surface, SDL_Palette* palette)
{
    return SDL_SetSurfacePalette(surface, palette);
}

SDL_Palette* GetSurfacePalette(SDL_Surface* surface)
{
    return SDL_GetSurfacePalette(surface);
}

bool AddSurfaceAlternateImage(SDL_Surface* surface, SDL_Surface* image)
{
    return SDL_AddSurfaceAlternateImage(surface, image);
}

bool SurfaceHasAlternateImages(SDL_Surface* surface)
{
    return SDL_SurfaceHasAlternateImages(surface);
}

void RemoveSurfaceAlternateImages(SDL_Surface* surface)
{
    SDL_RemoveSurfaceAlternateImages(surface);
}

bool LockSurface(SDL_Surface* surface)
{
    return SDL_LockSurface(surface);
}

void UnlockSurface(SDL_Surface* surface)
{
    SDL_UnlockSurface(surface);
}

SDL_Surface* LoadBMP_IO(SDL_IOStream* src, bool closeio)
{
    return SDL_LoadBMP_IO(src, closeio);
}

SDL_Surface* LoadBMP(const char* file)
{
    return SDL_LoadBMP(file);
}

bool SaveBMP_IO(SDL_Surface* surface, SDL_IOStream* dst, bool closeio)
{
    return SDL_SaveBMP_IO(surface, dst, closeio);
}

bool SaveBMP(SDL_Surface* surface, const char* file)
{
    return SDL_SaveBMP(surface, file);
}

bool SetSurfaceRLE(SDL_Surface* surface, bool enabled)
{
    return SDL_SetSurfaceRLE(surface, enabled);
}

bool SurfaceHasRLE(SDL_Surface* surface)
{
    return SDL_SurfaceHasRLE(surface);
}

bool SetSurfaceColorKey(SDL_Surface* surface, bool enabled, Uint32 key)
{
    return SDL_SetSurfaceColorKey(surface, enabled, key);
}

bool SurfaceHasColorKey(SDL_Surface* surface)
{
    return SDL_SurfaceHasColorKey(surface);
}

bool GetSurfaceColorKey(SDL_Surface* surface, Uint32* key)
{
    return SDL_GetSurfaceColorKey(surface, key);
}

bool SetSurfaceColorMod(SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_SetSurfaceColorMod(surface, r, g, b);
}

bool GetSurfaceColorMod(SDL_Surface* surface, Uint8* r, Uint8* g, Uint8* b)
{
    return SDL_GetSurfaceColorMod(surface, r, g, b);
}

bool SetSurfaceAlphaMod(SDL_Surface* surface, Uint8 alpha)
{
    return SDL_SetSurfaceAlphaMod(surface, alpha);
}

bool GetSurfaceAlphaMod(SDL_Surface* surface, Uint8* alpha)
{
    return SDL_GetSurfaceAlphaMod(surface, alpha);
}

bool SetSurfaceBlendMode(SDL_Surface* surface, BlendMode blendMode)
{
    return SDL_SetSurfaceBlendMode(surface, (SDL_BlendMode)(blendMode));
}

bool GetSurfaceBlendMode(SDL_Surface* surface, BlendMode* blendMode)
{
    return SDL_GetSurfaceBlendMode(surface, (SDL_BlendMode*)(blendMode));
}

bool SetSurfaceClipRect(SDL_Surface* surface, const SDL_Rect* rect)
{
    return SDL_SetSurfaceClipRect(surface, rect);
}

bool GetSurfaceClipRect(SDL_Surface* surface, SDL_Rect* rect)
{
    return SDL_GetSurfaceClipRect(surface, rect);
}

bool FlipSurface(SDL_Surface* surface, FlipMode flip)
{
    return SDL_FlipSurface(surface, (SDL_FlipMode)(flip));
}

SDL_Surface* DuplicateSurface(SDL_Surface* surface)
{
    return SDL_DuplicateSurface(surface);
}

SDL_Surface* ScaleSurface(SDL_Surface* surface,
                          int          width,
                          int          height,
                          ScaleMode    scaleMode)
{
    return SDL_ScaleSurface(surface, width, height, (SDL_ScaleMode)(scaleMode));
}

SDL_Surface* ConvertSurface(SDL_Surface* surface, PixelFormat format)
{
    return SDL_ConvertSurface(surface, (SDL_PixelFormat)(format));
}

SDL_Surface* ConvertSurfaceAndColorspace(SDL_Surface*     surface,
                                         PixelFormat      format,
                                         SDL_Palette*     palette,
                                         Colorspace       colorspace,
                                         SDL_PropertiesID props)
{
    return SDL_ConvertSurfaceAndColorspace(surface,
                                           (SDL_PixelFormat)(format),
                                           palette,
                                           (SDL_Colorspace)(colorspace),
                                           props);
}

bool ConvertPixels(int         width,
                   int         height,
                   PixelFormat src_format,
                   const void* src,
                   int         src_pitch,
                   PixelFormat dst_format,
                   void*       dst,
                   int         dst_pitch)
{
    return SDL_ConvertPixels(width,
                             height,
                             (SDL_PixelFormat)(src_format),
                             src,
                             src_pitch,
                             (SDL_PixelFormat)(dst_format),
                             dst,
                             dst_pitch);
}

bool ConvertPixelsAndColorspace(int              width,
                                int              height,
                                PixelFormat      src_format,
                                Colorspace       src_colorspace,
                                SDL_PropertiesID src_properties,
                                const void*      src,
                                int              src_pitch,
                                PixelFormat      dst_format,
                                Colorspace       dst_colorspace,
                                SDL_PropertiesID dst_properties,
                                void*            dst,
                                int              dst_pitch)
{
    return SDL_ConvertPixelsAndColorspace(width,
                                          height,
                                          (SDL_PixelFormat)(src_format),
                                          (SDL_Colorspace)(src_colorspace),
                                          src_properties,
                                          src,
                                          src_pitch,
                                          (SDL_PixelFormat)(dst_format),
                                          (SDL_Colorspace)(dst_colorspace),
                                          dst_properties,
                                          dst,
                                          dst_pitch);
}

bool PremultiplyAlpha(int         width,
                      int         height,
                      PixelFormat src_format,
                      const void* src,
                      int         src_pitch,
                      PixelFormat dst_format,
                      void*       dst,
                      int         dst_pitch,
                      bool        linear)
{
    return SDL_PremultiplyAlpha(width,
                                height,
                                (SDL_PixelFormat)(src_format),
                                src,
                                src_pitch,
                                (SDL_PixelFormat)(dst_format),
                                dst,
                                dst_pitch,
                                linear);
}

bool PremultiplySurfaceAlpha(SDL_Surface* surface, bool linear)
{
    return SDL_PremultiplySurfaceAlpha(surface, linear);
}

bool ClearSurface(SDL_Surface* surface, float r, float g, float b, float a)
{
    return SDL_ClearSurface(surface, r, g, b, a);
}

bool FillSurfaceRect(SDL_Surface* dst, const SDL_Rect* rect, Uint32 color)
{
    return SDL_FillSurfaceRect(dst, rect, color);
}

bool FillSurfaceRects(SDL_Surface*    dst,
                      const SDL_Rect* rects,
                      int             count,
                      Uint32          color)
{
    return SDL_FillSurfaceRects(dst, rects, count, color);
}

bool BlitSurface(SDL_Surface*    src,
                 const SDL_Rect* srcrect,
                 SDL_Surface*    dst,
                 const SDL_Rect* dstrect)
{
    return SDL_BlitSurface(src, srcrect, dst, dstrect);
}

bool BlitSurfaceUnchecked(SDL_Surface*    src,
                          const SDL_Rect* srcrect,
                          SDL_Surface*    dst,
                          const SDL_Rect* dstrect)
{
    return SDL_BlitSurfaceUnchecked(src, srcrect, dst, dstrect);
}

bool BlitSurfaceScaled(SDL_Surface*    src,
                       const SDL_Rect* srcrect,
                       SDL_Surface*    dst,
                       const SDL_Rect* dstrect,
                       ScaleMode       scaleMode)
{
    return SDL_BlitSurfaceScaled(
        src, srcrect, dst, dstrect, (SDL_ScaleMode)(scaleMode));
}

bool BlitSurfaceUncheckedScaled(SDL_Surface*    src,
                                const SDL_Rect* srcrect,
                                SDL_Surface*    dst,
                                const SDL_Rect* dstrect,
                                ScaleMode       scaleMode)
{
    return SDL_BlitSurfaceUncheckedScaled(
        src, srcrect, dst, dstrect, (SDL_ScaleMode)(scaleMode));
}

bool StretchSurface(SDL_Surface*    src,
                    const SDL_Rect* srcrect,
                    SDL_Surface*    dst,
                    const SDL_Rect* dstrect,
                    ScaleMode       scaleMode)
{
    return SDL_StretchSurface(
        src, srcrect, dst, dstrect, (SDL_ScaleMode)(scaleMode));
}

bool BlitSurfaceTiled(SDL_Surface*    src,
                      const SDL_Rect* srcrect,
                      SDL_Surface*    dst,
                      const SDL_Rect* dstrect)
{
    return SDL_BlitSurfaceTiled(src, srcrect, dst, dstrect);
}

bool BlitSurfaceTiledWithScale(SDL_Surface*    src,
                               const SDL_Rect* srcrect,
                               float           scale,
                               ScaleMode       scaleMode,
                               SDL_Surface*    dst,
                               const SDL_Rect* dstrect)
{
    return SDL_BlitSurfaceTiledWithScale(
        src, srcrect, scale, (SDL_ScaleMode)(scaleMode), dst, dstrect);
}

bool BlitSurface9Grid(SDL_Surface*    src,
                      const SDL_Rect* srcrect,
                      int             left_width,
                      int             right_width,
                      int             top_height,
                      int             bottom_height,
                      float           scale,
                      ScaleMode       scaleMode,
                      SDL_Surface*    dst,
                      const SDL_Rect* dstrect)
{
    return SDL_BlitSurface9Grid(src,
                                srcrect,
                                left_width,
                                right_width,
                                top_height,
                                bottom_height,
                                scale,
                                (SDL_ScaleMode)(scaleMode),
                                dst,
                                dstrect);
}

Uint32 MapSurfaceRGB(SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_MapSurfaceRGB(surface, r, g, b);
}

Uint32 MapSurfaceRGBA(SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_MapSurfaceRGBA(surface, r, g, b, a);
}

bool ReadSurfacePixel(
    SDL_Surface* surface, int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    return SDL_ReadSurfacePixel(surface, x, y, r, g, b, a);
}

bool ReadSurfacePixelFloat(
    SDL_Surface* surface, int x, int y, float* r, float* g, float* b, float* a)
{
    return SDL_ReadSurfacePixelFloat(surface, x, y, r, g, b, a);
}

bool WriteSurfacePixel(
    SDL_Surface* surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_WriteSurfacePixel(surface, x, y, r, g, b, a);
}

bool WriteSurfacePixelFloat(
    SDL_Surface* surface, int x, int y, float r, float g, float b, float a)
{
    return SDL_WriteSurfacePixelFloat(surface, x, y, r, g, b, a);
}

using CameraID = Uint32;

using SDL_Camera = SDL_Camera;

using CameraSpec = SDL_CameraSpec;

enum class CameraPosition
{
    POSITION_UNKNOWN      = SDL_CAMERA_POSITION_UNKNOWN,
    POSITION_FRONT_FACING = SDL_CAMERA_POSITION_FRONT_FACING,
    POSITION_BACK_FACING  = SDL_CAMERA_POSITION_BACK_FACING,
};
REGULAR_ENUM(CameraPosition);

int GetNumCameraDrivers(void)
{
    return SDL_GetNumCameraDrivers();
}

const char* GetCameraDriver(int index)
{
    return SDL_GetCameraDriver(index);
}

const char* GetCurrentCameraDriver(void)
{
    return SDL_GetCurrentCameraDriver();
}

SDL_CameraID* GetCameras(int* count)
{
    return SDL_GetCameras(count);
}

const char* GetCameraName(SDL_CameraID instance_id)
{
    return SDL_GetCameraName(instance_id);
}

SDL_CameraPosition GetCameraPosition(SDL_CameraID instance_id)
{
    return SDL_GetCameraPosition(instance_id);
}

SDL_Camera* OpenCamera(SDL_CameraID instance_id, const SDL_CameraSpec* spec)
{
    return SDL_OpenCamera(instance_id, spec);
}

int GetCameraPermissionState(SDL_Camera* camera)
{
    return SDL_GetCameraPermissionState(camera);
}

SDL_CameraID GetCameraID(SDL_Camera* camera)
{
    return SDL_GetCameraID(camera);
}

SDL_PropertiesID GetCameraProperties(SDL_Camera* camera)
{
    return SDL_GetCameraProperties(camera);
}

bool GetCameraFormat(SDL_Camera* camera, SDL_CameraSpec* spec)
{
    return SDL_GetCameraFormat(camera, spec);
}

SDL_Surface* AcquireCameraFrame(SDL_Camera* camera, Uint64* timestampNS)
{
    return SDL_AcquireCameraFrame(camera, timestampNS);
}

void ReleaseCameraFrame(SDL_Camera* camera, SDL_Surface* frame)
{
    SDL_ReleaseCameraFrame(camera, frame);
}

void CloseCamera(SDL_Camera* camera)
{
    SDL_CloseCamera(camera);
}

bool SetClipboardText(const char* text)
{
    return SDL_SetClipboardText(text);
}

char* GetClipboardText(void)
{
    return SDL_GetClipboardText();
}

bool HasClipboardText(void)
{
    return SDL_HasClipboardText();
}

bool SetPrimarySelectionText(const char* text)
{
    return SDL_SetPrimarySelectionText(text);
}

char* GetPrimarySelectionText(void)
{
    return SDL_GetPrimarySelectionText();
}

bool HasPrimarySelectionText(void)
{
    return SDL_HasPrimarySelectionText();
}

bool SetClipboardData(SDL_ClipboardDataCallback    callback,
                      SDL_ClipboardCleanupCallback cleanup,
                      void*                        userdata,
                      const char**                 mime_types,
                      size_t                       num_mime_types)
{
    return SDL_SetClipboardData(
        callback, cleanup, userdata, mime_types, num_mime_types);
}

bool ClearClipboardData(void)
{
    return SDL_ClearClipboardData();
}

void* GetClipboardData(const char* mime_type, size_t* size)
{
    return SDL_GetClipboardData(mime_type, size);
}

bool HasClipboardData(const char* mime_type)
{
    return SDL_HasClipboardData(mime_type);
}

constexpr auto CACHELINE_SIZE()
{
    return 128;
}

int GetNumLogicalCPUCores(void)
{
    return SDL_GetNumLogicalCPUCores();
}

int GetCPUCacheLineSize(void)
{
    return SDL_GetCPUCacheLineSize();
}

bool HasAltiVec(void)
{
    return SDL_HasAltiVec();
}

bool HasMMX(void)
{
    return SDL_HasMMX();
}

bool HasSSE(void)
{
    return SDL_HasSSE();
}

bool HasSSE2(void)
{
    return SDL_HasSSE2();
}

bool HasSSE3(void)
{
    return SDL_HasSSE3();
}

bool HasSSE41(void)
{
    return SDL_HasSSE41();
}

bool HasSSE42(void)
{
    return SDL_HasSSE42();
}

bool HasAVX(void)
{
    return SDL_HasAVX();
}

bool HasAVX2(void)
{
    return SDL_HasAVX2();
}

bool HasAVX512F(void)
{
    return SDL_HasAVX512F();
}

bool HasARMSIMD(void)
{
    return SDL_HasARMSIMD();
}

bool HasNEON(void)
{
    return SDL_HasNEON();
}

bool HasLSX(void)
{
    return SDL_HasLSX();
}

bool HasLASX(void)
{
    return SDL_HasLASX();
}

int GetSystemRAM(void)
{
    return SDL_GetSystemRAM();
}

size_t GetSIMDAlignment(void)
{
    return SDL_GetSIMDAlignment();
}

using DisplayID = Uint32;

using WindowID = Uint32;

constexpr auto PROP_GLOBAL_VIDEO_WAYLAND_WL_DISPLAY_POINTER()
{
    return "SDL.video.wayland.wl_display";
}

enum class SystemTheme
{
    THEME_UNKNOWN = SDL_SYSTEM_THEME_UNKNOWN,
    THEME_LIGHT   = SDL_SYSTEM_THEME_LIGHT,
    THEME_DARK    = SDL_SYSTEM_THEME_DARK,
};
REGULAR_ENUM(SystemTheme);

using SDL_DisplayModeData = SDL_DisplayModeData;

using DisplayMode = SDL_DisplayMode;

enum class DisplayOrientation
{
    ORIENTATION_UNKNOWN           = SDL_ORIENTATION_UNKNOWN,
    ORIENTATION_LANDSCAPE         = SDL_ORIENTATION_LANDSCAPE,
    ORIENTATION_LANDSCAPE_FLIPPED = SDL_ORIENTATION_LANDSCAPE_FLIPPED,
    ORIENTATION_PORTRAIT          = SDL_ORIENTATION_PORTRAIT,
    ORIENTATION_PORTRAIT_FLIPPED  = SDL_ORIENTATION_PORTRAIT_FLIPPED,
};
REGULAR_ENUM(DisplayOrientation);

using SDL_Window = SDL_Window;

enum class WindowFlags : Uint64
{
    FULLSCREEN          = SDL_WINDOW_FULLSCREEN,
    OPENGL              = SDL_WINDOW_OPENGL,
    OCCLUDED            = SDL_WINDOW_OCCLUDED,
    HIDDEN              = SDL_WINDOW_HIDDEN,
    BORDERLESS          = SDL_WINDOW_BORDERLESS,
    RESIZABLE           = SDL_WINDOW_RESIZABLE,
    MINIMIZED           = SDL_WINDOW_MINIMIZED,
    MAXIMIZED           = SDL_WINDOW_MAXIMIZED,
    MOUSE_GRABBED       = SDL_WINDOW_MOUSE_GRABBED,
    INPUT_FOCUS         = SDL_WINDOW_INPUT_FOCUS,
    MOUSE_FOCUS         = SDL_WINDOW_MOUSE_FOCUS,
    EXTERNAL            = SDL_WINDOW_EXTERNAL,
    MODAL               = SDL_WINDOW_MODAL,
    HIGH_PIXEL_DENSITY  = SDL_WINDOW_HIGH_PIXEL_DENSITY,
    MOUSE_CAPTURE       = SDL_WINDOW_MOUSE_CAPTURE,
    MOUSE_RELATIVE_MODE = SDL_WINDOW_MOUSE_RELATIVE_MODE,
    ALWAYS_ON_TOP       = SDL_WINDOW_ALWAYS_ON_TOP,
    UTILITY             = SDL_WINDOW_UTILITY,
    TOOLTIP             = SDL_WINDOW_TOOLTIP,
    POPUP_MENU          = SDL_WINDOW_POPUP_MENU,
    KEYBOARD_GRABBED    = SDL_WINDOW_KEYBOARD_GRABBED,
    VULKAN              = SDL_WINDOW_VULKAN,
    METAL               = SDL_WINDOW_METAL,
    TRANSPARENT         = SDL_WINDOW_TRANSPARENT,
    NOT_FOCUSABLE       = SDL_WINDOW_NOT_FOCUSABLE,
};
BITFLAG_ENUM(WindowFlags);

constexpr auto WINDOWPOS_UNDEFINED_MASK()
{
    return 0x1FFF0000u;
}

constexpr auto WINDOWPOS_UNDEFINED()
{
    return SDL_WINDOWPOS_UNDEFINED_DISPLAY(0);
}

constexpr auto WINDOWPOS_CENTERED_MASK()
{
    return 0x2FFF0000u;
}

constexpr auto WINDOWPOS_CENTERED()
{
    return SDL_WINDOWPOS_CENTERED_DISPLAY(0);
}

enum class FlashOperation
{
    CANCEL        = SDL_FLASH_CANCEL,
    BRIEFLY       = SDL_FLASH_BRIEFLY,
    UNTIL_FOCUSED = SDL_FLASH_UNTIL_FOCUSED,
};
REGULAR_ENUM(FlashOperation);

enum class ProgressState
{
    STATE_INVALID       = SDL_PROGRESS_STATE_INVALID,
    STATE_NONE          = SDL_PROGRESS_STATE_NONE,
    STATE_INDETERMINATE = SDL_PROGRESS_STATE_INDETERMINATE,
    STATE_NORMAL        = SDL_PROGRESS_STATE_NORMAL,
    STATE_PAUSED        = SDL_PROGRESS_STATE_PAUSED,
    STATE_ERROR         = SDL_PROGRESS_STATE_ERROR,
};
REGULAR_ENUM(ProgressState);

using SDL_GLContext = SDL_GLContext;

using EGLDisplay = void;

using EGLConfig = void;

using EGLSurface = void;

using EGLAttrib = intptr_t;

using EGLint = int;

enum class GLAttr
{
    RED_SIZE                   = SDL_GL_RED_SIZE,
    GREEN_SIZE                 = SDL_GL_GREEN_SIZE,
    BLUE_SIZE                  = SDL_GL_BLUE_SIZE,
    ALPHA_SIZE                 = SDL_GL_ALPHA_SIZE,
    BUFFER_SIZE                = SDL_GL_BUFFER_SIZE,
    DOUBLEBUFFER               = SDL_GL_DOUBLEBUFFER,
    DEPTH_SIZE                 = SDL_GL_DEPTH_SIZE,
    STENCIL_SIZE               = SDL_GL_STENCIL_SIZE,
    ACCUM_RED_SIZE             = SDL_GL_ACCUM_RED_SIZE,
    ACCUM_GREEN_SIZE           = SDL_GL_ACCUM_GREEN_SIZE,
    ACCUM_BLUE_SIZE            = SDL_GL_ACCUM_BLUE_SIZE,
    ACCUM_ALPHA_SIZE           = SDL_GL_ACCUM_ALPHA_SIZE,
    STEREO                     = SDL_GL_STEREO,
    MULTISAMPLEBUFFERS         = SDL_GL_MULTISAMPLEBUFFERS,
    MULTISAMPLESAMPLES         = SDL_GL_MULTISAMPLESAMPLES,
    ACCELERATED_VISUAL         = SDL_GL_ACCELERATED_VISUAL,
    RETAINED_BACKING           = SDL_GL_RETAINED_BACKING,
    CONTEXT_MAJOR_VERSION      = SDL_GL_CONTEXT_MAJOR_VERSION,
    CONTEXT_MINOR_VERSION      = SDL_GL_CONTEXT_MINOR_VERSION,
    CONTEXT_FLAGS              = SDL_GL_CONTEXT_FLAGS,
    CONTEXT_PROFILE_MASK       = SDL_GL_CONTEXT_PROFILE_MASK,
    SHARE_WITH_CURRENT_CONTEXT = SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
    FRAMEBUFFER_SRGB_CAPABLE   = SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
    CONTEXT_RELEASE_BEHAVIOR   = SDL_GL_CONTEXT_RELEASE_BEHAVIOR,
    CONTEXT_RESET_NOTIFICATION = SDL_GL_CONTEXT_RESET_NOTIFICATION,
    CONTEXT_NO_ERROR           = SDL_GL_CONTEXT_NO_ERROR,
    FLOATBUFFERS               = SDL_GL_FLOATBUFFERS,
    EGL_PLATFORM               = SDL_GL_EGL_PLATFORM,
};
REGULAR_ENUM(GLAttr);

enum class GLProfile : Uint32
{
    CONTEXT_PROFILE_CORE          = SDL_GL_CONTEXT_PROFILE_CORE,
    CONTEXT_PROFILE_COMPATIBILITY = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY,
    CONTEXT_PROFILE_ES            = SDL_GL_CONTEXT_PROFILE_ES,
};
BITFLAG_ENUM(GLProfile);

enum class GLContextFlag : Uint32
{
    CONTEXT_DEBUG_FLAG              = SDL_GL_CONTEXT_DEBUG_FLAG,
    CONTEXT_FORWARD_COMPATIBLE_FLAG = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG,
    CONTEXT_ROBUST_ACCESS_FLAG      = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,
    CONTEXT_RESET_ISOLATION_FLAG    = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG,
};
BITFLAG_ENUM(GLContextFlag);

enum class GLContextReleaseFlag : Uint32
{
    CONTEXT_RELEASE_BEHAVIOR_NONE  = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE,
    CONTEXT_RELEASE_BEHAVIOR_FLUSH = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH,
};
BITFLAG_ENUM(GLContextReleaseFlag);

enum class GLContextResetNotification : Uint32
{
    CONTEXT_RESET_NO_NOTIFICATION = SDL_GL_CONTEXT_RESET_NO_NOTIFICATION,
    CONTEXT_RESET_LOSE_CONTEXT    = SDL_GL_CONTEXT_RESET_LOSE_CONTEXT,
};
BITFLAG_ENUM(GLContextResetNotification);

int GetNumVideoDrivers(void)
{
    return SDL_GetNumVideoDrivers();
}

const char* GetVideoDriver(int index)
{
    return SDL_GetVideoDriver(index);
}

const char* GetCurrentVideoDriver(void)
{
    return SDL_GetCurrentVideoDriver();
}

SDL_SystemTheme GetSystemTheme(void)
{
    return SDL_GetSystemTheme();
}

SDL_DisplayID* GetDisplays(int* count)
{
    return SDL_GetDisplays(count);
}

SDL_DisplayID GetPrimaryDisplay(void)
{
    return SDL_GetPrimaryDisplay();
}

SDL_PropertiesID GetDisplayProperties(SDL_DisplayID displayID)
{
    return SDL_GetDisplayProperties(displayID);
}

constexpr auto PROP_DISPLAY_HDR_ENABLED_BOOLEAN()
{
    return "SDL.display.HDR_enabled";
}

constexpr auto PROP_DISPLAY_KMSDRM_PANEL_ORIENTATION_NUMBER()
{
    return "SDL.display.KMSDRM.panel_orientation";
}

constexpr auto PROP_DISPLAY_WAYLAND_WL_OUTPUT_POINTER()
{
    return "SDL.display.wayland.wl_output";
}

const char* GetDisplayName(SDL_DisplayID displayID)
{
    return SDL_GetDisplayName(displayID);
}

bool GetDisplayBounds(SDL_DisplayID displayID, SDL_Rect* rect)
{
    return SDL_GetDisplayBounds(displayID, rect);
}

bool GetDisplayUsableBounds(SDL_DisplayID displayID, SDL_Rect* rect)
{
    return SDL_GetDisplayUsableBounds(displayID, rect);
}

SDL_DisplayOrientation GetNaturalDisplayOrientation(SDL_DisplayID displayID)
{
    return SDL_GetNaturalDisplayOrientation(displayID);
}

SDL_DisplayOrientation GetCurrentDisplayOrientation(SDL_DisplayID displayID)
{
    return SDL_GetCurrentDisplayOrientation(displayID);
}

float GetDisplayContentScale(SDL_DisplayID displayID)
{
    return SDL_GetDisplayContentScale(displayID);
}

bool GetClosestFullscreenDisplayMode(SDL_DisplayID displayID,
                                     int           w,
                                     int           h,
                                     float         refresh_rate,
                                     bool          include_high_density_modes,
                                     SDL_DisplayMode* closest)
{
    return SDL_GetClosestFullscreenDisplayMode(
        displayID, w, h, refresh_rate, include_high_density_modes, closest);
}

const SDL_DisplayMode* GetDesktopDisplayMode(SDL_DisplayID displayID)
{
    return SDL_GetDesktopDisplayMode(displayID);
}

const SDL_DisplayMode* GetCurrentDisplayMode(SDL_DisplayID displayID)
{
    return SDL_GetCurrentDisplayMode(displayID);
}

SDL_DisplayID GetDisplayForPoint(const SDL_Point* point)
{
    return SDL_GetDisplayForPoint(point);
}

SDL_DisplayID GetDisplayForRect(const SDL_Rect* rect)
{
    return SDL_GetDisplayForRect(rect);
}

SDL_DisplayID GetDisplayForWindow(SDL_Window* window)
{
    return SDL_GetDisplayForWindow(window);
}

float GetWindowPixelDensity(SDL_Window* window)
{
    return SDL_GetWindowPixelDensity(window);
}

float GetWindowDisplayScale(SDL_Window* window)
{
    return SDL_GetWindowDisplayScale(window);
}

bool SetWindowFullscreenMode(SDL_Window* window, const SDL_DisplayMode* mode)
{
    return SDL_SetWindowFullscreenMode(window, mode);
}

const SDL_DisplayMode* GetWindowFullscreenMode(SDL_Window* window)
{
    return SDL_GetWindowFullscreenMode(window);
}

void* GetWindowICCProfile(SDL_Window* window, size_t* size)
{
    return SDL_GetWindowICCProfile(window, size);
}

SDL_PixelFormat GetWindowPixelFormat(SDL_Window* window)
{
    return SDL_GetWindowPixelFormat(window);
}

SDL_Window* CreateWindow(const char* title, int w, int h, WindowFlags flags)
{
    return SDL_CreateWindow(title, w, h, (SDL_WindowFlags)(flags));
}

SDL_Window* CreatePopupWindow(SDL_Window* parent,
                              int         offset_x,
                              int         offset_y,
                              int         w,
                              int         h,
                              WindowFlags flags)
{
    return SDL_CreatePopupWindow(
        parent, offset_x, offset_y, w, h, (SDL_WindowFlags)(flags));
}

SDL_Window* CreateWindowWithProperties(SDL_PropertiesID props)
{
    return SDL_CreateWindowWithProperties(props);
}

constexpr auto PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN()
{
    return "SDL.window.create.always_on_top";
}

constexpr auto PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN()
{
    return "SDL.window.create.borderless";
}

constexpr auto PROP_WINDOW_CREATE_CONSTRAIN_POPUP_BOOLEAN()
{
    return "SDL.window.create.constrain_popup";
}

constexpr auto PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN()
{
    return "SDL.window.create.focusable";
}

constexpr auto PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN()
{
    return "SDL.window.create.external_graphics_context";
}

constexpr auto PROP_WINDOW_CREATE_FLAGS_NUMBER()
{
    return "SDL.window.create.flags";
}

constexpr auto PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN()
{
    return "SDL.window.create.fullscreen";
}

constexpr auto PROP_WINDOW_CREATE_HEIGHT_NUMBER()
{
    return "SDL.window.create.height";
}

constexpr auto PROP_WINDOW_CREATE_HIDDEN_BOOLEAN()
{
    return "SDL.window.create.hidden";
}

constexpr auto PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN()
{
    return "SDL.window.create.high_pixel_density";
}

constexpr auto PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN()
{
    return "SDL.window.create.maximized";
}

constexpr auto PROP_WINDOW_CREATE_MENU_BOOLEAN()
{
    return "SDL.window.create.menu";
}

constexpr auto PROP_WINDOW_CREATE_METAL_BOOLEAN()
{
    return "SDL.window.create.metal";
}

constexpr auto PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN()
{
    return "SDL.window.create.minimized";
}

constexpr auto PROP_WINDOW_CREATE_MODAL_BOOLEAN()
{
    return "SDL.window.create.modal";
}

constexpr auto PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN()
{
    return "SDL.window.create.mouse_grabbed";
}

constexpr auto PROP_WINDOW_CREATE_OPENGL_BOOLEAN()
{
    return "SDL.window.create.opengl";
}

constexpr auto PROP_WINDOW_CREATE_PARENT_POINTER()
{
    return "SDL.window.create.parent";
}

constexpr auto PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN()
{
    return "SDL.window.create.resizable";
}

constexpr auto PROP_WINDOW_CREATE_TITLE_STRING()
{
    return "SDL.window.create.title";
}

constexpr auto PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN()
{
    return "SDL.window.create.transparent";
}

constexpr auto PROP_WINDOW_CREATE_TOOLTIP_BOOLEAN()
{
    return "SDL.window.create.tooltip";
}

constexpr auto PROP_WINDOW_CREATE_UTILITY_BOOLEAN()
{
    return "SDL.window.create.utility";
}

constexpr auto PROP_WINDOW_CREATE_VULKAN_BOOLEAN()
{
    return "SDL.window.create.vulkan";
}

constexpr auto PROP_WINDOW_CREATE_WIDTH_NUMBER()
{
    return "SDL.window.create.width";
}

constexpr auto PROP_WINDOW_CREATE_X_NUMBER()
{
    return "SDL.window.create.x";
}

constexpr auto PROP_WINDOW_CREATE_Y_NUMBER()
{
    return "SDL.window.create.y";
}

constexpr auto PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER()
{
    return "SDL.window.create.cocoa.window";
}

constexpr auto PROP_WINDOW_CREATE_COCOA_VIEW_POINTER()
{
    return "SDL.window.create.cocoa.view";
}

constexpr auto PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN()
{
    return "SDL.window.create.wayland.surface_role_custom";
}

constexpr auto PROP_WINDOW_CREATE_WAYLAND_CREATE_EGL_WINDOW_BOOLEAN()
{
    return "SDL.window.create.wayland.create_egl_window";
}

constexpr auto PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER()
{
    return "SDL.window.create.wayland.wl_surface";
}

constexpr auto PROP_WINDOW_CREATE_WIN32_HWND_POINTER()
{
    return "SDL.window.create.win32.hwnd";
}

constexpr auto PROP_WINDOW_CREATE_WIN32_PIXEL_FORMAT_HWND_POINTER()
{
    return "SDL.window.create.win32.pixel_format_hwnd";
}

constexpr auto PROP_WINDOW_CREATE_X11_WINDOW_NUMBER()
{
    return "SDL.window.create.x11.window";
}

constexpr auto PROP_WINDOW_CREATE_EMSCRIPTEN_CANVAS_ID_STRING()
{
    return "SDL.window.create.emscripten.canvas_id";
}

constexpr auto PROP_WINDOW_CREATE_EMSCRIPTEN_KEYBOARD_ELEMENT_STRING()
{
    return "SDL.window.create.emscripten.keyboard_element";
}

SDL_WindowID GetWindowID(SDL_Window* window)
{
    return SDL_GetWindowID(window);
}

SDL_Window* GetWindowFromID(SDL_WindowID id)
{
    return SDL_GetWindowFromID(id);
}

SDL_Window* GetWindowParent(SDL_Window* window)
{
    return SDL_GetWindowParent(window);
}

SDL_PropertiesID GetWindowProperties(SDL_Window* window)
{
    return SDL_GetWindowProperties(window);
}

constexpr auto PROP_WINDOW_SHAPE_POINTER()
{
    return "SDL.window.shape";
}

constexpr auto PROP_WINDOW_HDR_ENABLED_BOOLEAN()
{
    return "SDL.window.HDR_enabled";
}

constexpr auto PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT()
{
    return "SDL.window.SDR_white_level";
}

constexpr auto PROP_WINDOW_HDR_HEADROOM_FLOAT()
{
    return "SDL.window.HDR_headroom";
}

constexpr auto PROP_WINDOW_ANDROID_WINDOW_POINTER()
{
    return "SDL.window.android.window";
}

constexpr auto PROP_WINDOW_ANDROID_SURFACE_POINTER()
{
    return "SDL.window.android.surface";
}

constexpr auto PROP_WINDOW_UIKIT_WINDOW_POINTER()
{
    return "SDL.window.uikit.window";
}

constexpr auto PROP_WINDOW_UIKIT_METAL_VIEW_TAG_NUMBER()
{
    return "SDL.window.uikit.metal_view_tag";
}

constexpr auto PROP_WINDOW_UIKIT_OPENGL_FRAMEBUFFER_NUMBER()
{
    return "SDL.window.uikit.opengl.framebuffer";
}

constexpr auto PROP_WINDOW_UIKIT_OPENGL_RENDERBUFFER_NUMBER()
{
    return "SDL.window.uikit.opengl.renderbuffer";
}

constexpr auto PROP_WINDOW_UIKIT_OPENGL_RESOLVE_FRAMEBUFFER_NUMBER()
{
    return "SDL.window.uikit.opengl.resolve_framebuffer";
}

constexpr auto PROP_WINDOW_KMSDRM_DEVICE_INDEX_NUMBER()
{
    return "SDL.window.kmsdrm.dev_index";
}

constexpr auto PROP_WINDOW_KMSDRM_DRM_FD_NUMBER()
{
    return "SDL.window.kmsdrm.drm_fd";
}

constexpr auto PROP_WINDOW_KMSDRM_GBM_DEVICE_POINTER()
{
    return "SDL.window.kmsdrm.gbm_dev";
}

constexpr auto PROP_WINDOW_COCOA_WINDOW_POINTER()
{
    return "SDL.window.cocoa.window";
}

constexpr auto PROP_WINDOW_COCOA_METAL_VIEW_TAG_NUMBER()
{
    return "SDL.window.cocoa.metal_view_tag";
}

constexpr auto PROP_WINDOW_OPENVR_OVERLAY_ID_NUMBER()
{
    return "SDL.window.openvr.overlay_id";
}

constexpr auto PROP_WINDOW_VIVANTE_DISPLAY_POINTER()
{
    return "SDL.window.vivante.display";
}

constexpr auto PROP_WINDOW_VIVANTE_WINDOW_POINTER()
{
    return "SDL.window.vivante.window";
}

constexpr auto PROP_WINDOW_VIVANTE_SURFACE_POINTER()
{
    return "SDL.window.vivante.surface";
}

constexpr auto PROP_WINDOW_WIN32_HWND_POINTER()
{
    return "SDL.window.win32.hwnd";
}

constexpr auto PROP_WINDOW_WIN32_HDC_POINTER()
{
    return "SDL.window.win32.hdc";
}

constexpr auto PROP_WINDOW_WIN32_INSTANCE_POINTER()
{
    return "SDL.window.win32.instance";
}

constexpr auto PROP_WINDOW_WAYLAND_DISPLAY_POINTER()
{
    return "SDL.window.wayland.display";
}

constexpr auto PROP_WINDOW_WAYLAND_SURFACE_POINTER()
{
    return "SDL.window.wayland.surface";
}

constexpr auto PROP_WINDOW_WAYLAND_VIEWPORT_POINTER()
{
    return "SDL.window.wayland.viewport";
}

constexpr auto PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER()
{
    return "SDL.window.wayland.egl_window";
}

constexpr auto PROP_WINDOW_WAYLAND_XDG_SURFACE_POINTER()
{
    return "SDL.window.wayland.xdg_surface";
}

constexpr auto PROP_WINDOW_WAYLAND_XDG_TOPLEVEL_POINTER()
{
    return "SDL.window.wayland.xdg_toplevel";
}

constexpr auto PROP_WINDOW_WAYLAND_XDG_TOPLEVEL_EXPORT_HANDLE_STRING()
{
    return "SDL.window.wayland.xdg_toplevel_export_handle";
}

constexpr auto PROP_WINDOW_WAYLAND_XDG_POPUP_POINTER()
{
    return "SDL.window.wayland.xdg_popup";
}

constexpr auto PROP_WINDOW_WAYLAND_XDG_POSITIONER_POINTER()
{
    return "SDL.window.wayland.xdg_positioner";
}

constexpr auto PROP_WINDOW_X11_DISPLAY_POINTER()
{
    return "SDL.window.x11.display";
}

constexpr auto PROP_WINDOW_X11_SCREEN_NUMBER()
{
    return "SDL.window.x11.screen";
}

constexpr auto PROP_WINDOW_X11_WINDOW_NUMBER()
{
    return "SDL.window.x11.window";
}

constexpr auto PROP_WINDOW_EMSCRIPTEN_CANVAS_ID_STRING()
{
    return "SDL.window.emscripten.canvas_id";
}

constexpr auto PROP_WINDOW_EMSCRIPTEN_KEYBOARD_ELEMENT_STRING()
{
    return "SDL.window.emscripten.keyboard_element";
}

SDL_WindowFlags GetWindowFlags(SDL_Window* window)
{
    return SDL_GetWindowFlags(window);
}

bool SetWindowTitle(SDL_Window* window, const char* title)
{
    return SDL_SetWindowTitle(window, title);
}

const char* GetWindowTitle(SDL_Window* window)
{
    return SDL_GetWindowTitle(window);
}

bool SetWindowIcon(SDL_Window* window, SDL_Surface* icon)
{
    return SDL_SetWindowIcon(window, icon);
}

bool SetWindowPosition(SDL_Window* window, int x, int y)
{
    return SDL_SetWindowPosition(window, x, y);
}

bool GetWindowPosition(SDL_Window* window, int* x, int* y)
{
    return SDL_GetWindowPosition(window, x, y);
}

bool SetWindowSize(SDL_Window* window, int w, int h)
{
    return SDL_SetWindowSize(window, w, h);
}

bool GetWindowSize(SDL_Window* window, int* w, int* h)
{
    return SDL_GetWindowSize(window, w, h);
}

bool GetWindowSafeArea(SDL_Window* window, SDL_Rect* rect)
{
    return SDL_GetWindowSafeArea(window, rect);
}

bool SetWindowAspectRatio(SDL_Window* window,
                          float       min_aspect,
                          float       max_aspect)
{
    return SDL_SetWindowAspectRatio(window, min_aspect, max_aspect);
}

bool GetWindowAspectRatio(SDL_Window* window,
                          float*      min_aspect,
                          float*      max_aspect)
{
    return SDL_GetWindowAspectRatio(window, min_aspect, max_aspect);
}

bool GetWindowBordersSize(
    SDL_Window* window, int* top, int* left, int* bottom, int* right)
{
    return SDL_GetWindowBordersSize(window, top, left, bottom, right);
}

bool GetWindowSizeInPixels(SDL_Window* window, int* w, int* h)
{
    return SDL_GetWindowSizeInPixels(window, w, h);
}

bool SetWindowMinimumSize(SDL_Window* window, int min_w, int min_h)
{
    return SDL_SetWindowMinimumSize(window, min_w, min_h);
}

bool GetWindowMinimumSize(SDL_Window* window, int* w, int* h)
{
    return SDL_GetWindowMinimumSize(window, w, h);
}

bool SetWindowMaximumSize(SDL_Window* window, int max_w, int max_h)
{
    return SDL_SetWindowMaximumSize(window, max_w, max_h);
}

bool GetWindowMaximumSize(SDL_Window* window, int* w, int* h)
{
    return SDL_GetWindowMaximumSize(window, w, h);
}

bool SetWindowBordered(SDL_Window* window, bool bordered)
{
    return SDL_SetWindowBordered(window, bordered);
}

bool SetWindowResizable(SDL_Window* window, bool resizable)
{
    return SDL_SetWindowResizable(window, resizable);
}

bool SetWindowAlwaysOnTop(SDL_Window* window, bool on_top)
{
    return SDL_SetWindowAlwaysOnTop(window, on_top);
}

bool ShowWindow(SDL_Window* window)
{
    return SDL_ShowWindow(window);
}

bool HideWindow(SDL_Window* window)
{
    return SDL_HideWindow(window);
}

bool RaiseWindow(SDL_Window* window)
{
    return SDL_RaiseWindow(window);
}

bool MaximizeWindow(SDL_Window* window)
{
    return SDL_MaximizeWindow(window);
}

bool MinimizeWindow(SDL_Window* window)
{
    return SDL_MinimizeWindow(window);
}

bool RestoreWindow(SDL_Window* window)
{
    return SDL_RestoreWindow(window);
}

bool SetWindowFullscreen(SDL_Window* window, bool fullscreen)
{
    return SDL_SetWindowFullscreen(window, fullscreen);
}

bool SyncWindow(SDL_Window* window)
{
    return SDL_SyncWindow(window);
}

bool WindowHasSurface(SDL_Window* window)
{
    return SDL_WindowHasSurface(window);
}

SDL_Surface* GetWindowSurface(SDL_Window* window)
{
    return SDL_GetWindowSurface(window);
}

bool SetWindowSurfaceVSync(SDL_Window* window, int vsync)
{
    return SDL_SetWindowSurfaceVSync(window, vsync);
}

constexpr auto WINDOW_SURFACE_VSYNC_DISABLED()
{
    return 0;
}

constexpr auto WINDOW_SURFACE_VSYNC_ADAPTIVE()
{
    return (-1);
}

bool GetWindowSurfaceVSync(SDL_Window* window, int* vsync)
{
    return SDL_GetWindowSurfaceVSync(window, vsync);
}

bool UpdateWindowSurface(SDL_Window* window)
{
    return SDL_UpdateWindowSurface(window);
}

bool UpdateWindowSurfaceRects(SDL_Window*     window,
                              const SDL_Rect* rects,
                              int             numrects)
{
    return SDL_UpdateWindowSurfaceRects(window, rects, numrects);
}

bool DestroyWindowSurface(SDL_Window* window)
{
    return SDL_DestroyWindowSurface(window);
}

bool SetWindowKeyboardGrab(SDL_Window* window, bool grabbed)
{
    return SDL_SetWindowKeyboardGrab(window, grabbed);
}

bool SetWindowMouseGrab(SDL_Window* window, bool grabbed)
{
    return SDL_SetWindowMouseGrab(window, grabbed);
}

bool GetWindowKeyboardGrab(SDL_Window* window)
{
    return SDL_GetWindowKeyboardGrab(window);
}

bool GetWindowMouseGrab(SDL_Window* window)
{
    return SDL_GetWindowMouseGrab(window);
}

SDL_Window* GetGrabbedWindow(void)
{
    return SDL_GetGrabbedWindow();
}

bool SetWindowMouseRect(SDL_Window* window, const SDL_Rect* rect)
{
    return SDL_SetWindowMouseRect(window, rect);
}

const SDL_Rect* GetWindowMouseRect(SDL_Window* window)
{
    return SDL_GetWindowMouseRect(window);
}

bool SetWindowOpacity(SDL_Window* window, float opacity)
{
    return SDL_SetWindowOpacity(window, opacity);
}

float GetWindowOpacity(SDL_Window* window)
{
    return SDL_GetWindowOpacity(window);
}

bool SetWindowParent(SDL_Window* window, SDL_Window* parent)
{
    return SDL_SetWindowParent(window, parent);
}

bool SetWindowModal(SDL_Window* window, bool modal)
{
    return SDL_SetWindowModal(window, modal);
}

bool SetWindowFocusable(SDL_Window* window, bool focusable)
{
    return SDL_SetWindowFocusable(window, focusable);
}

bool ShowWindowSystemMenu(SDL_Window* window, int x, int y)
{
    return SDL_ShowWindowSystemMenu(window, x, y);
}

enum class HitTestResult
{
    NORMAL             = SDL_HITTEST_NORMAL,
    DRAGGABLE          = SDL_HITTEST_DRAGGABLE,
    RESIZE_TOPLEFT     = SDL_HITTEST_RESIZE_TOPLEFT,
    RESIZE_TOP         = SDL_HITTEST_RESIZE_TOP,
    RESIZE_TOPRIGHT    = SDL_HITTEST_RESIZE_TOPRIGHT,
    RESIZE_RIGHT       = SDL_HITTEST_RESIZE_RIGHT,
    RESIZE_BOTTOMRIGHT = SDL_HITTEST_RESIZE_BOTTOMRIGHT,
    RESIZE_BOTTOM      = SDL_HITTEST_RESIZE_BOTTOM,
    RESIZE_BOTTOMLEFT  = SDL_HITTEST_RESIZE_BOTTOMLEFT,
    RESIZE_LEFT        = SDL_HITTEST_RESIZE_LEFT,
};
REGULAR_ENUM(HitTestResult);

bool SetWindowHitTest(SDL_Window* window,
                      SDL_HitTest callback,
                      void*       callback_data)
{
    return SDL_SetWindowHitTest(window, callback, callback_data);
}

bool SetWindowShape(SDL_Window* window, SDL_Surface* shape)
{
    return SDL_SetWindowShape(window, shape);
}

bool FlashWindow(SDL_Window* window, FlashOperation operation)
{
    return SDL_FlashWindow(window, (SDL_FlashOperation)(operation));
}

bool SetWindowProgressState(SDL_Window* window, ProgressState state)
{
    return SDL_SetWindowProgressState(window, (SDL_ProgressState)(state));
}

SDL_ProgressState GetWindowProgressState(SDL_Window* window)
{
    return SDL_GetWindowProgressState(window);
}

bool SetWindowProgressValue(SDL_Window* window, float value)
{
    return SDL_SetWindowProgressValue(window, value);
}

float GetWindowProgressValue(SDL_Window* window)
{
    return SDL_GetWindowProgressValue(window);
}

void DestroyWindow(SDL_Window* window)
{
    SDL_DestroyWindow(window);
}

bool ScreenSaverEnabled(void)
{
    return SDL_ScreenSaverEnabled();
}

bool EnableScreenSaver(void)
{
    return SDL_EnableScreenSaver();
}

bool DisableScreenSaver(void)
{
    return SDL_DisableScreenSaver();
}

bool GL_LoadLibrary(const char* path)
{
    return SDL_GL_LoadLibrary(path);
}

SDL_FunctionPointer GL_GetProcAddress(const char* proc)
{
    return SDL_GL_GetProcAddress(proc);
}

SDL_FunctionPointer EGL_GetProcAddress(const char* proc)
{
    return SDL_EGL_GetProcAddress(proc);
}

void GL_UnloadLibrary(void)
{
    SDL_GL_UnloadLibrary();
}

bool GL_ExtensionSupported(const char* extension)
{
    return SDL_GL_ExtensionSupported(extension);
}

void GL_ResetAttributes(void)
{
    SDL_GL_ResetAttributes();
}

bool GL_SetAttribute(GLAttr attr, int value)
{
    return SDL_GL_SetAttribute((SDL_GLAttr)(attr), value);
}

bool GL_GetAttribute(GLAttr attr, int* value)
{
    return SDL_GL_GetAttribute((SDL_GLAttr)(attr), value);
}

SDL_GLContext GL_CreateContext(SDL_Window* window)
{
    return SDL_GL_CreateContext(window);
}

bool GL_MakeCurrent(SDL_Window* window, SDL_GLContext context)
{
    return SDL_GL_MakeCurrent(window, context);
}

SDL_Window* GL_GetCurrentWindow(void)
{
    return SDL_GL_GetCurrentWindow();
}

SDL_GLContext GL_GetCurrentContext(void)
{
    return SDL_GL_GetCurrentContext();
}

SDL_EGLDisplay EGL_GetCurrentDisplay(void)
{
    return SDL_EGL_GetCurrentDisplay();
}

SDL_EGLConfig EGL_GetCurrentConfig(void)
{
    return SDL_EGL_GetCurrentConfig();
}

SDL_EGLSurface EGL_GetWindowSurface(SDL_Window* window)
{
    return SDL_EGL_GetWindowSurface(window);
}

void EGL_SetAttributeCallbacks(
    SDL_EGLAttribArrayCallback platformAttribCallback,
    SDL_EGLIntArrayCallback    surfaceAttribCallback,
    SDL_EGLIntArrayCallback    contextAttribCallback,
    void*                      userdata)
{
    SDL_EGL_SetAttributeCallbacks(platformAttribCallback,
                                  surfaceAttribCallback,
                                  contextAttribCallback,
                                  userdata);
}

bool GL_SetSwapInterval(int interval)
{
    return SDL_GL_SetSwapInterval(interval);
}

bool GL_GetSwapInterval(int* interval)
{
    return SDL_GL_GetSwapInterval(interval);
}

bool GL_SwapWindow(SDL_Window* window)
{
    return SDL_GL_SwapWindow(window);
}

bool GL_DestroyContext(SDL_GLContext context)
{
    return SDL_GL_DestroyContext(context);
}

using DialogFileFilter = SDL_DialogFileFilter;

void ShowOpenFileDialog(SDL_DialogFileCallback      callback,
                        void*                       userdata,
                        SDL_Window*                 window,
                        const SDL_DialogFileFilter* filters,
                        int                         nfilters,
                        const char*                 default_location,
                        bool                        allow_many)
{
    SDL_ShowOpenFileDialog(callback,
                           userdata,
                           window,
                           filters,
                           nfilters,
                           default_location,
                           allow_many);
}

void ShowSaveFileDialog(SDL_DialogFileCallback      callback,
                        void*                       userdata,
                        SDL_Window*                 window,
                        const SDL_DialogFileFilter* filters,
                        int                         nfilters,
                        const char*                 default_location)
{
    SDL_ShowSaveFileDialog(
        callback, userdata, window, filters, nfilters, default_location);
}

void ShowOpenFolderDialog(SDL_DialogFileCallback callback,
                          void*                  userdata,
                          SDL_Window*            window,
                          const char*            default_location,
                          bool                   allow_many)
{
    SDL_ShowOpenFolderDialog(
        callback, userdata, window, default_location, allow_many);
}

enum class FileDialogType
{
    OPENFILE   = SDL_FILEDIALOG_OPENFILE,
    SAVEFILE   = SDL_FILEDIALOG_SAVEFILE,
    OPENFOLDER = SDL_FILEDIALOG_OPENFOLDER,
};
REGULAR_ENUM(FileDialogType);

void ShowFileDialogWithProperties(FileDialogType         type,
                                  SDL_DialogFileCallback callback,
                                  void*                  userdata,
                                  SDL_PropertiesID       props)
{
    SDL_ShowFileDialogWithProperties(
        (SDL_FileDialogType)(type), callback, userdata, props);
}

constexpr auto PROP_FILE_DIALOG_FILTERS_POINTER()
{
    return "SDL.filedialog.filters";
}

constexpr auto PROP_FILE_DIALOG_NFILTERS_NUMBER()
{
    return "SDL.filedialog.nfilters";
}

constexpr auto PROP_FILE_DIALOG_WINDOW_POINTER()
{
    return "SDL.filedialog.window";
}

constexpr auto PROP_FILE_DIALOG_LOCATION_STRING()
{
    return "SDL.filedialog.location";
}

constexpr auto PROP_FILE_DIALOG_MANY_BOOLEAN()
{
    return "SDL.filedialog.many";
}

constexpr auto PROP_FILE_DIALOG_TITLE_STRING()
{
    return "SDL.filedialog.title";
}

constexpr auto PROP_FILE_DIALOG_ACCEPT_STRING()
{
    return "SDL.filedialog.accept";
}

constexpr auto PROP_FILE_DIALOG_CANCEL_STRING()
{
    return "SDL.filedialog.cancel";
}

using GUID = SDL_GUID;

void GUIDToString(SDL_GUID guid, char* pszGUID, int cbGUID)
{
    SDL_GUIDToString(guid, pszGUID, cbGUID);
}

SDL_GUID StringToGUID(const char* pchGUID)
{
    return SDL_StringToGUID(pchGUID);
}

enum class PowerState
{
    ERROR      = SDL_POWERSTATE_ERROR,
    UNKNOWN    = SDL_POWERSTATE_UNKNOWN,
    ON_BATTERY = SDL_POWERSTATE_ON_BATTERY,
    NO_BATTERY = SDL_POWERSTATE_NO_BATTERY,
    CHARGING   = SDL_POWERSTATE_CHARGING,
    CHARGED    = SDL_POWERSTATE_CHARGED,
};
REGULAR_ENUM(PowerState);

SDL_PowerState GetPowerInfo(int* seconds, int* percent)
{
    return SDL_GetPowerInfo(seconds, percent);
}

using SDL_Sensor = SDL_Sensor;

using SensorID = Uint32;

constexpr auto STANDARD_GRAVITY()
{
    return 9.80665f;
}

enum class SensorType
{
    INVALID = SDL_SENSOR_INVALID,
    UNKNOWN = SDL_SENSOR_UNKNOWN,
    ACCEL   = SDL_SENSOR_ACCEL,
    GYRO    = SDL_SENSOR_GYRO,
    ACCEL_L = SDL_SENSOR_ACCEL_L,
    GYRO_L  = SDL_SENSOR_GYRO_L,
    ACCEL_R = SDL_SENSOR_ACCEL_R,
    GYRO_R  = SDL_SENSOR_GYRO_R,
};
REGULAR_ENUM(SensorType);

SDL_SensorID* GetSensors(int* count)
{
    return SDL_GetSensors(count);
}

const char* GetSensorNameForID(SDL_SensorID instance_id)
{
    return SDL_GetSensorNameForID(instance_id);
}

SDL_SensorType GetSensorTypeForID(SDL_SensorID instance_id)
{
    return SDL_GetSensorTypeForID(instance_id);
}

int GetSensorNonPortableTypeForID(SDL_SensorID instance_id)
{
    return SDL_GetSensorNonPortableTypeForID(instance_id);
}

SDL_Sensor* OpenSensor(SDL_SensorID instance_id)
{
    return SDL_OpenSensor(instance_id);
}

SDL_Sensor* GetSensorFromID(SDL_SensorID instance_id)
{
    return SDL_GetSensorFromID(instance_id);
}

SDL_PropertiesID GetSensorProperties(SDL_Sensor* sensor)
{
    return SDL_GetSensorProperties(sensor);
}

const char* GetSensorName(SDL_Sensor* sensor)
{
    return SDL_GetSensorName(sensor);
}

SDL_SensorType GetSensorType(SDL_Sensor* sensor)
{
    return SDL_GetSensorType(sensor);
}

int GetSensorNonPortableType(SDL_Sensor* sensor)
{
    return SDL_GetSensorNonPortableType(sensor);
}

SDL_SensorID GetSensorID(SDL_Sensor* sensor)
{
    return SDL_GetSensorID(sensor);
}

bool GetSensorData(SDL_Sensor* sensor, float* data, int num_values)
{
    return SDL_GetSensorData(sensor, data, num_values);
}

void CloseSensor(SDL_Sensor* sensor)
{
    SDL_CloseSensor(sensor);
}

void UpdateSensors(void)
{
    SDL_UpdateSensors();
}

using SDL_Joystick = SDL_Joystick;

using JoystickID = Uint32;

enum class JoystickType
{
    TYPE_UNKNOWN      = SDL_JOYSTICK_TYPE_UNKNOWN,
    TYPE_GAMEPAD      = SDL_JOYSTICK_TYPE_GAMEPAD,
    TYPE_WHEEL        = SDL_JOYSTICK_TYPE_WHEEL,
    TYPE_ARCADE_STICK = SDL_JOYSTICK_TYPE_ARCADE_STICK,
    TYPE_FLIGHT_STICK = SDL_JOYSTICK_TYPE_FLIGHT_STICK,
    TYPE_DANCE_PAD    = SDL_JOYSTICK_TYPE_DANCE_PAD,
    TYPE_GUITAR       = SDL_JOYSTICK_TYPE_GUITAR,
    TYPE_DRUM_KIT     = SDL_JOYSTICK_TYPE_DRUM_KIT,
    TYPE_ARCADE_PAD   = SDL_JOYSTICK_TYPE_ARCADE_PAD,
    TYPE_THROTTLE     = SDL_JOYSTICK_TYPE_THROTTLE,
    TYPE_COUNT        = SDL_JOYSTICK_TYPE_COUNT,
};
REGULAR_ENUM(JoystickType);

enum class JoystickConnectionState
{
    CONNECTION_INVALID  = SDL_JOYSTICK_CONNECTION_INVALID,
    CONNECTION_UNKNOWN  = SDL_JOYSTICK_CONNECTION_UNKNOWN,
    CONNECTION_WIRED    = SDL_JOYSTICK_CONNECTION_WIRED,
    CONNECTION_WIRELESS = SDL_JOYSTICK_CONNECTION_WIRELESS,
};
REGULAR_ENUM(JoystickConnectionState);

constexpr auto JOYSTICK_AXIS_MAX()
{
    return 32767;
}

constexpr auto JOYSTICK_AXIS_MIN()
{
    return -32768;
}

bool HasJoystick(void)
{
    return SDL_HasJoystick();
}

SDL_JoystickID* GetJoysticks(int* count)
{
    return SDL_GetJoysticks(count);
}

const char* GetJoystickNameForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickNameForID(instance_id);
}

const char* GetJoystickPathForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickPathForID(instance_id);
}

int GetJoystickPlayerIndexForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickPlayerIndexForID(instance_id);
}

SDL_GUID GetJoystickGUIDForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickGUIDForID(instance_id);
}

Uint16 GetJoystickVendorForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickVendorForID(instance_id);
}

Uint16 GetJoystickProductForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickProductForID(instance_id);
}

Uint16 GetJoystickProductVersionForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickProductVersionForID(instance_id);
}

SDL_JoystickType GetJoystickTypeForID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickTypeForID(instance_id);
}

SDL_Joystick* OpenJoystick(SDL_JoystickID instance_id)
{
    return SDL_OpenJoystick(instance_id);
}

SDL_Joystick* GetJoystickFromID(SDL_JoystickID instance_id)
{
    return SDL_GetJoystickFromID(instance_id);
}

SDL_Joystick* GetJoystickFromPlayerIndex(int player_index)
{
    return SDL_GetJoystickFromPlayerIndex(player_index);
}

using VirtualJoystickTouchpadDesc = SDL_VirtualJoystickTouchpadDesc;

using VirtualJoystickSensorDesc = SDL_VirtualJoystickSensorDesc;

using VirtualJoystickDesc = SDL_VirtualJoystickDesc;

SDL_JoystickID AttachVirtualJoystick(const SDL_VirtualJoystickDesc* desc)
{
    return SDL_AttachVirtualJoystick(desc);
}

bool DetachVirtualJoystick(SDL_JoystickID instance_id)
{
    return SDL_DetachVirtualJoystick(instance_id);
}

bool IsJoystickVirtual(SDL_JoystickID instance_id)
{
    return SDL_IsJoystickVirtual(instance_id);
}

bool SetJoystickVirtualAxis(SDL_Joystick* joystick, int axis, Sint16 value)
{
    return SDL_SetJoystickVirtualAxis(joystick, axis, value);
}

bool SetJoystickVirtualBall(SDL_Joystick* joystick,
                            int           ball,
                            Sint16        xrel,
                            Sint16        yrel)
{
    return SDL_SetJoystickVirtualBall(joystick, ball, xrel, yrel);
}

bool SetJoystickVirtualButton(SDL_Joystick* joystick, int button, bool down)
{
    return SDL_SetJoystickVirtualButton(joystick, button, down);
}

bool SetJoystickVirtualHat(SDL_Joystick* joystick, int hat, Uint8 value)
{
    return SDL_SetJoystickVirtualHat(joystick, hat, value);
}

bool SetJoystickVirtualTouchpad(SDL_Joystick* joystick,
                                int           touchpad,
                                int           finger,
                                bool          down,
                                float         x,
                                float         y,
                                float         pressure)
{
    return SDL_SetJoystickVirtualTouchpad(
        joystick, touchpad, finger, down, x, y, pressure);
}

bool SendJoystickVirtualSensorData(SDL_Joystick* joystick,
                                   SensorType    type,
                                   Uint64        sensor_timestamp,
                                   const float*  data,
                                   int           num_values)
{
    return SDL_SendJoystickVirtualSensorData(
        joystick, (SDL_SensorType)(type), sensor_timestamp, data, num_values);
}

SDL_PropertiesID GetJoystickProperties(SDL_Joystick* joystick)
{
    return SDL_GetJoystickProperties(joystick);
}

constexpr auto PROP_JOYSTICK_CAP_MONO_LED_BOOLEAN()
{
    return "SDL.joystick.cap.mono_led";
}

constexpr auto PROP_JOYSTICK_CAP_RGB_LED_BOOLEAN()
{
    return "SDL.joystick.cap.rgb_led";
}

constexpr auto PROP_JOYSTICK_CAP_PLAYER_LED_BOOLEAN()
{
    return "SDL.joystick.cap.player_led";
}

constexpr auto PROP_JOYSTICK_CAP_RUMBLE_BOOLEAN()
{
    return "SDL.joystick.cap.rumble";
}

constexpr auto PROP_JOYSTICK_CAP_TRIGGER_RUMBLE_BOOLEAN()
{
    return "SDL.joystick.cap.trigger_rumble";
}

const char* GetJoystickName(SDL_Joystick* joystick)
{
    return SDL_GetJoystickName(joystick);
}

const char* GetJoystickPath(SDL_Joystick* joystick)
{
    return SDL_GetJoystickPath(joystick);
}

int GetJoystickPlayerIndex(SDL_Joystick* joystick)
{
    return SDL_GetJoystickPlayerIndex(joystick);
}

bool SetJoystickPlayerIndex(SDL_Joystick* joystick, int player_index)
{
    return SDL_SetJoystickPlayerIndex(joystick, player_index);
}

SDL_GUID GetJoystickGUID(SDL_Joystick* joystick)
{
    return SDL_GetJoystickGUID(joystick);
}

Uint16 GetJoystickVendor(SDL_Joystick* joystick)
{
    return SDL_GetJoystickVendor(joystick);
}

Uint16 GetJoystickProduct(SDL_Joystick* joystick)
{
    return SDL_GetJoystickProduct(joystick);
}

Uint16 GetJoystickProductVersion(SDL_Joystick* joystick)
{
    return SDL_GetJoystickProductVersion(joystick);
}

Uint16 GetJoystickFirmwareVersion(SDL_Joystick* joystick)
{
    return SDL_GetJoystickFirmwareVersion(joystick);
}

const char* GetJoystickSerial(SDL_Joystick* joystick)
{
    return SDL_GetJoystickSerial(joystick);
}

SDL_JoystickType GetJoystickType(SDL_Joystick* joystick)
{
    return SDL_GetJoystickType(joystick);
}

void GetJoystickGUIDInfo(SDL_GUID guid,
                         Uint16*  vendor,
                         Uint16*  product,
                         Uint16*  version,
                         Uint16*  crc16)
{
    SDL_GetJoystickGUIDInfo(guid, vendor, product, version, crc16);
}

bool JoystickConnected(SDL_Joystick* joystick)
{
    return SDL_JoystickConnected(joystick);
}

SDL_JoystickID GetJoystickID(SDL_Joystick* joystick)
{
    return SDL_GetJoystickID(joystick);
}

int GetNumJoystickAxes(SDL_Joystick* joystick)
{
    return SDL_GetNumJoystickAxes(joystick);
}

int GetNumJoystickBalls(SDL_Joystick* joystick)
{
    return SDL_GetNumJoystickBalls(joystick);
}

int GetNumJoystickHats(SDL_Joystick* joystick)
{
    return SDL_GetNumJoystickHats(joystick);
}

int GetNumJoystickButtons(SDL_Joystick* joystick)
{
    return SDL_GetNumJoystickButtons(joystick);
}

void SetJoystickEventsEnabled(bool enabled)
{
    SDL_SetJoystickEventsEnabled(enabled);
}

bool JoystickEventsEnabled(void)
{
    return SDL_JoystickEventsEnabled();
}

void UpdateJoysticks(void)
{
    SDL_UpdateJoysticks();
}

Sint16 GetJoystickAxis(SDL_Joystick* joystick, int axis)
{
    return SDL_GetJoystickAxis(joystick, axis);
}

bool GetJoystickAxisInitialState(SDL_Joystick* joystick,
                                 int           axis,
                                 Sint16*       state)
{
    return SDL_GetJoystickAxisInitialState(joystick, axis, state);
}

bool GetJoystickBall(SDL_Joystick* joystick, int ball, int* dx, int* dy)
{
    return SDL_GetJoystickBall(joystick, ball, dx, dy);
}

Uint8 GetJoystickHat(SDL_Joystick* joystick, int hat)
{
    return SDL_GetJoystickHat(joystick, hat);
}

constexpr auto HAT_CENTERED()
{
    return 0x00u;
}

constexpr auto HAT_UP()
{
    return 0x01u;
}

constexpr auto HAT_RIGHT()
{
    return 0x02u;
}

constexpr auto HAT_DOWN()
{
    return 0x04u;
}

constexpr auto HAT_LEFT()
{
    return 0x08u;
}

constexpr auto HAT_RIGHTUP()
{
    return (SDL_HAT_RIGHT | SDL_HAT_UP);
}

constexpr auto HAT_RIGHTDOWN()
{
    return (SDL_HAT_RIGHT | SDL_HAT_DOWN);
}

constexpr auto HAT_LEFTUP()
{
    return (SDL_HAT_LEFT | SDL_HAT_UP);
}

constexpr auto HAT_LEFTDOWN()
{
    return (SDL_HAT_LEFT | SDL_HAT_DOWN);
}

bool GetJoystickButton(SDL_Joystick* joystick, int button)
{
    return SDL_GetJoystickButton(joystick, button);
}

bool RumbleJoystick(SDL_Joystick* joystick,
                    Uint16        low_frequency_rumble,
                    Uint16        high_frequency_rumble,
                    Uint32        duration_ms)
{
    return SDL_RumbleJoystick(
        joystick, low_frequency_rumble, high_frequency_rumble, duration_ms);
}

bool RumbleJoystickTriggers(SDL_Joystick* joystick,
                            Uint16        left_rumble,
                            Uint16        right_rumble,
                            Uint32        duration_ms)
{
    return SDL_RumbleJoystickTriggers(
        joystick, left_rumble, right_rumble, duration_ms);
}

bool SetJoystickLED(SDL_Joystick* joystick, Uint8 red, Uint8 green, Uint8 blue)
{
    return SDL_SetJoystickLED(joystick, red, green, blue);
}

bool SendJoystickEffect(SDL_Joystick* joystick, const void* data, int size)
{
    return SDL_SendJoystickEffect(joystick, data, size);
}

void CloseJoystick(SDL_Joystick* joystick)
{
    SDL_CloseJoystick(joystick);
}

SDL_JoystickConnectionState GetJoystickConnectionState(SDL_Joystick* joystick)
{
    return SDL_GetJoystickConnectionState(joystick);
}

SDL_PowerState GetJoystickPowerInfo(SDL_Joystick* joystick, int* percent)
{
    return SDL_GetJoystickPowerInfo(joystick, percent);
}

using SDL_Gamepad = SDL_Gamepad;

enum class GamepadType
{
    TYPE_UNKNOWN             = SDL_GAMEPAD_TYPE_UNKNOWN,
    TYPE_STANDARD            = SDL_GAMEPAD_TYPE_STANDARD,
    TYPE_XBOX360             = SDL_GAMEPAD_TYPE_XBOX360,
    TYPE_XBOXONE             = SDL_GAMEPAD_TYPE_XBOXONE,
    TYPE_PS3                 = SDL_GAMEPAD_TYPE_PS3,
    TYPE_PS4                 = SDL_GAMEPAD_TYPE_PS4,
    TYPE_PS5                 = SDL_GAMEPAD_TYPE_PS5,
    TYPE_NINTENDO_SWITCH_PRO = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
    TYPE_NINTENDO_SWITCH_JOYCON_LEFT =
        SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT,
    TYPE_NINTENDO_SWITCH_JOYCON_RIGHT =
        SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT,
    TYPE_NINTENDO_SWITCH_JOYCON_PAIR =
        SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR,
    TYPE_GAMECUBE = SDL_GAMEPAD_TYPE_GAMECUBE,
    TYPE_COUNT    = SDL_GAMEPAD_TYPE_COUNT,
};
REGULAR_ENUM(GamepadType);

enum class GamepadButton
{
    BUTTON_INVALID        = SDL_GAMEPAD_BUTTON_INVALID,
    BUTTON_SOUTH          = SDL_GAMEPAD_BUTTON_SOUTH,
    BUTTON_EAST           = SDL_GAMEPAD_BUTTON_EAST,
    BUTTON_WEST           = SDL_GAMEPAD_BUTTON_WEST,
    BUTTON_NORTH          = SDL_GAMEPAD_BUTTON_NORTH,
    BUTTON_BACK           = SDL_GAMEPAD_BUTTON_BACK,
    BUTTON_GUIDE          = SDL_GAMEPAD_BUTTON_GUIDE,
    BUTTON_START          = SDL_GAMEPAD_BUTTON_START,
    BUTTON_LEFT_STICK     = SDL_GAMEPAD_BUTTON_LEFT_STICK,
    BUTTON_RIGHT_STICK    = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    BUTTON_LEFT_SHOULDER  = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
    BUTTON_RIGHT_SHOULDER = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    BUTTON_DPAD_UP        = SDL_GAMEPAD_BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN      = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT      = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT     = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
    BUTTON_MISC1          = SDL_GAMEPAD_BUTTON_MISC1,
    BUTTON_RIGHT_PADDLE1  = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,
    BUTTON_LEFT_PADDLE1   = SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,
    BUTTON_RIGHT_PADDLE2  = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,
    BUTTON_LEFT_PADDLE2   = SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,
    BUTTON_TOUCHPAD       = SDL_GAMEPAD_BUTTON_TOUCHPAD,
    BUTTON_MISC2          = SDL_GAMEPAD_BUTTON_MISC2,
    BUTTON_MISC3          = SDL_GAMEPAD_BUTTON_MISC3,
    BUTTON_MISC4          = SDL_GAMEPAD_BUTTON_MISC4,
    BUTTON_MISC5          = SDL_GAMEPAD_BUTTON_MISC5,
    BUTTON_MISC6          = SDL_GAMEPAD_BUTTON_MISC6,
    BUTTON_COUNT          = SDL_GAMEPAD_BUTTON_COUNT,
};
REGULAR_ENUM(GamepadButton);

enum class GamepadButtonLabel
{
    BUTTON_LABEL_UNKNOWN  = SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN,
    BUTTON_LABEL_A        = SDL_GAMEPAD_BUTTON_LABEL_A,
    BUTTON_LABEL_B        = SDL_GAMEPAD_BUTTON_LABEL_B,
    BUTTON_LABEL_X        = SDL_GAMEPAD_BUTTON_LABEL_X,
    BUTTON_LABEL_Y        = SDL_GAMEPAD_BUTTON_LABEL_Y,
    BUTTON_LABEL_CROSS    = SDL_GAMEPAD_BUTTON_LABEL_CROSS,
    BUTTON_LABEL_CIRCLE   = SDL_GAMEPAD_BUTTON_LABEL_CIRCLE,
    BUTTON_LABEL_SQUARE   = SDL_GAMEPAD_BUTTON_LABEL_SQUARE,
    BUTTON_LABEL_TRIANGLE = SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE,
};
REGULAR_ENUM(GamepadButtonLabel);

enum class GamepadAxis
{
    AXIS_INVALID       = SDL_GAMEPAD_AXIS_INVALID,
    AXIS_LEFTX         = SDL_GAMEPAD_AXIS_LEFTX,
    AXIS_LEFTY         = SDL_GAMEPAD_AXIS_LEFTY,
    AXIS_RIGHTX        = SDL_GAMEPAD_AXIS_RIGHTX,
    AXIS_RIGHTY        = SDL_GAMEPAD_AXIS_RIGHTY,
    AXIS_LEFT_TRIGGER  = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
    AXIS_RIGHT_TRIGGER = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
    AXIS_COUNT         = SDL_GAMEPAD_AXIS_COUNT,
};
REGULAR_ENUM(GamepadAxis);

enum class GamepadBindingType
{
    BINDTYPE_NONE   = SDL_GAMEPAD_BINDTYPE_NONE,
    BINDTYPE_BUTTON = SDL_GAMEPAD_BINDTYPE_BUTTON,
    BINDTYPE_AXIS   = SDL_GAMEPAD_BINDTYPE_AXIS,
    BINDTYPE_HAT    = SDL_GAMEPAD_BINDTYPE_HAT,
};
REGULAR_ENUM(GamepadBindingType);

using GamepadBinding = SDL_GamepadBinding;

int AddGamepadMapping(const char* mapping)
{
    return SDL_AddGamepadMapping(mapping);
}

int AddGamepadMappingsFromIO(SDL_IOStream* src, bool closeio)
{
    return SDL_AddGamepadMappingsFromIO(src, closeio);
}

int AddGamepadMappingsFromFile(const char* file)
{
    return SDL_AddGamepadMappingsFromFile(file);
}

bool ReloadGamepadMappings(void)
{
    return SDL_ReloadGamepadMappings();
}

char* GetGamepadMappingForGUID(SDL_GUID guid)
{
    return SDL_GetGamepadMappingForGUID(guid);
}

char* GetGamepadMapping(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadMapping(gamepad);
}

bool SetGamepadMapping(SDL_JoystickID instance_id, const char* mapping)
{
    return SDL_SetGamepadMapping(instance_id, mapping);
}

bool HasGamepad(void)
{
    return SDL_HasGamepad();
}

SDL_JoystickID* GetGamepads(int* count)
{
    return SDL_GetGamepads(count);
}

bool IsGamepad(SDL_JoystickID instance_id)
{
    return SDL_IsGamepad(instance_id);
}

const char* GetGamepadNameForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadNameForID(instance_id);
}

const char* GetGamepadPathForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadPathForID(instance_id);
}

int GetGamepadPlayerIndexForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadPlayerIndexForID(instance_id);
}

SDL_GUID GetGamepadGUIDForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadGUIDForID(instance_id);
}

Uint16 GetGamepadVendorForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadVendorForID(instance_id);
}

Uint16 GetGamepadProductForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadProductForID(instance_id);
}

Uint16 GetGamepadProductVersionForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadProductVersionForID(instance_id);
}

SDL_GamepadType GetGamepadTypeForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadTypeForID(instance_id);
}

SDL_GamepadType GetRealGamepadTypeForID(SDL_JoystickID instance_id)
{
    return SDL_GetRealGamepadTypeForID(instance_id);
}

char* GetGamepadMappingForID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadMappingForID(instance_id);
}

SDL_Gamepad* OpenGamepad(SDL_JoystickID instance_id)
{
    return SDL_OpenGamepad(instance_id);
}

SDL_Gamepad* GetGamepadFromID(SDL_JoystickID instance_id)
{
    return SDL_GetGamepadFromID(instance_id);
}

SDL_Gamepad* GetGamepadFromPlayerIndex(int player_index)
{
    return SDL_GetGamepadFromPlayerIndex(player_index);
}

SDL_PropertiesID GetGamepadProperties(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadProperties(gamepad);
}

constexpr auto PROP_GAMEPAD_CAP_MONO_LED_BOOLEAN()
{
    return SDL_PROP_JOYSTICK_CAP_MONO_LED_BOOLEAN;
}

constexpr auto PROP_GAMEPAD_CAP_RGB_LED_BOOLEAN()
{
    return SDL_PROP_JOYSTICK_CAP_RGB_LED_BOOLEAN;
}

constexpr auto PROP_GAMEPAD_CAP_PLAYER_LED_BOOLEAN()
{
    return SDL_PROP_JOYSTICK_CAP_PLAYER_LED_BOOLEAN;
}

constexpr auto PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN()
{
    return SDL_PROP_JOYSTICK_CAP_RUMBLE_BOOLEAN;
}

constexpr auto PROP_GAMEPAD_CAP_TRIGGER_RUMBLE_BOOLEAN()
{
    return SDL_PROP_JOYSTICK_CAP_TRIGGER_RUMBLE_BOOLEAN;
}

SDL_JoystickID GetGamepadID(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadID(gamepad);
}

const char* GetGamepadName(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadName(gamepad);
}

const char* GetGamepadPath(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadPath(gamepad);
}

SDL_GamepadType GetGamepadType(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadType(gamepad);
}

SDL_GamepadType GetRealGamepadType(SDL_Gamepad* gamepad)
{
    return SDL_GetRealGamepadType(gamepad);
}

int GetGamepadPlayerIndex(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadPlayerIndex(gamepad);
}

bool SetGamepadPlayerIndex(SDL_Gamepad* gamepad, int player_index)
{
    return SDL_SetGamepadPlayerIndex(gamepad, player_index);
}

Uint16 GetGamepadVendor(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadVendor(gamepad);
}

Uint16 GetGamepadProduct(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadProduct(gamepad);
}

Uint16 GetGamepadProductVersion(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadProductVersion(gamepad);
}

Uint16 GetGamepadFirmwareVersion(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadFirmwareVersion(gamepad);
}

const char* GetGamepadSerial(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadSerial(gamepad);
}

Uint64 GetGamepadSteamHandle(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadSteamHandle(gamepad);
}

SDL_JoystickConnectionState GetGamepadConnectionState(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadConnectionState(gamepad);
}

SDL_PowerState GetGamepadPowerInfo(SDL_Gamepad* gamepad, int* percent)
{
    return SDL_GetGamepadPowerInfo(gamepad, percent);
}

bool GamepadConnected(SDL_Gamepad* gamepad)
{
    return SDL_GamepadConnected(gamepad);
}

SDL_Joystick* GetGamepadJoystick(SDL_Gamepad* gamepad)
{
    return SDL_GetGamepadJoystick(gamepad);
}

void SetGamepadEventsEnabled(bool enabled)
{
    SDL_SetGamepadEventsEnabled(enabled);
}

bool GamepadEventsEnabled(void)
{
    return SDL_GamepadEventsEnabled();
}

void UpdateGamepads(void)
{
    SDL_UpdateGamepads();
}

SDL_GamepadType GetGamepadTypeFromString(const char* str)
{
    return SDL_GetGamepadTypeFromString(str);
}

const char* GetGamepadStringForType(GamepadType type)
{
    return SDL_GetGamepadStringForType((SDL_GamepadType)(type));
}

SDL_GamepadAxis GetGamepadAxisFromString(const char* str)
{
    return SDL_GetGamepadAxisFromString(str);
}

const char* GetGamepadStringForAxis(GamepadAxis axis)
{
    return SDL_GetGamepadStringForAxis((SDL_GamepadAxis)(axis));
}

bool GamepadHasAxis(SDL_Gamepad* gamepad, GamepadAxis axis)
{
    return SDL_GamepadHasAxis(gamepad, (SDL_GamepadAxis)(axis));
}

Sint16 GetGamepadAxis(SDL_Gamepad* gamepad, GamepadAxis axis)
{
    return SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)(axis));
}

SDL_GamepadButton GetGamepadButtonFromString(const char* str)
{
    return SDL_GetGamepadButtonFromString(str);
}

const char* GetGamepadStringForButton(GamepadButton button)
{
    return SDL_GetGamepadStringForButton((SDL_GamepadButton)(button));
}

bool GamepadHasButton(SDL_Gamepad* gamepad, GamepadButton button)
{
    return SDL_GamepadHasButton(gamepad, (SDL_GamepadButton)(button));
}

bool GetGamepadButton(SDL_Gamepad* gamepad, GamepadButton button)
{
    return SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)(button));
}

SDL_GamepadButtonLabel GetGamepadButtonLabelForType(GamepadType   type,
                                                    GamepadButton button)
{
    return SDL_GetGamepadButtonLabelForType((SDL_GamepadType)(type),
                                            (SDL_GamepadButton)(button));
}

SDL_GamepadButtonLabel GetGamepadButtonLabel(SDL_Gamepad*  gamepad,
                                             GamepadButton button)
{
    return SDL_GetGamepadButtonLabel(gamepad, (SDL_GamepadButton)(button));
}

int GetNumGamepadTouchpads(SDL_Gamepad* gamepad)
{
    return SDL_GetNumGamepadTouchpads(gamepad);
}

int GetNumGamepadTouchpadFingers(SDL_Gamepad* gamepad, int touchpad)
{
    return SDL_GetNumGamepadTouchpadFingers(gamepad, touchpad);
}

bool GetGamepadTouchpadFinger(SDL_Gamepad* gamepad,
                              int          touchpad,
                              int          finger,
                              bool*        down,
                              float*       x,
                              float*       y,
                              float*       pressure)
{
    return SDL_GetGamepadTouchpadFinger(
        gamepad, touchpad, finger, down, x, y, pressure);
}

bool GamepadHasSensor(SDL_Gamepad* gamepad, SensorType type)
{
    return SDL_GamepadHasSensor(gamepad, (SDL_SensorType)(type));
}

bool SetGamepadSensorEnabled(SDL_Gamepad* gamepad,
                             SensorType   type,
                             bool         enabled)
{
    return SDL_SetGamepadSensorEnabled(
        gamepad, (SDL_SensorType)(type), enabled);
}

bool GamepadSensorEnabled(SDL_Gamepad* gamepad, SensorType type)
{
    return SDL_GamepadSensorEnabled(gamepad, (SDL_SensorType)(type));
}

float GetGamepadSensorDataRate(SDL_Gamepad* gamepad, SensorType type)
{
    return SDL_GetGamepadSensorDataRate(gamepad, (SDL_SensorType)(type));
}

bool GetGamepadSensorData(SDL_Gamepad* gamepad,
                          SensorType   type,
                          float*       data,
                          int          num_values)
{
    return SDL_GetGamepadSensorData(
        gamepad, (SDL_SensorType)(type), data, num_values);
}

bool RumbleGamepad(SDL_Gamepad* gamepad,
                   Uint16       low_frequency_rumble,
                   Uint16       high_frequency_rumble,
                   Uint32       duration_ms)
{
    return SDL_RumbleGamepad(
        gamepad, low_frequency_rumble, high_frequency_rumble, duration_ms);
}

bool RumbleGamepadTriggers(SDL_Gamepad* gamepad,
                           Uint16       left_rumble,
                           Uint16       right_rumble,
                           Uint32       duration_ms)
{
    return SDL_RumbleGamepadTriggers(
        gamepad, left_rumble, right_rumble, duration_ms);
}

bool SetGamepadLED(SDL_Gamepad* gamepad, Uint8 red, Uint8 green, Uint8 blue)
{
    return SDL_SetGamepadLED(gamepad, red, green, blue);
}

bool SendGamepadEffect(SDL_Gamepad* gamepad, const void* data, int size)
{
    return SDL_SendGamepadEffect(gamepad, data, size);
}

void CloseGamepad(SDL_Gamepad* gamepad)
{
    SDL_CloseGamepad(gamepad);
}

const char* GetGamepadAppleSFSymbolsNameForButton(SDL_Gamepad*  gamepad,
                                                  GamepadButton button)
{
    return SDL_GetGamepadAppleSFSymbolsNameForButton(
        gamepad, (SDL_GamepadButton)(button));
}

const char* GetGamepadAppleSFSymbolsNameForAxis(SDL_Gamepad* gamepad,
                                                GamepadAxis  axis)
{
    return SDL_GetGamepadAppleSFSymbolsNameForAxis(gamepad,
                                                   (SDL_GamepadAxis)(axis));
}

enum class Scancode
{
    UNKNOWN              = SDL_SCANCODE_UNKNOWN,
    A                    = SDL_SCANCODE_A,
    B                    = SDL_SCANCODE_B,
    C                    = SDL_SCANCODE_C,
    D                    = SDL_SCANCODE_D,
    E                    = SDL_SCANCODE_E,
    F                    = SDL_SCANCODE_F,
    G                    = SDL_SCANCODE_G,
    H                    = SDL_SCANCODE_H,
    I                    = SDL_SCANCODE_I,
    J                    = SDL_SCANCODE_J,
    K                    = SDL_SCANCODE_K,
    L                    = SDL_SCANCODE_L,
    M                    = SDL_SCANCODE_M,
    N                    = SDL_SCANCODE_N,
    O                    = SDL_SCANCODE_O,
    P                    = SDL_SCANCODE_P,
    Q                    = SDL_SCANCODE_Q,
    R                    = SDL_SCANCODE_R,
    S                    = SDL_SCANCODE_S,
    T                    = SDL_SCANCODE_T,
    U                    = SDL_SCANCODE_U,
    V                    = SDL_SCANCODE_V,
    W                    = SDL_SCANCODE_W,
    X                    = SDL_SCANCODE_X,
    Y                    = SDL_SCANCODE_Y,
    Z                    = SDL_SCANCODE_Z,
    _1                   = SDL_SCANCODE_1,
    _2                   = SDL_SCANCODE_2,
    _3                   = SDL_SCANCODE_3,
    _4                   = SDL_SCANCODE_4,
    _5                   = SDL_SCANCODE_5,
    _6                   = SDL_SCANCODE_6,
    _7                   = SDL_SCANCODE_7,
    _8                   = SDL_SCANCODE_8,
    _9                   = SDL_SCANCODE_9,
    _0                   = SDL_SCANCODE_0,
    RETURN               = SDL_SCANCODE_RETURN,
    ESCAPE               = SDL_SCANCODE_ESCAPE,
    BACKSPACE            = SDL_SCANCODE_BACKSPACE,
    TAB                  = SDL_SCANCODE_TAB,
    SPACE                = SDL_SCANCODE_SPACE,
    MINUS                = SDL_SCANCODE_MINUS,
    EQUALS               = SDL_SCANCODE_EQUALS,
    LEFTBRACKET          = SDL_SCANCODE_LEFTBRACKET,
    RIGHTBRACKET         = SDL_SCANCODE_RIGHTBRACKET,
    BACKSLASH            = SDL_SCANCODE_BACKSLASH,
    NONUSHASH            = SDL_SCANCODE_NONUSHASH,
    SEMICOLON            = SDL_SCANCODE_SEMICOLON,
    APOSTROPHE           = SDL_SCANCODE_APOSTROPHE,
    GRAVE                = SDL_SCANCODE_GRAVE,
    COMMA                = SDL_SCANCODE_COMMA,
    PERIOD               = SDL_SCANCODE_PERIOD,
    SLASH                = SDL_SCANCODE_SLASH,
    CAPSLOCK             = SDL_SCANCODE_CAPSLOCK,
    F1                   = SDL_SCANCODE_F1,
    F2                   = SDL_SCANCODE_F2,
    F3                   = SDL_SCANCODE_F3,
    F4                   = SDL_SCANCODE_F4,
    F5                   = SDL_SCANCODE_F5,
    F6                   = SDL_SCANCODE_F6,
    F7                   = SDL_SCANCODE_F7,
    F8                   = SDL_SCANCODE_F8,
    F9                   = SDL_SCANCODE_F9,
    F10                  = SDL_SCANCODE_F10,
    F11                  = SDL_SCANCODE_F11,
    F12                  = SDL_SCANCODE_F12,
    PRINTSCREEN          = SDL_SCANCODE_PRINTSCREEN,
    SCROLLLOCK           = SDL_SCANCODE_SCROLLLOCK,
    PAUSE                = SDL_SCANCODE_PAUSE,
    INSERT               = SDL_SCANCODE_INSERT,
    HOME                 = SDL_SCANCODE_HOME,
    PAGEUP               = SDL_SCANCODE_PAGEUP,
    DELETE               = SDL_SCANCODE_DELETE,
    END                  = SDL_SCANCODE_END,
    PAGEDOWN             = SDL_SCANCODE_PAGEDOWN,
    RIGHT                = SDL_SCANCODE_RIGHT,
    LEFT                 = SDL_SCANCODE_LEFT,
    DOWN                 = SDL_SCANCODE_DOWN,
    UP                   = SDL_SCANCODE_UP,
    NUMLOCKCLEAR         = SDL_SCANCODE_NUMLOCKCLEAR,
    KP_DIVIDE            = SDL_SCANCODE_KP_DIVIDE,
    KP_MULTIPLY          = SDL_SCANCODE_KP_MULTIPLY,
    KP_MINUS             = SDL_SCANCODE_KP_MINUS,
    KP_PLUS              = SDL_SCANCODE_KP_PLUS,
    KP_ENTER             = SDL_SCANCODE_KP_ENTER,
    KP_1                 = SDL_SCANCODE_KP_1,
    KP_2                 = SDL_SCANCODE_KP_2,
    KP_3                 = SDL_SCANCODE_KP_3,
    KP_4                 = SDL_SCANCODE_KP_4,
    KP_5                 = SDL_SCANCODE_KP_5,
    KP_6                 = SDL_SCANCODE_KP_6,
    KP_7                 = SDL_SCANCODE_KP_7,
    KP_8                 = SDL_SCANCODE_KP_8,
    KP_9                 = SDL_SCANCODE_KP_9,
    KP_0                 = SDL_SCANCODE_KP_0,
    KP_PERIOD            = SDL_SCANCODE_KP_PERIOD,
    NONUSBACKSLASH       = SDL_SCANCODE_NONUSBACKSLASH,
    APPLICATION          = SDL_SCANCODE_APPLICATION,
    POWER                = SDL_SCANCODE_POWER,
    KP_EQUALS            = SDL_SCANCODE_KP_EQUALS,
    F13                  = SDL_SCANCODE_F13,
    F14                  = SDL_SCANCODE_F14,
    F15                  = SDL_SCANCODE_F15,
    F16                  = SDL_SCANCODE_F16,
    F17                  = SDL_SCANCODE_F17,
    F18                  = SDL_SCANCODE_F18,
    F19                  = SDL_SCANCODE_F19,
    F20                  = SDL_SCANCODE_F20,
    F21                  = SDL_SCANCODE_F21,
    F22                  = SDL_SCANCODE_F22,
    F23                  = SDL_SCANCODE_F23,
    F24                  = SDL_SCANCODE_F24,
    EXECUTE              = SDL_SCANCODE_EXECUTE,
    HELP                 = SDL_SCANCODE_HELP,
    MENU                 = SDL_SCANCODE_MENU,
    SELECT               = SDL_SCANCODE_SELECT,
    STOP                 = SDL_SCANCODE_STOP,
    AGAIN                = SDL_SCANCODE_AGAIN,
    UNDO                 = SDL_SCANCODE_UNDO,
    CUT                  = SDL_SCANCODE_CUT,
    COPY                 = SDL_SCANCODE_COPY,
    PASTE                = SDL_SCANCODE_PASTE,
    FIND                 = SDL_SCANCODE_FIND,
    MUTE                 = SDL_SCANCODE_MUTE,
    VOLUMEUP             = SDL_SCANCODE_VOLUMEUP,
    VOLUMEDOWN           = SDL_SCANCODE_VOLUMEDOWN,
    KP_COMMA             = SDL_SCANCODE_KP_COMMA,
    KP_EQUALSAS400       = SDL_SCANCODE_KP_EQUALSAS400,
    INTERNATIONAL1       = SDL_SCANCODE_INTERNATIONAL1,
    INTERNATIONAL2       = SDL_SCANCODE_INTERNATIONAL2,
    INTERNATIONAL3       = SDL_SCANCODE_INTERNATIONAL3,
    INTERNATIONAL4       = SDL_SCANCODE_INTERNATIONAL4,
    INTERNATIONAL5       = SDL_SCANCODE_INTERNATIONAL5,
    INTERNATIONAL6       = SDL_SCANCODE_INTERNATIONAL6,
    INTERNATIONAL7       = SDL_SCANCODE_INTERNATIONAL7,
    INTERNATIONAL8       = SDL_SCANCODE_INTERNATIONAL8,
    INTERNATIONAL9       = SDL_SCANCODE_INTERNATIONAL9,
    LANG1                = SDL_SCANCODE_LANG1,
    LANG2                = SDL_SCANCODE_LANG2,
    LANG3                = SDL_SCANCODE_LANG3,
    LANG4                = SDL_SCANCODE_LANG4,
    LANG5                = SDL_SCANCODE_LANG5,
    LANG6                = SDL_SCANCODE_LANG6,
    LANG7                = SDL_SCANCODE_LANG7,
    LANG8                = SDL_SCANCODE_LANG8,
    LANG9                = SDL_SCANCODE_LANG9,
    ALTERASE             = SDL_SCANCODE_ALTERASE,
    SYSREQ               = SDL_SCANCODE_SYSREQ,
    CANCEL               = SDL_SCANCODE_CANCEL,
    CLEAR                = SDL_SCANCODE_CLEAR,
    PRIOR                = SDL_SCANCODE_PRIOR,
    RETURN2              = SDL_SCANCODE_RETURN2,
    SEPARATOR            = SDL_SCANCODE_SEPARATOR,
    OUT                  = SDL_SCANCODE_OUT,
    OPER                 = SDL_SCANCODE_OPER,
    CLEARAGAIN           = SDL_SCANCODE_CLEARAGAIN,
    CRSEL                = SDL_SCANCODE_CRSEL,
    EXSEL                = SDL_SCANCODE_EXSEL,
    KP_00                = SDL_SCANCODE_KP_00,
    KP_000               = SDL_SCANCODE_KP_000,
    THOUSANDSSEPARATOR   = SDL_SCANCODE_THOUSANDSSEPARATOR,
    DECIMALSEPARATOR     = SDL_SCANCODE_DECIMALSEPARATOR,
    CURRENCYUNIT         = SDL_SCANCODE_CURRENCYUNIT,
    CURRENCYSUBUNIT      = SDL_SCANCODE_CURRENCYSUBUNIT,
    KP_LEFTPAREN         = SDL_SCANCODE_KP_LEFTPAREN,
    KP_RIGHTPAREN        = SDL_SCANCODE_KP_RIGHTPAREN,
    KP_LEFTBRACE         = SDL_SCANCODE_KP_LEFTBRACE,
    KP_RIGHTBRACE        = SDL_SCANCODE_KP_RIGHTBRACE,
    KP_TAB               = SDL_SCANCODE_KP_TAB,
    KP_BACKSPACE         = SDL_SCANCODE_KP_BACKSPACE,
    KP_A                 = SDL_SCANCODE_KP_A,
    KP_B                 = SDL_SCANCODE_KP_B,
    KP_C                 = SDL_SCANCODE_KP_C,
    KP_D                 = SDL_SCANCODE_KP_D,
    KP_E                 = SDL_SCANCODE_KP_E,
    KP_F                 = SDL_SCANCODE_KP_F,
    KP_XOR               = SDL_SCANCODE_KP_XOR,
    KP_POWER             = SDL_SCANCODE_KP_POWER,
    KP_PERCENT           = SDL_SCANCODE_KP_PERCENT,
    KP_LESS              = SDL_SCANCODE_KP_LESS,
    KP_GREATER           = SDL_SCANCODE_KP_GREATER,
    KP_AMPERSAND         = SDL_SCANCODE_KP_AMPERSAND,
    KP_DBLAMPERSAND      = SDL_SCANCODE_KP_DBLAMPERSAND,
    KP_VERTICALBAR       = SDL_SCANCODE_KP_VERTICALBAR,
    KP_DBLVERTICALBAR    = SDL_SCANCODE_KP_DBLVERTICALBAR,
    KP_COLON             = SDL_SCANCODE_KP_COLON,
    KP_HASH              = SDL_SCANCODE_KP_HASH,
    KP_SPACE             = SDL_SCANCODE_KP_SPACE,
    KP_AT                = SDL_SCANCODE_KP_AT,
    KP_EXCLAM            = SDL_SCANCODE_KP_EXCLAM,
    KP_MEMSTORE          = SDL_SCANCODE_KP_MEMSTORE,
    KP_MEMRECALL         = SDL_SCANCODE_KP_MEMRECALL,
    KP_MEMCLEAR          = SDL_SCANCODE_KP_MEMCLEAR,
    KP_MEMADD            = SDL_SCANCODE_KP_MEMADD,
    KP_MEMSUBTRACT       = SDL_SCANCODE_KP_MEMSUBTRACT,
    KP_MEMMULTIPLY       = SDL_SCANCODE_KP_MEMMULTIPLY,
    KP_MEMDIVIDE         = SDL_SCANCODE_KP_MEMDIVIDE,
    KP_PLUSMINUS         = SDL_SCANCODE_KP_PLUSMINUS,
    KP_CLEAR             = SDL_SCANCODE_KP_CLEAR,
    KP_CLEARENTRY        = SDL_SCANCODE_KP_CLEARENTRY,
    KP_BINARY            = SDL_SCANCODE_KP_BINARY,
    KP_OCTAL             = SDL_SCANCODE_KP_OCTAL,
    KP_DECIMAL           = SDL_SCANCODE_KP_DECIMAL,
    KP_HEXADECIMAL       = SDL_SCANCODE_KP_HEXADECIMAL,
    LCTRL                = SDL_SCANCODE_LCTRL,
    LSHIFT               = SDL_SCANCODE_LSHIFT,
    LALT                 = SDL_SCANCODE_LALT,
    LGUI                 = SDL_SCANCODE_LGUI,
    RCTRL                = SDL_SCANCODE_RCTRL,
    RSHIFT               = SDL_SCANCODE_RSHIFT,
    RALT                 = SDL_SCANCODE_RALT,
    RGUI                 = SDL_SCANCODE_RGUI,
    MODE                 = SDL_SCANCODE_MODE,
    SLEEP                = SDL_SCANCODE_SLEEP,
    WAKE                 = SDL_SCANCODE_WAKE,
    CHANNEL_INCREMENT    = SDL_SCANCODE_CHANNEL_INCREMENT,
    CHANNEL_DECREMENT    = SDL_SCANCODE_CHANNEL_DECREMENT,
    MEDIA_PLAY           = SDL_SCANCODE_MEDIA_PLAY,
    MEDIA_PAUSE          = SDL_SCANCODE_MEDIA_PAUSE,
    MEDIA_RECORD         = SDL_SCANCODE_MEDIA_RECORD,
    MEDIA_FAST_FORWARD   = SDL_SCANCODE_MEDIA_FAST_FORWARD,
    MEDIA_REWIND         = SDL_SCANCODE_MEDIA_REWIND,
    MEDIA_NEXT_TRACK     = SDL_SCANCODE_MEDIA_NEXT_TRACK,
    MEDIA_PREVIOUS_TRACK = SDL_SCANCODE_MEDIA_PREVIOUS_TRACK,
    MEDIA_STOP           = SDL_SCANCODE_MEDIA_STOP,
    MEDIA_EJECT          = SDL_SCANCODE_MEDIA_EJECT,
    MEDIA_PLAY_PAUSE     = SDL_SCANCODE_MEDIA_PLAY_PAUSE,
    MEDIA_SELECT         = SDL_SCANCODE_MEDIA_SELECT,
    AC_NEW               = SDL_SCANCODE_AC_NEW,
    AC_OPEN              = SDL_SCANCODE_AC_OPEN,
    AC_CLOSE             = SDL_SCANCODE_AC_CLOSE,
    AC_EXIT              = SDL_SCANCODE_AC_EXIT,
    AC_SAVE              = SDL_SCANCODE_AC_SAVE,
    AC_PRINT             = SDL_SCANCODE_AC_PRINT,
    AC_PROPERTIES        = SDL_SCANCODE_AC_PROPERTIES,
    AC_SEARCH            = SDL_SCANCODE_AC_SEARCH,
    AC_HOME              = SDL_SCANCODE_AC_HOME,
    AC_BACK              = SDL_SCANCODE_AC_BACK,
    AC_FORWARD           = SDL_SCANCODE_AC_FORWARD,
    AC_STOP              = SDL_SCANCODE_AC_STOP,
    AC_REFRESH           = SDL_SCANCODE_AC_REFRESH,
    AC_BOOKMARKS         = SDL_SCANCODE_AC_BOOKMARKS,
    SOFTLEFT             = SDL_SCANCODE_SOFTLEFT,
    SOFTRIGHT            = SDL_SCANCODE_SOFTRIGHT,
    CALL                 = SDL_SCANCODE_CALL,
    ENDCALL              = SDL_SCANCODE_ENDCALL,
    RESERVED             = SDL_SCANCODE_RESERVED,
    COUNT                = SDL_SCANCODE_COUNT,
};
REGULAR_ENUM(Scancode);

enum class Keycode : Uint32
{
    EXTENDED_MASK        = SDLK_EXTENDED_MASK,
    SCANCODE_MASK        = SDLK_SCANCODE_MASK,
    UNKNOWN              = SDLK_UNKNOWN,
    RETURN               = SDLK_RETURN,
    ESCAPE               = SDLK_ESCAPE,
    BACKSPACE            = SDLK_BACKSPACE,
    TAB                  = SDLK_TAB,
    SPACE                = SDLK_SPACE,
    EXCLAIM              = SDLK_EXCLAIM,
    DBLAPOSTROPHE        = SDLK_DBLAPOSTROPHE,
    HASH                 = SDLK_HASH,
    DOLLAR               = SDLK_DOLLAR,
    PERCENT              = SDLK_PERCENT,
    AMPERSAND            = SDLK_AMPERSAND,
    APOSTROPHE           = SDLK_APOSTROPHE,
    LEFTPAREN            = SDLK_LEFTPAREN,
    RIGHTPAREN           = SDLK_RIGHTPAREN,
    ASTERISK             = SDLK_ASTERISK,
    PLUS                 = SDLK_PLUS,
    COMMA                = SDLK_COMMA,
    MINUS                = SDLK_MINUS,
    PERIOD               = SDLK_PERIOD,
    SLASH                = SDLK_SLASH,
    _0                   = SDLK_0,
    _1                   = SDLK_1,
    _2                   = SDLK_2,
    _3                   = SDLK_3,
    _4                   = SDLK_4,
    _5                   = SDLK_5,
    _6                   = SDLK_6,
    _7                   = SDLK_7,
    _8                   = SDLK_8,
    _9                   = SDLK_9,
    COLON                = SDLK_COLON,
    SEMICOLON            = SDLK_SEMICOLON,
    LESS                 = SDLK_LESS,
    EQUALS               = SDLK_EQUALS,
    GREATER              = SDLK_GREATER,
    QUESTION             = SDLK_QUESTION,
    AT                   = SDLK_AT,
    LEFTBRACKET          = SDLK_LEFTBRACKET,
    BACKSLASH            = SDLK_BACKSLASH,
    RIGHTBRACKET         = SDLK_RIGHTBRACKET,
    CARET                = SDLK_CARET,
    UNDERSCORE           = SDLK_UNDERSCORE,
    GRAVE                = SDLK_GRAVE,
    A                    = SDLK_A,
    B                    = SDLK_B,
    C                    = SDLK_C,
    D                    = SDLK_D,
    E                    = SDLK_E,
    F                    = SDLK_F,
    G                    = SDLK_G,
    H                    = SDLK_H,
    I                    = SDLK_I,
    J                    = SDLK_J,
    K                    = SDLK_K,
    L                    = SDLK_L,
    M                    = SDLK_M,
    N                    = SDLK_N,
    O                    = SDLK_O,
    P                    = SDLK_P,
    Q                    = SDLK_Q,
    R                    = SDLK_R,
    S                    = SDLK_S,
    T                    = SDLK_T,
    U                    = SDLK_U,
    V                    = SDLK_V,
    W                    = SDLK_W,
    X                    = SDLK_X,
    Y                    = SDLK_Y,
    Z                    = SDLK_Z,
    LEFTBRACE            = SDLK_LEFTBRACE,
    PIPE                 = SDLK_PIPE,
    RIGHTBRACE           = SDLK_RIGHTBRACE,
    TILDE                = SDLK_TILDE,
    DELETE               = SDLK_DELETE,
    PLUSMINUS            = SDLK_PLUSMINUS,
    CAPSLOCK             = SDLK_CAPSLOCK,
    F1                   = SDLK_F1,
    F2                   = SDLK_F2,
    F3                   = SDLK_F3,
    F4                   = SDLK_F4,
    F5                   = SDLK_F5,
    F6                   = SDLK_F6,
    F7                   = SDLK_F7,
    F8                   = SDLK_F8,
    F9                   = SDLK_F9,
    F10                  = SDLK_F10,
    F11                  = SDLK_F11,
    F12                  = SDLK_F12,
    PRINTSCREEN          = SDLK_PRINTSCREEN,
    SCROLLLOCK           = SDLK_SCROLLLOCK,
    PAUSE                = SDLK_PAUSE,
    INSERT               = SDLK_INSERT,
    HOME                 = SDLK_HOME,
    PAGEUP               = SDLK_PAGEUP,
    END                  = SDLK_END,
    PAGEDOWN             = SDLK_PAGEDOWN,
    RIGHT                = SDLK_RIGHT,
    LEFT                 = SDLK_LEFT,
    DOWN                 = SDLK_DOWN,
    UP                   = SDLK_UP,
    NUMLOCKCLEAR         = SDLK_NUMLOCKCLEAR,
    KP_DIVIDE            = SDLK_KP_DIVIDE,
    KP_MULTIPLY          = SDLK_KP_MULTIPLY,
    KP_MINUS             = SDLK_KP_MINUS,
    KP_PLUS              = SDLK_KP_PLUS,
    KP_ENTER             = SDLK_KP_ENTER,
    KP_1                 = SDLK_KP_1,
    KP_2                 = SDLK_KP_2,
    KP_3                 = SDLK_KP_3,
    KP_4                 = SDLK_KP_4,
    KP_5                 = SDLK_KP_5,
    KP_6                 = SDLK_KP_6,
    KP_7                 = SDLK_KP_7,
    KP_8                 = SDLK_KP_8,
    KP_9                 = SDLK_KP_9,
    KP_0                 = SDLK_KP_0,
    KP_PERIOD            = SDLK_KP_PERIOD,
    APPLICATION          = SDLK_APPLICATION,
    POWER                = SDLK_POWER,
    KP_EQUALS            = SDLK_KP_EQUALS,
    F13                  = SDLK_F13,
    F14                  = SDLK_F14,
    F15                  = SDLK_F15,
    F16                  = SDLK_F16,
    F17                  = SDLK_F17,
    F18                  = SDLK_F18,
    F19                  = SDLK_F19,
    F20                  = SDLK_F20,
    F21                  = SDLK_F21,
    F22                  = SDLK_F22,
    F23                  = SDLK_F23,
    F24                  = SDLK_F24,
    EXECUTE              = SDLK_EXECUTE,
    HELP                 = SDLK_HELP,
    MENU                 = SDLK_MENU,
    SELECT               = SDLK_SELECT,
    STOP                 = SDLK_STOP,
    AGAIN                = SDLK_AGAIN,
    UNDO                 = SDLK_UNDO,
    CUT                  = SDLK_CUT,
    COPY                 = SDLK_COPY,
    PASTE                = SDLK_PASTE,
    FIND                 = SDLK_FIND,
    MUTE                 = SDLK_MUTE,
    VOLUMEUP             = SDLK_VOLUMEUP,
    VOLUMEDOWN           = SDLK_VOLUMEDOWN,
    KP_COMMA             = SDLK_KP_COMMA,
    KP_EQUALSAS400       = SDLK_KP_EQUALSAS400,
    ALTERASE             = SDLK_ALTERASE,
    SYSREQ               = SDLK_SYSREQ,
    CANCEL               = SDLK_CANCEL,
    CLEAR                = SDLK_CLEAR,
    PRIOR                = SDLK_PRIOR,
    RETURN2              = SDLK_RETURN2,
    SEPARATOR            = SDLK_SEPARATOR,
    OUT                  = SDLK_OUT,
    OPER                 = SDLK_OPER,
    CLEARAGAIN           = SDLK_CLEARAGAIN,
    CRSEL                = SDLK_CRSEL,
    EXSEL                = SDLK_EXSEL,
    KP_00                = SDLK_KP_00,
    KP_000               = SDLK_KP_000,
    THOUSANDSSEPARATOR   = SDLK_THOUSANDSSEPARATOR,
    DECIMALSEPARATOR     = SDLK_DECIMALSEPARATOR,
    CURRENCYUNIT         = SDLK_CURRENCYUNIT,
    CURRENCYSUBUNIT      = SDLK_CURRENCYSUBUNIT,
    KP_LEFTPAREN         = SDLK_KP_LEFTPAREN,
    KP_RIGHTPAREN        = SDLK_KP_RIGHTPAREN,
    KP_LEFTBRACE         = SDLK_KP_LEFTBRACE,
    KP_RIGHTBRACE        = SDLK_KP_RIGHTBRACE,
    KP_TAB               = SDLK_KP_TAB,
    KP_BACKSPACE         = SDLK_KP_BACKSPACE,
    KP_A                 = SDLK_KP_A,
    KP_B                 = SDLK_KP_B,
    KP_C                 = SDLK_KP_C,
    KP_D                 = SDLK_KP_D,
    KP_E                 = SDLK_KP_E,
    KP_F                 = SDLK_KP_F,
    KP_XOR               = SDLK_KP_XOR,
    KP_POWER             = SDLK_KP_POWER,
    KP_PERCENT           = SDLK_KP_PERCENT,
    KP_LESS              = SDLK_KP_LESS,
    KP_GREATER           = SDLK_KP_GREATER,
    KP_AMPERSAND         = SDLK_KP_AMPERSAND,
    KP_DBLAMPERSAND      = SDLK_KP_DBLAMPERSAND,
    KP_VERTICALBAR       = SDLK_KP_VERTICALBAR,
    KP_DBLVERTICALBAR    = SDLK_KP_DBLVERTICALBAR,
    KP_COLON             = SDLK_KP_COLON,
    KP_HASH              = SDLK_KP_HASH,
    KP_SPACE             = SDLK_KP_SPACE,
    KP_AT                = SDLK_KP_AT,
    KP_EXCLAM            = SDLK_KP_EXCLAM,
    KP_MEMSTORE          = SDLK_KP_MEMSTORE,
    KP_MEMRECALL         = SDLK_KP_MEMRECALL,
    KP_MEMCLEAR          = SDLK_KP_MEMCLEAR,
    KP_MEMADD            = SDLK_KP_MEMADD,
    KP_MEMSUBTRACT       = SDLK_KP_MEMSUBTRACT,
    KP_MEMMULTIPLY       = SDLK_KP_MEMMULTIPLY,
    KP_MEMDIVIDE         = SDLK_KP_MEMDIVIDE,
    KP_PLUSMINUS         = SDLK_KP_PLUSMINUS,
    KP_CLEAR             = SDLK_KP_CLEAR,
    KP_CLEARENTRY        = SDLK_KP_CLEARENTRY,
    KP_BINARY            = SDLK_KP_BINARY,
    KP_OCTAL             = SDLK_KP_OCTAL,
    KP_DECIMAL           = SDLK_KP_DECIMAL,
    KP_HEXADECIMAL       = SDLK_KP_HEXADECIMAL,
    LCTRL                = SDLK_LCTRL,
    LSHIFT               = SDLK_LSHIFT,
    LALT                 = SDLK_LALT,
    LGUI                 = SDLK_LGUI,
    RCTRL                = SDLK_RCTRL,
    RSHIFT               = SDLK_RSHIFT,
    RALT                 = SDLK_RALT,
    RGUI                 = SDLK_RGUI,
    MODE                 = SDLK_MODE,
    SLEEP                = SDLK_SLEEP,
    WAKE                 = SDLK_WAKE,
    CHANNEL_INCREMENT    = SDLK_CHANNEL_INCREMENT,
    CHANNEL_DECREMENT    = SDLK_CHANNEL_DECREMENT,
    MEDIA_PLAY           = SDLK_MEDIA_PLAY,
    MEDIA_PAUSE          = SDLK_MEDIA_PAUSE,
    MEDIA_RECORD         = SDLK_MEDIA_RECORD,
    MEDIA_FAST_FORWARD   = SDLK_MEDIA_FAST_FORWARD,
    MEDIA_REWIND         = SDLK_MEDIA_REWIND,
    MEDIA_NEXT_TRACK     = SDLK_MEDIA_NEXT_TRACK,
    MEDIA_PREVIOUS_TRACK = SDLK_MEDIA_PREVIOUS_TRACK,
    MEDIA_STOP           = SDLK_MEDIA_STOP,
    MEDIA_EJECT          = SDLK_MEDIA_EJECT,
    MEDIA_PLAY_PAUSE     = SDLK_MEDIA_PLAY_PAUSE,
    MEDIA_SELECT         = SDLK_MEDIA_SELECT,
    AC_NEW               = SDLK_AC_NEW,
    AC_OPEN              = SDLK_AC_OPEN,
    AC_CLOSE             = SDLK_AC_CLOSE,
    AC_EXIT              = SDLK_AC_EXIT,
    AC_SAVE              = SDLK_AC_SAVE,
    AC_PRINT             = SDLK_AC_PRINT,
    AC_PROPERTIES        = SDLK_AC_PROPERTIES,
    AC_SEARCH            = SDLK_AC_SEARCH,
    AC_HOME              = SDLK_AC_HOME,
    AC_BACK              = SDLK_AC_BACK,
    AC_FORWARD           = SDLK_AC_FORWARD,
    AC_STOP              = SDLK_AC_STOP,
    AC_REFRESH           = SDLK_AC_REFRESH,
    AC_BOOKMARKS         = SDLK_AC_BOOKMARKS,
    SOFTLEFT             = SDLK_SOFTLEFT,
    SOFTRIGHT            = SDLK_SOFTRIGHT,
    CALL                 = SDLK_CALL,
    ENDCALL              = SDLK_ENDCALL,
    LEFT_TAB             = SDLK_LEFT_TAB,
    LEVEL5_SHIFT         = SDLK_LEVEL5_SHIFT,
    MULTI_KEY_COMPOSE    = SDLK_MULTI_KEY_COMPOSE,
    LMETA                = SDLK_LMETA,
    RMETA                = SDLK_RMETA,
    LHYPER               = SDLK_LHYPER,
    RHYPER               = SDLK_RHYPER,
};
BITFLAG_ENUM(Keycode);

enum class Keymod : Uint16
{
    KMOD_NONE   = SDL_KMOD_NONE,
    KMOD_LSHIFT = SDL_KMOD_LSHIFT,
    KMOD_RSHIFT = SDL_KMOD_RSHIFT,
    KMOD_LEVEL5 = SDL_KMOD_LEVEL5,
    KMOD_LCTRL  = SDL_KMOD_LCTRL,
    KMOD_RCTRL  = SDL_KMOD_RCTRL,
    KMOD_LALT   = SDL_KMOD_LALT,
    KMOD_RALT   = SDL_KMOD_RALT,
    KMOD_LGUI   = SDL_KMOD_LGUI,
    KMOD_RGUI   = SDL_KMOD_RGUI,
    KMOD_NUM    = SDL_KMOD_NUM,
    KMOD_CAPS   = SDL_KMOD_CAPS,
    KMOD_MODE   = SDL_KMOD_MODE,
    KMOD_SCROLL = SDL_KMOD_SCROLL,
    KMOD_CTRL   = SDL_KMOD_CTRL,
    KMOD_SHIFT  = SDL_KMOD_SHIFT,
    KMOD_ALT    = SDL_KMOD_ALT,
    KMOD_GUI    = SDL_KMOD_GUI,
};
BITFLAG_ENUM(Keymod);

using KeyboardID = Uint32;

bool HasKeyboard(void)
{
    return SDL_HasKeyboard();
}

SDL_KeyboardID* GetKeyboards(int* count)
{
    return SDL_GetKeyboards(count);
}

const char* GetKeyboardNameForID(SDL_KeyboardID instance_id)
{
    return SDL_GetKeyboardNameForID(instance_id);
}

SDL_Window* GetKeyboardFocus(void)
{
    return SDL_GetKeyboardFocus();
}

const bool* GetKeyboardState(int* numkeys)
{
    return SDL_GetKeyboardState(numkeys);
}

void ResetKeyboard(void)
{
    SDL_ResetKeyboard();
}

SDL_Keymod GetModState(void)
{
    return SDL_GetModState();
}

void SetModState(Keymod modstate)
{
    SDL_SetModState((SDL_Keymod)(modstate));
}

SDL_Keycode GetKeyFromScancode(Scancode scancode,
                               Keymod   modstate,
                               bool     key_event)
{
    return SDL_GetKeyFromScancode(
        (SDL_Scancode)(scancode), (SDL_Keymod)(modstate), key_event);
}

SDL_Scancode GetScancodeFromKey(Keycode key, Keymod* modstate)
{
    return SDL_GetScancodeFromKey((SDL_Keycode)(key), (SDL_Keymod*)(modstate));
}

bool SetScancodeName(Scancode scancode, const char* name)
{
    return SDL_SetScancodeName((SDL_Scancode)(scancode), name);
}

const char* GetScancodeName(Scancode scancode)
{
    return SDL_GetScancodeName((SDL_Scancode)(scancode));
}

SDL_Scancode GetScancodeFromName(const char* name)
{
    return SDL_GetScancodeFromName(name);
}

const char* GetKeyName(Keycode key)
{
    return SDL_GetKeyName((SDL_Keycode)(key));
}

SDL_Keycode GetKeyFromName(const char* name)
{
    return SDL_GetKeyFromName(name);
}

bool StartTextInput(SDL_Window* window)
{
    return SDL_StartTextInput(window);
}

enum class TextInputType
{
    TYPE_TEXT                    = SDL_TEXTINPUT_TYPE_TEXT,
    TYPE_TEXT_NAME               = SDL_TEXTINPUT_TYPE_TEXT_NAME,
    TYPE_TEXT_EMAIL              = SDL_TEXTINPUT_TYPE_TEXT_EMAIL,
    TYPE_TEXT_USERNAME           = SDL_TEXTINPUT_TYPE_TEXT_USERNAME,
    TYPE_TEXT_PASSWORD_HIDDEN    = SDL_TEXTINPUT_TYPE_TEXT_PASSWORD_HIDDEN,
    TYPE_TEXT_PASSWORD_VISIBLE   = SDL_TEXTINPUT_TYPE_TEXT_PASSWORD_VISIBLE,
    TYPE_NUMBER                  = SDL_TEXTINPUT_TYPE_NUMBER,
    TYPE_NUMBER_PASSWORD_HIDDEN  = SDL_TEXTINPUT_TYPE_NUMBER_PASSWORD_HIDDEN,
    TYPE_NUMBER_PASSWORD_VISIBLE = SDL_TEXTINPUT_TYPE_NUMBER_PASSWORD_VISIBLE,
};
REGULAR_ENUM(TextInputType);

enum class Capitalization
{
    CAPITALIZE_NONE      = SDL_CAPITALIZE_NONE,
    CAPITALIZE_SENTENCES = SDL_CAPITALIZE_SENTENCES,
    CAPITALIZE_WORDS     = SDL_CAPITALIZE_WORDS,
    CAPITALIZE_LETTERS   = SDL_CAPITALIZE_LETTERS,
};
REGULAR_ENUM(Capitalization);

bool StartTextInputWithProperties(SDL_Window* window, SDL_PropertiesID props)
{
    return SDL_StartTextInputWithProperties(window, props);
}

constexpr auto PROP_TEXTINPUT_TYPE_NUMBER()
{
    return "SDL.textinput.type";
}

constexpr auto PROP_TEXTINPUT_CAPITALIZATION_NUMBER()
{
    return "SDL.textinput.capitalization";
}

constexpr auto PROP_TEXTINPUT_AUTOCORRECT_BOOLEAN()
{
    return "SDL.textinput.autocorrect";
}

constexpr auto PROP_TEXTINPUT_MULTILINE_BOOLEAN()
{
    return "SDL.textinput.multiline";
}

constexpr auto PROP_TEXTINPUT_ANDROID_INPUTTYPE_NUMBER()
{
    return "SDL.textinput.android.inputtype";
}

bool TextInputActive(SDL_Window* window)
{
    return SDL_TextInputActive(window);
}

bool StopTextInput(SDL_Window* window)
{
    return SDL_StopTextInput(window);
}

bool ClearComposition(SDL_Window* window)
{
    return SDL_ClearComposition(window);
}

bool SetTextInputArea(SDL_Window* window, const SDL_Rect* rect, int cursor)
{
    return SDL_SetTextInputArea(window, rect, cursor);
}

bool GetTextInputArea(SDL_Window* window, SDL_Rect* rect, int* cursor)
{
    return SDL_GetTextInputArea(window, rect, cursor);
}

bool HasScreenKeyboardSupport(void)
{
    return SDL_HasScreenKeyboardSupport();
}

bool ScreenKeyboardShown(SDL_Window* window)
{
    return SDL_ScreenKeyboardShown(window);
}

using MouseID = Uint32;

using SDL_Cursor = SDL_Cursor;

enum class SystemCursor
{
    CURSOR_DEFAULT     = SDL_SYSTEM_CURSOR_DEFAULT,
    CURSOR_TEXT        = SDL_SYSTEM_CURSOR_TEXT,
    CURSOR_WAIT        = SDL_SYSTEM_CURSOR_WAIT,
    CURSOR_CROSSHAIR   = SDL_SYSTEM_CURSOR_CROSSHAIR,
    CURSOR_PROGRESS    = SDL_SYSTEM_CURSOR_PROGRESS,
    CURSOR_NWSE_RESIZE = SDL_SYSTEM_CURSOR_NWSE_RESIZE,
    CURSOR_NESW_RESIZE = SDL_SYSTEM_CURSOR_NESW_RESIZE,
    CURSOR_EW_RESIZE   = SDL_SYSTEM_CURSOR_EW_RESIZE,
    CURSOR_NS_RESIZE   = SDL_SYSTEM_CURSOR_NS_RESIZE,
    CURSOR_MOVE        = SDL_SYSTEM_CURSOR_MOVE,
    CURSOR_NOT_ALLOWED = SDL_SYSTEM_CURSOR_NOT_ALLOWED,
    CURSOR_POINTER     = SDL_SYSTEM_CURSOR_POINTER,
    CURSOR_NW_RESIZE   = SDL_SYSTEM_CURSOR_NW_RESIZE,
    CURSOR_N_RESIZE    = SDL_SYSTEM_CURSOR_N_RESIZE,
    CURSOR_NE_RESIZE   = SDL_SYSTEM_CURSOR_NE_RESIZE,
    CURSOR_E_RESIZE    = SDL_SYSTEM_CURSOR_E_RESIZE,
    CURSOR_SE_RESIZE   = SDL_SYSTEM_CURSOR_SE_RESIZE,
    CURSOR_S_RESIZE    = SDL_SYSTEM_CURSOR_S_RESIZE,
    CURSOR_SW_RESIZE   = SDL_SYSTEM_CURSOR_SW_RESIZE,
    CURSOR_W_RESIZE    = SDL_SYSTEM_CURSOR_W_RESIZE,
    CURSOR_COUNT       = SDL_SYSTEM_CURSOR_COUNT,
};
REGULAR_ENUM(SystemCursor);

enum class MouseWheelDirection
{
    NORMAL  = SDL_MOUSEWHEEL_NORMAL,
    FLIPPED = SDL_MOUSEWHEEL_FLIPPED,
};
REGULAR_ENUM(MouseWheelDirection);

enum class MouseButtonFlags : Uint32
{
    BUTTON_LEFT   = SDL_BUTTON_LEFT,
    BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    BUTTON_RIGHT  = SDL_BUTTON_RIGHT,
    BUTTON_X1     = SDL_BUTTON_X1,
    BUTTON_X2     = SDL_BUTTON_X2,
    BUTTON_LMASK  = SDL_BUTTON_LMASK,
    BUTTON_MMASK  = SDL_BUTTON_MMASK,
    BUTTON_RMASK  = SDL_BUTTON_RMASK,
    BUTTON_X1MASK = SDL_BUTTON_X1MASK,
    BUTTON_X2MASK = SDL_BUTTON_X2MASK,
};
BITFLAG_ENUM(MouseButtonFlags);

bool HasMouse(void)
{
    return SDL_HasMouse();
}

SDL_MouseID* GetMice(int* count)
{
    return SDL_GetMice(count);
}

const char* GetMouseNameForID(SDL_MouseID instance_id)
{
    return SDL_GetMouseNameForID(instance_id);
}

SDL_Window* GetMouseFocus(void)
{
    return SDL_GetMouseFocus();
}

SDL_MouseButtonFlags GetMouseState(float* x, float* y)
{
    return SDL_GetMouseState(x, y);
}

SDL_MouseButtonFlags GetGlobalMouseState(float* x, float* y)
{
    return SDL_GetGlobalMouseState(x, y);
}

SDL_MouseButtonFlags GetRelativeMouseState(float* x, float* y)
{
    return SDL_GetRelativeMouseState(x, y);
}

void WarpMouseInWindow(SDL_Window* window, float x, float y)
{
    SDL_WarpMouseInWindow(window, x, y);
}

bool WarpMouseGlobal(float x, float y)
{
    return SDL_WarpMouseGlobal(x, y);
}

bool SetRelativeMouseTransform(SDL_MouseMotionTransformCallback callback,
                               void*                            userdata)
{
    return SDL_SetRelativeMouseTransform(callback, userdata);
}

bool SetWindowRelativeMouseMode(SDL_Window* window, bool enabled)
{
    return SDL_SetWindowRelativeMouseMode(window, enabled);
}

bool GetWindowRelativeMouseMode(SDL_Window* window)
{
    return SDL_GetWindowRelativeMouseMode(window);
}

bool CaptureMouse(bool enabled)
{
    return SDL_CaptureMouse(enabled);
}

SDL_Cursor* CreateCursor(
    const Uint8* data, const Uint8* mask, int w, int h, int hot_x, int hot_y)
{
    return SDL_CreateCursor(data, mask, w, h, hot_x, hot_y);
}

SDL_Cursor* CreateColorCursor(SDL_Surface* surface, int hot_x, int hot_y)
{
    return SDL_CreateColorCursor(surface, hot_x, hot_y);
}

SDL_Cursor* CreateSystemCursor(SystemCursor id)
{
    return SDL_CreateSystemCursor((SDL_SystemCursor)(id));
}

bool SetCursor(SDL_Cursor* cursor)
{
    return SDL_SetCursor(cursor);
}

SDL_Cursor* GetCursor(void)
{
    return SDL_GetCursor();
}

SDL_Cursor* GetDefaultCursor(void)
{
    return SDL_GetDefaultCursor();
}

void DestroyCursor(SDL_Cursor* cursor)
{
    SDL_DestroyCursor(cursor);
}

bool ShowCursor(void)
{
    return SDL_ShowCursor();
}

bool HideCursor(void)
{
    return SDL_HideCursor();
}

bool CursorVisible(void)
{
    return SDL_CursorVisible();
}

using TouchID = Uint64;

using FingerID = Uint64;

enum class TouchDeviceType
{
    DEVICE_INVALID           = SDL_TOUCH_DEVICE_INVALID,
    DEVICE_DIRECT            = SDL_TOUCH_DEVICE_DIRECT,
    DEVICE_INDIRECT_ABSOLUTE = SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE,
    DEVICE_INDIRECT_RELATIVE = SDL_TOUCH_DEVICE_INDIRECT_RELATIVE,
};
REGULAR_ENUM(TouchDeviceType);

using Finger = SDL_Finger;

constexpr auto TOUCH_MOUSEID()
{
    return ((SDL_MouseID)-1);
}

constexpr auto MOUSE_TOUCHID()
{
    return ((SDL_TouchID)-1);
}

SDL_TouchID* GetTouchDevices(int* count)
{
    return SDL_GetTouchDevices(count);
}

const char* GetTouchDeviceName(SDL_TouchID touchID)
{
    return SDL_GetTouchDeviceName(touchID);
}

SDL_TouchDeviceType GetTouchDeviceType(SDL_TouchID touchID)
{
    return SDL_GetTouchDeviceType(touchID);
}

using PenID = Uint32;

constexpr auto PEN_MOUSEID()
{
    return ((SDL_MouseID)-2);
}

constexpr auto PEN_TOUCHID()
{
    return ((SDL_TouchID)-2);
}

enum class PenInputFlags : Uint32
{
    INPUT_DOWN       = SDL_PEN_INPUT_DOWN,
    INPUT_BUTTON_1   = SDL_PEN_INPUT_BUTTON_1,
    INPUT_BUTTON_2   = SDL_PEN_INPUT_BUTTON_2,
    INPUT_BUTTON_3   = SDL_PEN_INPUT_BUTTON_3,
    INPUT_BUTTON_4   = SDL_PEN_INPUT_BUTTON_4,
    INPUT_BUTTON_5   = SDL_PEN_INPUT_BUTTON_5,
    INPUT_ERASER_TIP = SDL_PEN_INPUT_ERASER_TIP,
};
BITFLAG_ENUM(PenInputFlags);

enum class PenAxis
{
    AXIS_PRESSURE            = SDL_PEN_AXIS_PRESSURE,
    AXIS_XTILT               = SDL_PEN_AXIS_XTILT,
    AXIS_YTILT               = SDL_PEN_AXIS_YTILT,
    AXIS_DISTANCE            = SDL_PEN_AXIS_DISTANCE,
    AXIS_ROTATION            = SDL_PEN_AXIS_ROTATION,
    AXIS_SLIDER              = SDL_PEN_AXIS_SLIDER,
    AXIS_TANGENTIAL_PRESSURE = SDL_PEN_AXIS_TANGENTIAL_PRESSURE,
    AXIS_COUNT               = SDL_PEN_AXIS_COUNT,
};
REGULAR_ENUM(PenAxis);

enum class EventType
{
    FIRST                         = SDL_EVENT_FIRST,
    QUIT                          = SDL_EVENT_QUIT,
    TERMINATING                   = SDL_EVENT_TERMINATING,
    LOW_MEMORY                    = SDL_EVENT_LOW_MEMORY,
    WILL_ENTER_BACKGROUND         = SDL_EVENT_WILL_ENTER_BACKGROUND,
    DID_ENTER_BACKGROUND          = SDL_EVENT_DID_ENTER_BACKGROUND,
    WILL_ENTER_FOREGROUND         = SDL_EVENT_WILL_ENTER_FOREGROUND,
    DID_ENTER_FOREGROUND          = SDL_EVENT_DID_ENTER_FOREGROUND,
    LOCALE_CHANGED                = SDL_EVENT_LOCALE_CHANGED,
    SYSTEM_THEME_CHANGED          = SDL_EVENT_SYSTEM_THEME_CHANGED,
    DISPLAY_ORIENTATION           = SDL_EVENT_DISPLAY_ORIENTATION,
    DISPLAY_ADDED                 = SDL_EVENT_DISPLAY_ADDED,
    DISPLAY_REMOVED               = SDL_EVENT_DISPLAY_REMOVED,
    DISPLAY_MOVED                 = SDL_EVENT_DISPLAY_MOVED,
    DISPLAY_DESKTOP_MODE_CHANGED  = SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED,
    DISPLAY_CURRENT_MODE_CHANGED  = SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED,
    DISPLAY_CONTENT_SCALE_CHANGED = SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED,
    DISPLAY_FIRST                 = SDL_EVENT_DISPLAY_FIRST,
    DISPLAY_LAST                  = SDL_EVENT_DISPLAY_LAST,
    WINDOW_SHOWN                  = SDL_EVENT_WINDOW_SHOWN,
    WINDOW_HIDDEN                 = SDL_EVENT_WINDOW_HIDDEN,
    WINDOW_EXPOSED                = SDL_EVENT_WINDOW_EXPOSED,
    WINDOW_MOVED                  = SDL_EVENT_WINDOW_MOVED,
    WINDOW_RESIZED                = SDL_EVENT_WINDOW_RESIZED,
    WINDOW_PIXEL_SIZE_CHANGED     = SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED,
    WINDOW_METAL_VIEW_RESIZED     = SDL_EVENT_WINDOW_METAL_VIEW_RESIZED,
    WINDOW_MINIMIZED              = SDL_EVENT_WINDOW_MINIMIZED,
    WINDOW_MAXIMIZED              = SDL_EVENT_WINDOW_MAXIMIZED,
    WINDOW_RESTORED               = SDL_EVENT_WINDOW_RESTORED,
    WINDOW_MOUSE_ENTER            = SDL_EVENT_WINDOW_MOUSE_ENTER,
    WINDOW_MOUSE_LEAVE            = SDL_EVENT_WINDOW_MOUSE_LEAVE,
    WINDOW_FOCUS_GAINED           = SDL_EVENT_WINDOW_FOCUS_GAINED,
    WINDOW_FOCUS_LOST             = SDL_EVENT_WINDOW_FOCUS_LOST,
    WINDOW_CLOSE_REQUESTED        = SDL_EVENT_WINDOW_CLOSE_REQUESTED,
    WINDOW_HIT_TEST               = SDL_EVENT_WINDOW_HIT_TEST,
    WINDOW_ICCPROF_CHANGED        = SDL_EVENT_WINDOW_ICCPROF_CHANGED,
    WINDOW_DISPLAY_CHANGED        = SDL_EVENT_WINDOW_DISPLAY_CHANGED,
    WINDOW_DISPLAY_SCALE_CHANGED  = SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED,
    WINDOW_SAFE_AREA_CHANGED      = SDL_EVENT_WINDOW_SAFE_AREA_CHANGED,
    WINDOW_OCCLUDED               = SDL_EVENT_WINDOW_OCCLUDED,
    WINDOW_ENTER_FULLSCREEN       = SDL_EVENT_WINDOW_ENTER_FULLSCREEN,
    WINDOW_LEAVE_FULLSCREEN       = SDL_EVENT_WINDOW_LEAVE_FULLSCREEN,
    WINDOW_DESTROYED              = SDL_EVENT_WINDOW_DESTROYED,
    WINDOW_HDR_STATE_CHANGED      = SDL_EVENT_WINDOW_HDR_STATE_CHANGED,
    WINDOW_FIRST                  = SDL_EVENT_WINDOW_FIRST,
    WINDOW_LAST                   = SDL_EVENT_WINDOW_LAST,
    KEY_DOWN                      = SDL_EVENT_KEY_DOWN,
    KEY_UP                        = SDL_EVENT_KEY_UP,
    TEXT_EDITING                  = SDL_EVENT_TEXT_EDITING,
    TEXT_INPUT                    = SDL_EVENT_TEXT_INPUT,
    KEYMAP_CHANGED                = SDL_EVENT_KEYMAP_CHANGED,
    KEYBOARD_ADDED                = SDL_EVENT_KEYBOARD_ADDED,
    KEYBOARD_REMOVED              = SDL_EVENT_KEYBOARD_REMOVED,
    TEXT_EDITING_CANDIDATES       = SDL_EVENT_TEXT_EDITING_CANDIDATES,
    MOUSE_MOTION                  = SDL_EVENT_MOUSE_MOTION,
    MOUSE_BUTTON_DOWN             = SDL_EVENT_MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP               = SDL_EVENT_MOUSE_BUTTON_UP,
    MOUSE_WHEEL                   = SDL_EVENT_MOUSE_WHEEL,
    MOUSE_ADDED                   = SDL_EVENT_MOUSE_ADDED,
    MOUSE_REMOVED                 = SDL_EVENT_MOUSE_REMOVED,
    JOYSTICK_AXIS_MOTION          = SDL_EVENT_JOYSTICK_AXIS_MOTION,
    JOYSTICK_BALL_MOTION          = SDL_EVENT_JOYSTICK_BALL_MOTION,
    JOYSTICK_HAT_MOTION           = SDL_EVENT_JOYSTICK_HAT_MOTION,
    JOYSTICK_BUTTON_DOWN          = SDL_EVENT_JOYSTICK_BUTTON_DOWN,
    JOYSTICK_BUTTON_UP            = SDL_EVENT_JOYSTICK_BUTTON_UP,
    JOYSTICK_ADDED                = SDL_EVENT_JOYSTICK_ADDED,
    JOYSTICK_REMOVED              = SDL_EVENT_JOYSTICK_REMOVED,
    JOYSTICK_BATTERY_UPDATED      = SDL_EVENT_JOYSTICK_BATTERY_UPDATED,
    JOYSTICK_UPDATE_COMPLETE      = SDL_EVENT_JOYSTICK_UPDATE_COMPLETE,
    GAMEPAD_AXIS_MOTION           = SDL_EVENT_GAMEPAD_AXIS_MOTION,
    GAMEPAD_BUTTON_DOWN           = SDL_EVENT_GAMEPAD_BUTTON_DOWN,
    GAMEPAD_BUTTON_UP             = SDL_EVENT_GAMEPAD_BUTTON_UP,
    GAMEPAD_ADDED                 = SDL_EVENT_GAMEPAD_ADDED,
    GAMEPAD_REMOVED               = SDL_EVENT_GAMEPAD_REMOVED,
    GAMEPAD_REMAPPED              = SDL_EVENT_GAMEPAD_REMAPPED,
    GAMEPAD_TOUCHPAD_DOWN         = SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN,
    GAMEPAD_TOUCHPAD_MOTION       = SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION,
    GAMEPAD_TOUCHPAD_UP           = SDL_EVENT_GAMEPAD_TOUCHPAD_UP,
    GAMEPAD_SENSOR_UPDATE         = SDL_EVENT_GAMEPAD_SENSOR_UPDATE,
    GAMEPAD_UPDATE_COMPLETE       = SDL_EVENT_GAMEPAD_UPDATE_COMPLETE,
    GAMEPAD_STEAM_HANDLE_UPDATED  = SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED,
    FINGER_DOWN                   = SDL_EVENT_FINGER_DOWN,
    FINGER_UP                     = SDL_EVENT_FINGER_UP,
    FINGER_MOTION                 = SDL_EVENT_FINGER_MOTION,
    FINGER_CANCELED               = SDL_EVENT_FINGER_CANCELED,
    CLIPBOARD_UPDATE              = SDL_EVENT_CLIPBOARD_UPDATE,
    DROP_FILE                     = SDL_EVENT_DROP_FILE,
    DROP_TEXT                     = SDL_EVENT_DROP_TEXT,
    DROP_BEGIN                    = SDL_EVENT_DROP_BEGIN,
    DROP_COMPLETE                 = SDL_EVENT_DROP_COMPLETE,
    DROP_POSITION                 = SDL_EVENT_DROP_POSITION,
    AUDIO_DEVICE_ADDED            = SDL_EVENT_AUDIO_DEVICE_ADDED,
    AUDIO_DEVICE_REMOVED          = SDL_EVENT_AUDIO_DEVICE_REMOVED,
    AUDIO_DEVICE_FORMAT_CHANGED   = SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED,
    SENSOR_UPDATE                 = SDL_EVENT_SENSOR_UPDATE,
    PEN_PROXIMITY_IN              = SDL_EVENT_PEN_PROXIMITY_IN,
    PEN_PROXIMITY_OUT             = SDL_EVENT_PEN_PROXIMITY_OUT,
    PEN_DOWN                      = SDL_EVENT_PEN_DOWN,
    PEN_UP                        = SDL_EVENT_PEN_UP,
    PEN_BUTTON_DOWN               = SDL_EVENT_PEN_BUTTON_DOWN,
    PEN_BUTTON_UP                 = SDL_EVENT_PEN_BUTTON_UP,
    PEN_MOTION                    = SDL_EVENT_PEN_MOTION,
    PEN_AXIS                      = SDL_EVENT_PEN_AXIS,
    CAMERA_DEVICE_ADDED           = SDL_EVENT_CAMERA_DEVICE_ADDED,
    CAMERA_DEVICE_REMOVED         = SDL_EVENT_CAMERA_DEVICE_REMOVED,
    CAMERA_DEVICE_APPROVED        = SDL_EVENT_CAMERA_DEVICE_APPROVED,
    CAMERA_DEVICE_DENIED          = SDL_EVENT_CAMERA_DEVICE_DENIED,
    RENDER_TARGETS_RESET          = SDL_EVENT_RENDER_TARGETS_RESET,
    RENDER_DEVICE_RESET           = SDL_EVENT_RENDER_DEVICE_RESET,
    RENDER_DEVICE_LOST            = SDL_EVENT_RENDER_DEVICE_LOST,
    PRIVATE0                      = SDL_EVENT_PRIVATE0,
    PRIVATE1                      = SDL_EVENT_PRIVATE1,
    PRIVATE2                      = SDL_EVENT_PRIVATE2,
    PRIVATE3                      = SDL_EVENT_PRIVATE3,
    POLL_SENTINEL                 = SDL_EVENT_POLL_SENTINEL,
    USER                          = SDL_EVENT_USER,
    LAST                          = SDL_EVENT_LAST,
    ENUM_PADDING                  = SDL_EVENT_ENUM_PADDING,
};
REGULAR_ENUM(EventType);

using CommonEvent = SDL_CommonEvent;

using DisplayEvent = SDL_DisplayEvent;

using WindowEvent = SDL_WindowEvent;

using KeyboardDeviceEvent = SDL_KeyboardDeviceEvent;

using KeyboardEvent = SDL_KeyboardEvent;

using TextEditingEvent = SDL_TextEditingEvent;

using TextEditingCandidatesEvent = SDL_TextEditingCandidatesEvent;

using TextInputEvent = SDL_TextInputEvent;

using MouseDeviceEvent = SDL_MouseDeviceEvent;

using MouseMotionEvent = SDL_MouseMotionEvent;

using MouseButtonEvent = SDL_MouseButtonEvent;

using MouseWheelEvent = SDL_MouseWheelEvent;

using JoyAxisEvent = SDL_JoyAxisEvent;

using JoyBallEvent = SDL_JoyBallEvent;

using JoyHatEvent = SDL_JoyHatEvent;

using JoyButtonEvent = SDL_JoyButtonEvent;

using JoyDeviceEvent = SDL_JoyDeviceEvent;

using JoyBatteryEvent = SDL_JoyBatteryEvent;

using GamepadAxisEvent = SDL_GamepadAxisEvent;

using GamepadButtonEvent = SDL_GamepadButtonEvent;

using GamepadDeviceEvent = SDL_GamepadDeviceEvent;

using GamepadTouchpadEvent = SDL_GamepadTouchpadEvent;

using GamepadSensorEvent = SDL_GamepadSensorEvent;

using AudioDeviceEvent = SDL_AudioDeviceEvent;

using CameraDeviceEvent = SDL_CameraDeviceEvent;

using RenderEvent = SDL_RenderEvent;

using TouchFingerEvent = SDL_TouchFingerEvent;

using PenProximityEvent = SDL_PenProximityEvent;

using PenMotionEvent = SDL_PenMotionEvent;

using PenTouchEvent = SDL_PenTouchEvent;

using PenButtonEvent = SDL_PenButtonEvent;

using PenAxisEvent = SDL_PenAxisEvent;

using DropEvent = SDL_DropEvent;

using ClipboardEvent = SDL_ClipboardEvent;

using SensorEvent = SDL_SensorEvent;

using QuitEvent = SDL_QuitEvent;

using UserEvent = SDL_UserEvent;

using Event = SDL_Event;

void PumpEvents(void)
{
    SDL_PumpEvents();
}

enum class EventAction
{
    ADDEVENT  = SDL_ADDEVENT,
    PEEKEVENT = SDL_PEEKEVENT,
    GETEVENT  = SDL_GETEVENT,
};
REGULAR_ENUM(EventAction);

int PeepEvents(SDL_Event*  events,
               int         numevents,
               EventAction action,
               Uint32      minType,
               Uint32      maxType)
{
    return SDL_PeepEvents(
        events, numevents, (SDL_EventAction)(action), minType, maxType);
}

bool HasEvent(Uint32 type)
{
    return SDL_HasEvent(type);
}

bool HasEvents(Uint32 minType, Uint32 maxType)
{
    return SDL_HasEvents(minType, maxType);
}

void FlushEvent(Uint32 type)
{
    SDL_FlushEvent(type);
}

void FlushEvents(Uint32 minType, Uint32 maxType)
{
    SDL_FlushEvents(minType, maxType);
}

bool PollEvent(SDL_Event* event)
{
    return SDL_PollEvent(event);
}

bool WaitEvent(SDL_Event* event)
{
    return SDL_WaitEvent(event);
}

bool WaitEventTimeout(SDL_Event* event, Sint32 timeoutMS)
{
    return SDL_WaitEventTimeout(event, timeoutMS);
}

bool PushEvent(SDL_Event* event)
{
    return SDL_PushEvent(event);
}

void SetEventFilter(SDL_EventFilter filter, void* userdata)
{
    SDL_SetEventFilter(filter, userdata);
}

bool GetEventFilter(SDL_EventFilter* filter, void** userdata)
{
    return SDL_GetEventFilter(filter, userdata);
}

bool AddEventWatch(SDL_EventFilter filter, void* userdata)
{
    return SDL_AddEventWatch(filter, userdata);
}

void RemoveEventWatch(SDL_EventFilter filter, void* userdata)
{
    SDL_RemoveEventWatch(filter, userdata);
}

void FilterEvents(SDL_EventFilter filter, void* userdata)
{
    SDL_FilterEvents(filter, userdata);
}

void SetEventEnabled(Uint32 type, bool enabled)
{
    SDL_SetEventEnabled(type, enabled);
}

bool EventEnabled(Uint32 type)
{
    return SDL_EventEnabled(type);
}

Uint32 RegisterEvents(int numevents)
{
    return SDL_RegisterEvents(numevents);
}

SDL_Window* GetWindowFromEvent(const SDL_Event* event)
{
    return SDL_GetWindowFromEvent(event);
}

int GetEventDescription(const SDL_Event* event, char* buf, int buflen)
{
    return SDL_GetEventDescription(event, buf, buflen);
}

const char* GetBasePath(void)
{
    return SDL_GetBasePath();
}

char* GetPrefPath(const char* org, const char* app)
{
    return SDL_GetPrefPath(org, app);
}

enum class Folder
{
    HOME        = SDL_FOLDER_HOME,
    DESKTOP     = SDL_FOLDER_DESKTOP,
    DOCUMENTS   = SDL_FOLDER_DOCUMENTS,
    DOWNLOADS   = SDL_FOLDER_DOWNLOADS,
    MUSIC       = SDL_FOLDER_MUSIC,
    PICTURES    = SDL_FOLDER_PICTURES,
    PUBLICSHARE = SDL_FOLDER_PUBLICSHARE,
    SAVEDGAMES  = SDL_FOLDER_SAVEDGAMES,
    SCREENSHOTS = SDL_FOLDER_SCREENSHOTS,
    TEMPLATES   = SDL_FOLDER_TEMPLATES,
    VIDEOS      = SDL_FOLDER_VIDEOS,
    COUNT       = SDL_FOLDER_COUNT,
};
REGULAR_ENUM(Folder);

const char* GetUserFolder(Folder folder)
{
    return SDL_GetUserFolder((SDL_Folder)(folder));
}

enum class PathType
{
    NONE      = SDL_PATHTYPE_NONE,
    FILE      = SDL_PATHTYPE_FILE,
    DIRECTORY = SDL_PATHTYPE_DIRECTORY,
    OTHER     = SDL_PATHTYPE_OTHER,
};
REGULAR_ENUM(PathType);

using PathInfo = SDL_PathInfo;

enum class GlobFlags : Uint32
{
    CASEINSENSITIVE = SDL_GLOB_CASEINSENSITIVE,
};
BITFLAG_ENUM(GlobFlags);

bool CreateDirectory(const char* path)
{
    return SDL_CreateDirectory(path);
}

enum class EnumerationResult
{
    CONTINUE = SDL_ENUM_CONTINUE,
    SUCCESS  = SDL_ENUM_SUCCESS,
    FAILURE  = SDL_ENUM_FAILURE,
};
REGULAR_ENUM(EnumerationResult);

bool EnumerateDirectory(const char*                    path,
                        SDL_EnumerateDirectoryCallback callback,
                        void*                          userdata)
{
    return SDL_EnumerateDirectory(path, callback, userdata);
}

bool RemovePath(const char* path)
{
    return SDL_RemovePath(path);
}

bool RenamePath(const char* oldpath, const char* newpath)
{
    return SDL_RenamePath(oldpath, newpath);
}

bool CopyFile(const char* oldpath, const char* newpath)
{
    return SDL_CopyFile(oldpath, newpath);
}

bool GetPathInfo(const char* path, SDL_PathInfo* info)
{
    return SDL_GetPathInfo(path, info);
}

char* GetCurrentDirectory(void)
{
    return SDL_GetCurrentDirectory();
}

using SDL_GPUDevice = SDL_GPUDevice;

using SDL_GPUBuffer = SDL_GPUBuffer;

using SDL_GPUTransferBuffer = SDL_GPUTransferBuffer;

using SDL_GPUTexture = SDL_GPUTexture;

using SDL_GPUSampler = SDL_GPUSampler;

using SDL_GPUShader = SDL_GPUShader;

using SDL_GPUComputePipeline = SDL_GPUComputePipeline;

using SDL_GPUGraphicsPipeline = SDL_GPUGraphicsPipeline;

using SDL_GPUCommandBuffer = SDL_GPUCommandBuffer;

using SDL_GPURenderPass = SDL_GPURenderPass;

using SDL_GPUComputePass = SDL_GPUComputePass;

using SDL_GPUCopyPass = SDL_GPUCopyPass;

using SDL_GPUFence = SDL_GPUFence;

enum class GPUPrimitiveType
{
    PRIMITIVETYPE_TRIANGLELIST  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    PRIMITIVETYPE_TRIANGLESTRIP = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP,
    PRIMITIVETYPE_LINELIST      = SDL_GPU_PRIMITIVETYPE_LINELIST,
    PRIMITIVETYPE_LINESTRIP     = SDL_GPU_PRIMITIVETYPE_LINESTRIP,
    PRIMITIVETYPE_POINTLIST     = SDL_GPU_PRIMITIVETYPE_POINTLIST,
};
REGULAR_ENUM(GPUPrimitiveType);

enum class GPULoadOp
{
    LOADOP_LOAD      = SDL_GPU_LOADOP_LOAD,
    LOADOP_CLEAR     = SDL_GPU_LOADOP_CLEAR,
    LOADOP_DONT_CARE = SDL_GPU_LOADOP_DONT_CARE,
};
REGULAR_ENUM(GPULoadOp);

enum class GPUStoreOp
{
    STOREOP_STORE             = SDL_GPU_STOREOP_STORE,
    STOREOP_DONT_CARE         = SDL_GPU_STOREOP_DONT_CARE,
    STOREOP_RESOLVE           = SDL_GPU_STOREOP_RESOLVE,
    STOREOP_RESOLVE_AND_STORE = SDL_GPU_STOREOP_RESOLVE_AND_STORE,
};
REGULAR_ENUM(GPUStoreOp);

enum class GPUIndexElementSize
{
    INDEXELEMENTSIZE_16BIT = SDL_GPU_INDEXELEMENTSIZE_16BIT,
    INDEXELEMENTSIZE_32BIT = SDL_GPU_INDEXELEMENTSIZE_32BIT,
};
REGULAR_ENUM(GPUIndexElementSize);

enum class GPUTextureFormat
{
    TEXTUREFORMAT_INVALID            = SDL_GPU_TEXTUREFORMAT_INVALID,
    TEXTUREFORMAT_A8_UNORM           = SDL_GPU_TEXTUREFORMAT_A8_UNORM,
    TEXTUREFORMAT_R8_UNORM           = SDL_GPU_TEXTUREFORMAT_R8_UNORM,
    TEXTUREFORMAT_R8G8_UNORM         = SDL_GPU_TEXTUREFORMAT_R8G8_UNORM,
    TEXTUREFORMAT_R8G8B8A8_UNORM     = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    TEXTUREFORMAT_R16_UNORM          = SDL_GPU_TEXTUREFORMAT_R16_UNORM,
    TEXTUREFORMAT_R16G16_UNORM       = SDL_GPU_TEXTUREFORMAT_R16G16_UNORM,
    TEXTUREFORMAT_R16G16B16A16_UNORM = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UNORM,
    TEXTUREFORMAT_R10G10B10A2_UNORM  = SDL_GPU_TEXTUREFORMAT_R10G10B10A2_UNORM,
    TEXTUREFORMAT_B5G6R5_UNORM       = SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM,
    TEXTUREFORMAT_B5G5R5A1_UNORM     = SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM,
    TEXTUREFORMAT_B4G4R4A4_UNORM     = SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM,
    TEXTUREFORMAT_B8G8R8A8_UNORM     = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
    TEXTUREFORMAT_BC1_RGBA_UNORM     = SDL_GPU_TEXTUREFORMAT_BC1_RGBA_UNORM,
    TEXTUREFORMAT_BC2_RGBA_UNORM     = SDL_GPU_TEXTUREFORMAT_BC2_RGBA_UNORM,
    TEXTUREFORMAT_BC3_RGBA_UNORM     = SDL_GPU_TEXTUREFORMAT_BC3_RGBA_UNORM,
    TEXTUREFORMAT_BC4_R_UNORM        = SDL_GPU_TEXTUREFORMAT_BC4_R_UNORM,
    TEXTUREFORMAT_BC5_RG_UNORM       = SDL_GPU_TEXTUREFORMAT_BC5_RG_UNORM,
    TEXTUREFORMAT_BC7_RGBA_UNORM     = SDL_GPU_TEXTUREFORMAT_BC7_RGBA_UNORM,
    TEXTUREFORMAT_BC6H_RGB_FLOAT     = SDL_GPU_TEXTUREFORMAT_BC6H_RGB_FLOAT,
    TEXTUREFORMAT_BC6H_RGB_UFLOAT    = SDL_GPU_TEXTUREFORMAT_BC6H_RGB_UFLOAT,
    TEXTUREFORMAT_R8_SNORM           = SDL_GPU_TEXTUREFORMAT_R8_SNORM,
    TEXTUREFORMAT_R8G8_SNORM         = SDL_GPU_TEXTUREFORMAT_R8G8_SNORM,
    TEXTUREFORMAT_R8G8B8A8_SNORM     = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_SNORM,
    TEXTUREFORMAT_R16_SNORM          = SDL_GPU_TEXTUREFORMAT_R16_SNORM,
    TEXTUREFORMAT_R16G16_SNORM       = SDL_GPU_TEXTUREFORMAT_R16G16_SNORM,
    TEXTUREFORMAT_R16G16B16A16_SNORM = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_SNORM,
    TEXTUREFORMAT_R16_FLOAT          = SDL_GPU_TEXTUREFORMAT_R16_FLOAT,
    TEXTUREFORMAT_R16G16_FLOAT       = SDL_GPU_TEXTUREFORMAT_R16G16_FLOAT,
    TEXTUREFORMAT_R16G16B16A16_FLOAT = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT,
    TEXTUREFORMAT_R32_FLOAT          = SDL_GPU_TEXTUREFORMAT_R32_FLOAT,
    TEXTUREFORMAT_R32G32_FLOAT       = SDL_GPU_TEXTUREFORMAT_R32G32_FLOAT,
    TEXTUREFORMAT_R32G32B32A32_FLOAT = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT,
    TEXTUREFORMAT_R11G11B10_UFLOAT   = SDL_GPU_TEXTUREFORMAT_R11G11B10_UFLOAT,
    TEXTUREFORMAT_R8_UINT            = SDL_GPU_TEXTUREFORMAT_R8_UINT,
    TEXTUREFORMAT_R8G8_UINT          = SDL_GPU_TEXTUREFORMAT_R8G8_UINT,
    TEXTUREFORMAT_R8G8B8A8_UINT      = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UINT,
    TEXTUREFORMAT_R16_UINT           = SDL_GPU_TEXTUREFORMAT_R16_UINT,
    TEXTUREFORMAT_R16G16_UINT        = SDL_GPU_TEXTUREFORMAT_R16G16_UINT,
    TEXTUREFORMAT_R16G16B16A16_UINT  = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_UINT,
    TEXTUREFORMAT_R32_UINT           = SDL_GPU_TEXTUREFORMAT_R32_UINT,
    TEXTUREFORMAT_R32G32_UINT        = SDL_GPU_TEXTUREFORMAT_R32G32_UINT,
    TEXTUREFORMAT_R32G32B32A32_UINT  = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_UINT,
    TEXTUREFORMAT_R8_INT             = SDL_GPU_TEXTUREFORMAT_R8_INT,
    TEXTUREFORMAT_R8G8_INT           = SDL_GPU_TEXTUREFORMAT_R8G8_INT,
    TEXTUREFORMAT_R8G8B8A8_INT       = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_INT,
    TEXTUREFORMAT_R16_INT            = SDL_GPU_TEXTUREFORMAT_R16_INT,
    TEXTUREFORMAT_R16G16_INT         = SDL_GPU_TEXTUREFORMAT_R16G16_INT,
    TEXTUREFORMAT_R16G16B16A16_INT   = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_INT,
    TEXTUREFORMAT_R32_INT            = SDL_GPU_TEXTUREFORMAT_R32_INT,
    TEXTUREFORMAT_R32G32_INT         = SDL_GPU_TEXTUREFORMAT_R32G32_INT,
    TEXTUREFORMAT_R32G32B32A32_INT   = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_INT,
    TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB,
    TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB,
    TEXTUREFORMAT_BC1_RGBA_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_BC1_RGBA_UNORM_SRGB,
    TEXTUREFORMAT_BC2_RGBA_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_BC2_RGBA_UNORM_SRGB,
    TEXTUREFORMAT_BC3_RGBA_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_BC3_RGBA_UNORM_SRGB,
    TEXTUREFORMAT_BC7_RGBA_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_BC7_RGBA_UNORM_SRGB,
    TEXTUREFORMAT_D16_UNORM         = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
    TEXTUREFORMAT_D24_UNORM         = SDL_GPU_TEXTUREFORMAT_D24_UNORM,
    TEXTUREFORMAT_D32_FLOAT         = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
    TEXTUREFORMAT_D24_UNORM_S8_UINT = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
    TEXTUREFORMAT_D32_FLOAT_S8_UINT = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
    TEXTUREFORMAT_ASTC_4x4_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_4x4_UNORM,
    TEXTUREFORMAT_ASTC_5x4_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_5x4_UNORM,
    TEXTUREFORMAT_ASTC_5x5_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_5x5_UNORM,
    TEXTUREFORMAT_ASTC_6x5_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_6x5_UNORM,
    TEXTUREFORMAT_ASTC_6x6_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_6x6_UNORM,
    TEXTUREFORMAT_ASTC_8x5_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_8x5_UNORM,
    TEXTUREFORMAT_ASTC_8x6_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_8x6_UNORM,
    TEXTUREFORMAT_ASTC_8x8_UNORM    = SDL_GPU_TEXTUREFORMAT_ASTC_8x8_UNORM,
    TEXTUREFORMAT_ASTC_10x5_UNORM   = SDL_GPU_TEXTUREFORMAT_ASTC_10x5_UNORM,
    TEXTUREFORMAT_ASTC_10x6_UNORM   = SDL_GPU_TEXTUREFORMAT_ASTC_10x6_UNORM,
    TEXTUREFORMAT_ASTC_10x8_UNORM   = SDL_GPU_TEXTUREFORMAT_ASTC_10x8_UNORM,
    TEXTUREFORMAT_ASTC_10x10_UNORM  = SDL_GPU_TEXTUREFORMAT_ASTC_10x10_UNORM,
    TEXTUREFORMAT_ASTC_12x10_UNORM  = SDL_GPU_TEXTUREFORMAT_ASTC_12x10_UNORM,
    TEXTUREFORMAT_ASTC_12x12_UNORM  = SDL_GPU_TEXTUREFORMAT_ASTC_12x12_UNORM,
    TEXTUREFORMAT_ASTC_4x4_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_4x4_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_5x4_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_5x4_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_5x5_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_5x5_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_6x5_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_6x5_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_6x6_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_6x6_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_8x5_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_8x5_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_8x6_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_8x6_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_8x8_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_8x8_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_10x5_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_10x5_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_10x6_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_10x6_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_10x8_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_10x8_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_10x10_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_10x10_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_12x10_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_12x10_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_12x12_UNORM_SRGB =
        SDL_GPU_TEXTUREFORMAT_ASTC_12x12_UNORM_SRGB,
    TEXTUREFORMAT_ASTC_4x4_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_4x4_FLOAT,
    TEXTUREFORMAT_ASTC_5x4_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_5x4_FLOAT,
    TEXTUREFORMAT_ASTC_5x5_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_5x5_FLOAT,
    TEXTUREFORMAT_ASTC_6x5_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_6x5_FLOAT,
    TEXTUREFORMAT_ASTC_6x6_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_6x6_FLOAT,
    TEXTUREFORMAT_ASTC_8x5_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_8x5_FLOAT,
    TEXTUREFORMAT_ASTC_8x6_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_8x6_FLOAT,
    TEXTUREFORMAT_ASTC_8x8_FLOAT   = SDL_GPU_TEXTUREFORMAT_ASTC_8x8_FLOAT,
    TEXTUREFORMAT_ASTC_10x5_FLOAT  = SDL_GPU_TEXTUREFORMAT_ASTC_10x5_FLOAT,
    TEXTUREFORMAT_ASTC_10x6_FLOAT  = SDL_GPU_TEXTUREFORMAT_ASTC_10x6_FLOAT,
    TEXTUREFORMAT_ASTC_10x8_FLOAT  = SDL_GPU_TEXTUREFORMAT_ASTC_10x8_FLOAT,
    TEXTUREFORMAT_ASTC_10x10_FLOAT = SDL_GPU_TEXTUREFORMAT_ASTC_10x10_FLOAT,
    TEXTUREFORMAT_ASTC_12x10_FLOAT = SDL_GPU_TEXTUREFORMAT_ASTC_12x10_FLOAT,
    TEXTUREFORMAT_ASTC_12x12_FLOAT = SDL_GPU_TEXTUREFORMAT_ASTC_12x12_FLOAT,
};
REGULAR_ENUM(GPUTextureFormat);

enum class GPUTextureUsageFlags : Uint32
{
    TEXTUREUSAGE_SAMPLER      = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    TEXTUREUSAGE_COLOR_TARGET = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET,
    TEXTUREUSAGE_DEPTH_STENCIL_TARGET =
        SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    TEXTUREUSAGE_GRAPHICS_STORAGE_READ =
        SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ,
    TEXTUREUSAGE_COMPUTE_STORAGE_READ =
        SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_READ,
    TEXTUREUSAGE_COMPUTE_STORAGE_WRITE =
        SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE,
    TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE =
        SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE,
};
BITFLAG_ENUM(GPUTextureUsageFlags);

enum class GPUTextureType
{
    TEXTURETYPE_2D         = SDL_GPU_TEXTURETYPE_2D,
    TEXTURETYPE_2D_ARRAY   = SDL_GPU_TEXTURETYPE_2D_ARRAY,
    TEXTURETYPE_3D         = SDL_GPU_TEXTURETYPE_3D,
    TEXTURETYPE_CUBE       = SDL_GPU_TEXTURETYPE_CUBE,
    TEXTURETYPE_CUBE_ARRAY = SDL_GPU_TEXTURETYPE_CUBE_ARRAY,
};
REGULAR_ENUM(GPUTextureType);

enum class GPUSampleCount
{
    SAMPLECOUNT_1 = SDL_GPU_SAMPLECOUNT_1,
    SAMPLECOUNT_2 = SDL_GPU_SAMPLECOUNT_2,
    SAMPLECOUNT_4 = SDL_GPU_SAMPLECOUNT_4,
    SAMPLECOUNT_8 = SDL_GPU_SAMPLECOUNT_8,
};
REGULAR_ENUM(GPUSampleCount);

enum class GPUCubeMapFace
{
    CUBEMAPFACE_POSITIVEX = SDL_GPU_CUBEMAPFACE_POSITIVEX,
    CUBEMAPFACE_NEGATIVEX = SDL_GPU_CUBEMAPFACE_NEGATIVEX,
    CUBEMAPFACE_POSITIVEY = SDL_GPU_CUBEMAPFACE_POSITIVEY,
    CUBEMAPFACE_NEGATIVEY = SDL_GPU_CUBEMAPFACE_NEGATIVEY,
    CUBEMAPFACE_POSITIVEZ = SDL_GPU_CUBEMAPFACE_POSITIVEZ,
    CUBEMAPFACE_NEGATIVEZ = SDL_GPU_CUBEMAPFACE_NEGATIVEZ,
};
REGULAR_ENUM(GPUCubeMapFace);

enum class GPUBufferUsageFlags : Uint32
{
    BUFFERUSAGE_VERTEX   = SDL_GPU_BUFFERUSAGE_VERTEX,
    BUFFERUSAGE_INDEX    = SDL_GPU_BUFFERUSAGE_INDEX,
    BUFFERUSAGE_INDIRECT = SDL_GPU_BUFFERUSAGE_INDIRECT,
    BUFFERUSAGE_GRAPHICS_STORAGE_READ =
        SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    BUFFERUSAGE_COMPUTE_STORAGE_READ = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ,
    BUFFERUSAGE_COMPUTE_STORAGE_WRITE =
        SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE,
};
BITFLAG_ENUM(GPUBufferUsageFlags);

enum class GPUTransferBufferUsage
{
    TRANSFERBUFFERUSAGE_UPLOAD   = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    TRANSFERBUFFERUSAGE_DOWNLOAD = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
};
REGULAR_ENUM(GPUTransferBufferUsage);

enum class GPUShaderStage
{
    SHADERSTAGE_VERTEX   = SDL_GPU_SHADERSTAGE_VERTEX,
    SHADERSTAGE_FRAGMENT = SDL_GPU_SHADERSTAGE_FRAGMENT,
};
REGULAR_ENUM(GPUShaderStage);

enum class GPUShaderFormat : Uint32
{
    SHADERFORMAT_INVALID  = SDL_GPU_SHADERFORMAT_INVALID,
    SHADERFORMAT_PRIVATE  = SDL_GPU_SHADERFORMAT_PRIVATE,
    SHADERFORMAT_SPIRV    = SDL_GPU_SHADERFORMAT_SPIRV,
    SHADERFORMAT_DXBC     = SDL_GPU_SHADERFORMAT_DXBC,
    SHADERFORMAT_DXIL     = SDL_GPU_SHADERFORMAT_DXIL,
    SHADERFORMAT_MSL      = SDL_GPU_SHADERFORMAT_MSL,
    SHADERFORMAT_METALLIB = SDL_GPU_SHADERFORMAT_METALLIB,
};
BITFLAG_ENUM(GPUShaderFormat);

enum class GPUVertexElementFormat
{
    VERTEXELEMENTFORMAT_INVALID      = SDL_GPU_VERTEXELEMENTFORMAT_INVALID,
    VERTEXELEMENTFORMAT_INT          = SDL_GPU_VERTEXELEMENTFORMAT_INT,
    VERTEXELEMENTFORMAT_INT2         = SDL_GPU_VERTEXELEMENTFORMAT_INT2,
    VERTEXELEMENTFORMAT_INT3         = SDL_GPU_VERTEXELEMENTFORMAT_INT3,
    VERTEXELEMENTFORMAT_INT4         = SDL_GPU_VERTEXELEMENTFORMAT_INT4,
    VERTEXELEMENTFORMAT_UINT         = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
    VERTEXELEMENTFORMAT_UINT2        = SDL_GPU_VERTEXELEMENTFORMAT_UINT2,
    VERTEXELEMENTFORMAT_UINT3        = SDL_GPU_VERTEXELEMENTFORMAT_UINT3,
    VERTEXELEMENTFORMAT_UINT4        = SDL_GPU_VERTEXELEMENTFORMAT_UINT4,
    VERTEXELEMENTFORMAT_FLOAT        = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
    VERTEXELEMENTFORMAT_FLOAT2       = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
    VERTEXELEMENTFORMAT_FLOAT3       = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
    VERTEXELEMENTFORMAT_FLOAT4       = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
    VERTEXELEMENTFORMAT_BYTE2        = SDL_GPU_VERTEXELEMENTFORMAT_BYTE2,
    VERTEXELEMENTFORMAT_BYTE4        = SDL_GPU_VERTEXELEMENTFORMAT_BYTE4,
    VERTEXELEMENTFORMAT_UBYTE2       = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2,
    VERTEXELEMENTFORMAT_UBYTE4       = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4,
    VERTEXELEMENTFORMAT_BYTE2_NORM   = SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM,
    VERTEXELEMENTFORMAT_BYTE4_NORM   = SDL_GPU_VERTEXELEMENTFORMAT_BYTE4_NORM,
    VERTEXELEMENTFORMAT_UBYTE2_NORM  = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM,
    VERTEXELEMENTFORMAT_UBYTE4_NORM  = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
    VERTEXELEMENTFORMAT_SHORT2       = SDL_GPU_VERTEXELEMENTFORMAT_SHORT2,
    VERTEXELEMENTFORMAT_SHORT4       = SDL_GPU_VERTEXELEMENTFORMAT_SHORT4,
    VERTEXELEMENTFORMAT_USHORT2      = SDL_GPU_VERTEXELEMENTFORMAT_USHORT2,
    VERTEXELEMENTFORMAT_USHORT4      = SDL_GPU_VERTEXELEMENTFORMAT_USHORT4,
    VERTEXELEMENTFORMAT_SHORT2_NORM  = SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM,
    VERTEXELEMENTFORMAT_SHORT4_NORM  = SDL_GPU_VERTEXELEMENTFORMAT_SHORT4_NORM,
    VERTEXELEMENTFORMAT_USHORT2_NORM = SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM,
    VERTEXELEMENTFORMAT_USHORT4_NORM = SDL_GPU_VERTEXELEMENTFORMAT_USHORT4_NORM,
    VERTEXELEMENTFORMAT_HALF2        = SDL_GPU_VERTEXELEMENTFORMAT_HALF2,
    VERTEXELEMENTFORMAT_HALF4        = SDL_GPU_VERTEXELEMENTFORMAT_HALF4,
};
REGULAR_ENUM(GPUVertexElementFormat);

enum class GPUVertexInputRate
{
    VERTEXINPUTRATE_VERTEX   = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    VERTEXINPUTRATE_INSTANCE = SDL_GPU_VERTEXINPUTRATE_INSTANCE,
};
REGULAR_ENUM(GPUVertexInputRate);

enum class GPUFillMode
{
    FILLMODE_FILL = SDL_GPU_FILLMODE_FILL,
    FILLMODE_LINE = SDL_GPU_FILLMODE_LINE,
};
REGULAR_ENUM(GPUFillMode);

enum class GPUCullMode
{
    CULLMODE_NONE  = SDL_GPU_CULLMODE_NONE,
    CULLMODE_FRONT = SDL_GPU_CULLMODE_FRONT,
    CULLMODE_BACK  = SDL_GPU_CULLMODE_BACK,
};
REGULAR_ENUM(GPUCullMode);

enum class GPUFrontFace
{
    FRONTFACE_COUNTER_CLOCKWISE = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
    FRONTFACE_CLOCKWISE         = SDL_GPU_FRONTFACE_CLOCKWISE,
};
REGULAR_ENUM(GPUFrontFace);

enum class GPUCompareOp
{
    COMPAREOP_INVALID          = SDL_GPU_COMPAREOP_INVALID,
    COMPAREOP_NEVER            = SDL_GPU_COMPAREOP_NEVER,
    COMPAREOP_LESS             = SDL_GPU_COMPAREOP_LESS,
    COMPAREOP_EQUAL            = SDL_GPU_COMPAREOP_EQUAL,
    COMPAREOP_LESS_OR_EQUAL    = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
    COMPAREOP_GREATER          = SDL_GPU_COMPAREOP_GREATER,
    COMPAREOP_NOT_EQUAL        = SDL_GPU_COMPAREOP_NOT_EQUAL,
    COMPAREOP_GREATER_OR_EQUAL = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL,
    COMPAREOP_ALWAYS           = SDL_GPU_COMPAREOP_ALWAYS,
};
REGULAR_ENUM(GPUCompareOp);

enum class GPUStencilOp
{
    STENCILOP_INVALID             = SDL_GPU_STENCILOP_INVALID,
    STENCILOP_KEEP                = SDL_GPU_STENCILOP_KEEP,
    STENCILOP_ZERO                = SDL_GPU_STENCILOP_ZERO,
    STENCILOP_REPLACE             = SDL_GPU_STENCILOP_REPLACE,
    STENCILOP_INCREMENT_AND_CLAMP = SDL_GPU_STENCILOP_INCREMENT_AND_CLAMP,
    STENCILOP_DECREMENT_AND_CLAMP = SDL_GPU_STENCILOP_DECREMENT_AND_CLAMP,
    STENCILOP_INVERT              = SDL_GPU_STENCILOP_INVERT,
    STENCILOP_INCREMENT_AND_WRAP  = SDL_GPU_STENCILOP_INCREMENT_AND_WRAP,
    STENCILOP_DECREMENT_AND_WRAP  = SDL_GPU_STENCILOP_DECREMENT_AND_WRAP,
};
REGULAR_ENUM(GPUStencilOp);

enum class GPUBlendOp
{
    BLENDOP_INVALID          = SDL_GPU_BLENDOP_INVALID,
    BLENDOP_ADD              = SDL_GPU_BLENDOP_ADD,
    BLENDOP_SUBTRACT         = SDL_GPU_BLENDOP_SUBTRACT,
    BLENDOP_REVERSE_SUBTRACT = SDL_GPU_BLENDOP_REVERSE_SUBTRACT,
    BLENDOP_MIN              = SDL_GPU_BLENDOP_MIN,
    BLENDOP_MAX              = SDL_GPU_BLENDOP_MAX,
};
REGULAR_ENUM(GPUBlendOp);

enum class GPUBlendFactor
{
    BLENDFACTOR_INVALID             = SDL_GPU_BLENDFACTOR_INVALID,
    BLENDFACTOR_ZERO                = SDL_GPU_BLENDFACTOR_ZERO,
    BLENDFACTOR_ONE                 = SDL_GPU_BLENDFACTOR_ONE,
    BLENDFACTOR_SRC_COLOR           = SDL_GPU_BLENDFACTOR_SRC_COLOR,
    BLENDFACTOR_ONE_MINUS_SRC_COLOR = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
    BLENDFACTOR_DST_COLOR           = SDL_GPU_BLENDFACTOR_DST_COLOR,
    BLENDFACTOR_ONE_MINUS_DST_COLOR = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    BLENDFACTOR_SRC_ALPHA           = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
    BLENDFACTOR_ONE_MINUS_SRC_ALPHA = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    BLENDFACTOR_DST_ALPHA           = SDL_GPU_BLENDFACTOR_DST_ALPHA,
    BLENDFACTOR_ONE_MINUS_DST_ALPHA = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
    BLENDFACTOR_CONSTANT_COLOR      = SDL_GPU_BLENDFACTOR_CONSTANT_COLOR,
    BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR =
        SDL_GPU_BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR,
    BLENDFACTOR_SRC_ALPHA_SATURATE = SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE,
};
REGULAR_ENUM(GPUBlendFactor);

enum class GPUColorComponentFlags : Uint8
{
    COLORCOMPONENT_R = SDL_GPU_COLORCOMPONENT_R,
    COLORCOMPONENT_G = SDL_GPU_COLORCOMPONENT_G,
    COLORCOMPONENT_B = SDL_GPU_COLORCOMPONENT_B,
    COLORCOMPONENT_A = SDL_GPU_COLORCOMPONENT_A,
};
BITFLAG_ENUM(GPUColorComponentFlags);

enum class GPUFilter
{
    FILTER_NEAREST = SDL_GPU_FILTER_NEAREST,
    FILTER_LINEAR  = SDL_GPU_FILTER_LINEAR,
};
REGULAR_ENUM(GPUFilter);

enum class GPUSamplerMipmapMode
{
    SAMPLERMIPMAPMODE_NEAREST = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
    SAMPLERMIPMAPMODE_LINEAR  = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
};
REGULAR_ENUM(GPUSamplerMipmapMode);

enum class GPUSamplerAddressMode
{
    SAMPLERADDRESSMODE_REPEAT = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    SAMPLERADDRESSMODE_MIRRORED_REPEAT =
        SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT,
    SAMPLERADDRESSMODE_CLAMP_TO_EDGE = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
};
REGULAR_ENUM(GPUSamplerAddressMode);

enum class GPUPresentMode
{
    PRESENTMODE_VSYNC     = SDL_GPU_PRESENTMODE_VSYNC,
    PRESENTMODE_IMMEDIATE = SDL_GPU_PRESENTMODE_IMMEDIATE,
    PRESENTMODE_MAILBOX   = SDL_GPU_PRESENTMODE_MAILBOX,
};
REGULAR_ENUM(GPUPresentMode);

enum class GPUSwapchainComposition
{
    SWAPCHAINCOMPOSITION_SDR        = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
    SWAPCHAINCOMPOSITION_SDR_LINEAR = SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR,
    SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR =
        SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR,
    SWAPCHAINCOMPOSITION_HDR10_ST2084 =
        SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084,
};
REGULAR_ENUM(GPUSwapchainComposition);

using GPUViewport = SDL_GPUViewport;

using GPUTextureTransferInfo = SDL_GPUTextureTransferInfo;

using GPUTransferBufferLocation = SDL_GPUTransferBufferLocation;

using GPUTextureLocation = SDL_GPUTextureLocation;

using GPUTextureRegion = SDL_GPUTextureRegion;

using GPUBlitRegion = SDL_GPUBlitRegion;

using GPUBufferLocation = SDL_GPUBufferLocation;

using GPUBufferRegion = SDL_GPUBufferRegion;

using GPUIndirectDrawCommand = SDL_GPUIndirectDrawCommand;

using GPUIndexedIndirectDrawCommand = SDL_GPUIndexedIndirectDrawCommand;

using GPUIndirectDispatchCommand = SDL_GPUIndirectDispatchCommand;

using GPUSamplerCreateInfo = SDL_GPUSamplerCreateInfo;

using GPUVertexBufferDescription = SDL_GPUVertexBufferDescription;

using GPUVertexAttribute = SDL_GPUVertexAttribute;

using GPUVertexInputState = SDL_GPUVertexInputState;

using GPUStencilOpState = SDL_GPUStencilOpState;

using GPUColorTargetBlendState = SDL_GPUColorTargetBlendState;

using GPUShaderCreateInfo = SDL_GPUShaderCreateInfo;

using GPUTextureCreateInfo = SDL_GPUTextureCreateInfo;

using GPUBufferCreateInfo = SDL_GPUBufferCreateInfo;

using GPUTransferBufferCreateInfo = SDL_GPUTransferBufferCreateInfo;

using GPURasterizerState = SDL_GPURasterizerState;

using GPUMultisampleState = SDL_GPUMultisampleState;

using GPUDepthStencilState = SDL_GPUDepthStencilState;

using GPUColorTargetDescription = SDL_GPUColorTargetDescription;

using GPUGraphicsPipelineTargetInfo = SDL_GPUGraphicsPipelineTargetInfo;

using GPUGraphicsPipelineCreateInfo = SDL_GPUGraphicsPipelineCreateInfo;

using GPUComputePipelineCreateInfo = SDL_GPUComputePipelineCreateInfo;

using GPUColorTargetInfo = SDL_GPUColorTargetInfo;

using GPUDepthStencilTargetInfo = SDL_GPUDepthStencilTargetInfo;

using GPUBlitInfo = SDL_GPUBlitInfo;

using GPUBufferBinding = SDL_GPUBufferBinding;

using GPUTextureSamplerBinding = SDL_GPUTextureSamplerBinding;

using GPUStorageBufferReadWriteBinding = SDL_GPUStorageBufferReadWriteBinding;

using GPUStorageTextureReadWriteBinding = SDL_GPUStorageTextureReadWriteBinding;

bool GPUSupportsShaderFormats(GPUShaderFormat format_flags, const char* name)
{
    return SDL_GPUSupportsShaderFormats((SDL_GPUShaderFormat)(format_flags),
                                        name);
}

bool GPUSupportsProperties(SDL_PropertiesID props)
{
    return SDL_GPUSupportsProperties(props);
}

SDL_GPUDevice* CreateGPUDevice(GPUShaderFormat format_flags,
                               bool            debug_mode,
                               const char*     name)
{
    return SDL_CreateGPUDevice(
        (SDL_GPUShaderFormat)(format_flags), debug_mode, name);
}

SDL_GPUDevice* CreateGPUDeviceWithProperties(SDL_PropertiesID props)
{
    return SDL_CreateGPUDeviceWithProperties(props);
}

constexpr auto PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN()
{
    return "SDL.gpu.device.create.debugmode";
}

constexpr auto PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN()
{
    return "SDL.gpu.device.create.preferlowpower";
}

constexpr auto PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN()
{
    return "SDL.gpu.device.create.verbose";
}

constexpr auto PROP_GPU_DEVICE_CREATE_NAME_STRING()
{
    return "SDL.gpu.device.create.name";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.private";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.spirv";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.dxbc";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.dxil";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.msl";
}

constexpr auto PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN()
{
    return "SDL.gpu.device.create.shaders.metallib";
}

constexpr auto PROP_GPU_DEVICE_CREATE_D3D12_SEMANTIC_NAME_STRING()
{
    return "SDL.gpu.device.create.d3d12.semantic";
}

constexpr auto PROP_GPU_DEVICE_CREATE_VULKAN_SHADERCLIPDISTANCE_BOOLEAN()
{
    return "SDL.gpu.device.create.vulkan.shaderclipdistance";
}

constexpr auto PROP_GPU_DEVICE_CREATE_VULKAN_DEPTHCLAMP_BOOLEAN()
{
    return "SDL.gpu.device.create.vulkan.depthclamp";
}

constexpr auto PROP_GPU_DEVICE_CREATE_VULKAN_DRAWINDIRECTFIRST_BOOLEAN()
{
    return "SDL.gpu.device.create.vulkan.drawindirectfirstinstance";
}

constexpr auto PROP_GPU_DEVICE_CREATE_VULKAN_SAMPLERANISOTROPY_BOOLEAN()
{
    return "SDL.gpu.device.create.vulkan.sampleranisotropy";
}

void DestroyGPUDevice(SDL_GPUDevice* device)
{
    SDL_DestroyGPUDevice(device);
}

int GetNumGPUDrivers(void)
{
    return SDL_GetNumGPUDrivers();
}

const char* GetGPUDriver(int index)
{
    return SDL_GetGPUDriver(index);
}

const char* GetGPUDeviceDriver(SDL_GPUDevice* device)
{
    return SDL_GetGPUDeviceDriver(device);
}

SDL_GPUShaderFormat GetGPUShaderFormats(SDL_GPUDevice* device)
{
    return SDL_GetGPUShaderFormats(device);
}

SDL_PropertiesID GetGPUDeviceProperties(SDL_GPUDevice* device)
{
    return SDL_GetGPUDeviceProperties(device);
}

constexpr auto PROP_GPU_DEVICE_NAME_STRING()
{
    return "SDL.gpu.device.name";
}

constexpr auto PROP_GPU_DEVICE_DRIVER_NAME_STRING()
{
    return "SDL.gpu.device.driver_name";
}

constexpr auto PROP_GPU_DEVICE_DRIVER_VERSION_STRING()
{
    return "SDL.gpu.device.driver_version";
}

constexpr auto PROP_GPU_DEVICE_DRIVER_INFO_STRING()
{
    return "SDL.gpu.device.driver_info";
}

SDL_GPUComputePipeline* CreateGPUComputePipeline(
    SDL_GPUDevice* device, const SDL_GPUComputePipelineCreateInfo* createinfo)
{
    return SDL_CreateGPUComputePipeline(device, createinfo);
}

constexpr auto PROP_GPU_COMPUTEPIPELINE_CREATE_NAME_STRING()
{
    return "SDL.gpu.computepipeline.create.name";
}

SDL_GPUGraphicsPipeline* CreateGPUGraphicsPipeline(
    SDL_GPUDevice* device, const SDL_GPUGraphicsPipelineCreateInfo* createinfo)
{
    return SDL_CreateGPUGraphicsPipeline(device, createinfo);
}

constexpr auto PROP_GPU_GRAPHICSPIPELINE_CREATE_NAME_STRING()
{
    return "SDL.gpu.graphicspipeline.create.name";
}

SDL_GPUSampler* CreateGPUSampler(SDL_GPUDevice*                  device,
                                 const SDL_GPUSamplerCreateInfo* createinfo)
{
    return SDL_CreateGPUSampler(device, createinfo);
}

constexpr auto PROP_GPU_SAMPLER_CREATE_NAME_STRING()
{
    return "SDL.gpu.sampler.create.name";
}

SDL_GPUShader* CreateGPUShader(SDL_GPUDevice*                 device,
                               const SDL_GPUShaderCreateInfo* createinfo)
{
    return SDL_CreateGPUShader(device, createinfo);
}

constexpr auto PROP_GPU_SHADER_CREATE_NAME_STRING()
{
    return "SDL.gpu.shader.create.name";
}

SDL_GPUTexture* CreateGPUTexture(SDL_GPUDevice*                  device,
                                 const SDL_GPUTextureCreateInfo* createinfo)
{
    return SDL_CreateGPUTexture(device, createinfo);
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT()
{
    return "SDL.gpu.texture.create.d3d12.clear.r";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT()
{
    return "SDL.gpu.texture.create.d3d12.clear.g";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT()
{
    return "SDL.gpu.texture.create.d3d12.clear.b";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT()
{
    return "SDL.gpu.texture.create.d3d12.clear.a";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_DEPTH_FLOAT()
{
    return "SDL.gpu.texture.create.d3d12.clear.depth";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_STENCIL_NUMBER()
{
    return "SDL.gpu.texture.create.d3d12.clear.stencil";
}

constexpr auto PROP_GPU_TEXTURE_CREATE_NAME_STRING()
{
    return "SDL.gpu.texture.create.name";
}

SDL_GPUBuffer* CreateGPUBuffer(SDL_GPUDevice*                 device,
                               const SDL_GPUBufferCreateInfo* createinfo)
{
    return SDL_CreateGPUBuffer(device, createinfo);
}

constexpr auto PROP_GPU_BUFFER_CREATE_NAME_STRING()
{
    return "SDL.gpu.buffer.create.name";
}

SDL_GPUTransferBuffer* CreateGPUTransferBuffer(
    SDL_GPUDevice* device, const SDL_GPUTransferBufferCreateInfo* createinfo)
{
    return SDL_CreateGPUTransferBuffer(device, createinfo);
}

constexpr auto PROP_GPU_TRANSFERBUFFER_CREATE_NAME_STRING()
{
    return "SDL.gpu.transferbuffer.create.name";
}

void SetGPUBufferName(SDL_GPUDevice* device,
                      SDL_GPUBuffer* buffer,
                      const char*    text)
{
    SDL_SetGPUBufferName(device, buffer, text);
}

void SetGPUTextureName(SDL_GPUDevice*  device,
                       SDL_GPUTexture* texture,
                       const char*     text)
{
    SDL_SetGPUTextureName(device, texture, text);
}

void InsertGPUDebugLabel(SDL_GPUCommandBuffer* command_buffer, const char* text)
{
    SDL_InsertGPUDebugLabel(command_buffer, text);
}

void PushGPUDebugGroup(SDL_GPUCommandBuffer* command_buffer, const char* name)
{
    SDL_PushGPUDebugGroup(command_buffer, name);
}

void PopGPUDebugGroup(SDL_GPUCommandBuffer* command_buffer)
{
    SDL_PopGPUDebugGroup(command_buffer);
}

void ReleaseGPUTexture(SDL_GPUDevice* device, SDL_GPUTexture* texture)
{
    SDL_ReleaseGPUTexture(device, texture);
}

void ReleaseGPUSampler(SDL_GPUDevice* device, SDL_GPUSampler* sampler)
{
    SDL_ReleaseGPUSampler(device, sampler);
}

void ReleaseGPUBuffer(SDL_GPUDevice* device, SDL_GPUBuffer* buffer)
{
    SDL_ReleaseGPUBuffer(device, buffer);
}

void ReleaseGPUTransferBuffer(SDL_GPUDevice*         device,
                              SDL_GPUTransferBuffer* transfer_buffer)
{
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}

void ReleaseGPUComputePipeline(SDL_GPUDevice*          device,
                               SDL_GPUComputePipeline* compute_pipeline)
{
    SDL_ReleaseGPUComputePipeline(device, compute_pipeline);
}

void ReleaseGPUShader(SDL_GPUDevice* device, SDL_GPUShader* shader)
{
    SDL_ReleaseGPUShader(device, shader);
}

void ReleaseGPUGraphicsPipeline(SDL_GPUDevice*           device,
                                SDL_GPUGraphicsPipeline* graphics_pipeline)
{
    SDL_ReleaseGPUGraphicsPipeline(device, graphics_pipeline);
}

SDL_GPUCommandBuffer* AcquireGPUCommandBuffer(SDL_GPUDevice* device)
{
    return SDL_AcquireGPUCommandBuffer(device);
}

void PushGPUVertexUniformData(SDL_GPUCommandBuffer* command_buffer,
                              Uint32                slot_index,
                              const void*           data,
                              Uint32                length)
{
    SDL_PushGPUVertexUniformData(command_buffer, slot_index, data, length);
}

void PushGPUFragmentUniformData(SDL_GPUCommandBuffer* command_buffer,
                                Uint32                slot_index,
                                const void*           data,
                                Uint32                length)
{
    SDL_PushGPUFragmentUniformData(command_buffer, slot_index, data, length);
}

void PushGPUComputeUniformData(SDL_GPUCommandBuffer* command_buffer,
                               Uint32                slot_index,
                               const void*           data,
                               Uint32                length)
{
    SDL_PushGPUComputeUniformData(command_buffer, slot_index, data, length);
}

SDL_GPURenderPass* BeginGPURenderPass(
    SDL_GPUCommandBuffer*                command_buffer,
    const SDL_GPUColorTargetInfo*        color_target_infos,
    Uint32                               num_color_targets,
    const SDL_GPUDepthStencilTargetInfo* depth_stencil_target_info)
{
    return SDL_BeginGPURenderPass(command_buffer,
                                  color_target_infos,
                                  num_color_targets,
                                  depth_stencil_target_info);
}

void BindGPUGraphicsPipeline(SDL_GPURenderPass*       render_pass,
                             SDL_GPUGraphicsPipeline* graphics_pipeline)
{
    SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline);
}

void SetGPUViewport(SDL_GPURenderPass*     render_pass,
                    const SDL_GPUViewport* viewport)
{
    SDL_SetGPUViewport(render_pass, viewport);
}

void SetGPUScissor(SDL_GPURenderPass* render_pass, const SDL_Rect* scissor)
{
    SDL_SetGPUScissor(render_pass, scissor);
}

void SetGPUBlendConstants(SDL_GPURenderPass* render_pass,
                          SDL_FColor         blend_constants)
{
    SDL_SetGPUBlendConstants(render_pass, blend_constants);
}

void SetGPUStencilReference(SDL_GPURenderPass* render_pass, Uint8 reference)
{
    SDL_SetGPUStencilReference(render_pass, reference);
}

void BindGPUVertexBuffers(SDL_GPURenderPass*          render_pass,
                          Uint32                      first_slot,
                          const SDL_GPUBufferBinding* bindings,
                          Uint32                      num_bindings)
{
    SDL_BindGPUVertexBuffers(render_pass, first_slot, bindings, num_bindings);
}

void BindGPUIndexBuffer(SDL_GPURenderPass*          render_pass,
                        const SDL_GPUBufferBinding* binding,
                        GPUIndexElementSize         index_element_size)
{
    SDL_BindGPUIndexBuffer(
        render_pass, binding, (SDL_GPUIndexElementSize)(index_element_size));
}

void BindGPUVertexSamplers(
    SDL_GPURenderPass*                  render_pass,
    Uint32                              first_slot,
    const SDL_GPUTextureSamplerBinding* texture_sampler_bindings,
    Uint32                              num_bindings)
{
    SDL_BindGPUVertexSamplers(
        render_pass, first_slot, texture_sampler_bindings, num_bindings);
}

void BindGPUVertexStorageTextures(SDL_GPURenderPass* render_pass,
                                  Uint32             first_slot,
                                  SDL_GPUTexture**   storage_textures,
                                  Uint32             num_bindings)
{
    SDL_BindGPUVertexStorageTextures(
        render_pass, first_slot, storage_textures, num_bindings);
}

void BindGPUVertexStorageBuffers(SDL_GPURenderPass* render_pass,
                                 Uint32             first_slot,
                                 SDL_GPUBuffer**    storage_buffers,
                                 Uint32             num_bindings)
{
    SDL_BindGPUVertexStorageBuffers(
        render_pass, first_slot, storage_buffers, num_bindings);
}

void BindGPUFragmentSamplers(
    SDL_GPURenderPass*                  render_pass,
    Uint32                              first_slot,
    const SDL_GPUTextureSamplerBinding* texture_sampler_bindings,
    Uint32                              num_bindings)
{
    SDL_BindGPUFragmentSamplers(
        render_pass, first_slot, texture_sampler_bindings, num_bindings);
}

void BindGPUFragmentStorageTextures(SDL_GPURenderPass* render_pass,
                                    Uint32             first_slot,
                                    SDL_GPUTexture**   storage_textures,
                                    Uint32             num_bindings)
{
    SDL_BindGPUFragmentStorageTextures(
        render_pass, first_slot, storage_textures, num_bindings);
}

void BindGPUFragmentStorageBuffers(SDL_GPURenderPass* render_pass,
                                   Uint32             first_slot,
                                   SDL_GPUBuffer**    storage_buffers,
                                   Uint32             num_bindings)
{
    SDL_BindGPUFragmentStorageBuffers(
        render_pass, first_slot, storage_buffers, num_bindings);
}

void DrawGPUIndexedPrimitives(SDL_GPURenderPass* render_pass,
                              Uint32             num_indices,
                              Uint32             num_instances,
                              Uint32             first_index,
                              Sint32             vertex_offset,
                              Uint32             first_instance)
{
    SDL_DrawGPUIndexedPrimitives(render_pass,
                                 num_indices,
                                 num_instances,
                                 first_index,
                                 vertex_offset,
                                 first_instance);
}

void DrawGPUPrimitives(SDL_GPURenderPass* render_pass,
                       Uint32             num_vertices,
                       Uint32             num_instances,
                       Uint32             first_vertex,
                       Uint32             first_instance)
{
    SDL_DrawGPUPrimitives(
        render_pass, num_vertices, num_instances, first_vertex, first_instance);
}

void DrawGPUPrimitivesIndirect(SDL_GPURenderPass* render_pass,
                               SDL_GPUBuffer*     buffer,
                               Uint32             offset,
                               Uint32             draw_count)
{
    SDL_DrawGPUPrimitivesIndirect(render_pass, buffer, offset, draw_count);
}

void DrawGPUIndexedPrimitivesIndirect(SDL_GPURenderPass* render_pass,
                                      SDL_GPUBuffer*     buffer,
                                      Uint32             offset,
                                      Uint32             draw_count)
{
    SDL_DrawGPUIndexedPrimitivesIndirect(
        render_pass, buffer, offset, draw_count);
}

void EndGPURenderPass(SDL_GPURenderPass* render_pass)
{
    SDL_EndGPURenderPass(render_pass);
}

SDL_GPUComputePass* BeginGPUComputePass(
    SDL_GPUCommandBuffer*                        command_buffer,
    const SDL_GPUStorageTextureReadWriteBinding* storage_texture_bindings,
    Uint32                                       num_storage_texture_bindings,
    const SDL_GPUStorageBufferReadWriteBinding*  storage_buffer_bindings,
    Uint32                                       num_storage_buffer_bindings)
{
    return SDL_BeginGPUComputePass(command_buffer,
                                   storage_texture_bindings,
                                   num_storage_texture_bindings,
                                   storage_buffer_bindings,
                                   num_storage_buffer_bindings);
}

void BindGPUComputePipeline(SDL_GPUComputePass*     compute_pass,
                            SDL_GPUComputePipeline* compute_pipeline)
{
    SDL_BindGPUComputePipeline(compute_pass, compute_pipeline);
}

void BindGPUComputeSamplers(
    SDL_GPUComputePass*                 compute_pass,
    Uint32                              first_slot,
    const SDL_GPUTextureSamplerBinding* texture_sampler_bindings,
    Uint32                              num_bindings)
{
    SDL_BindGPUComputeSamplers(
        compute_pass, first_slot, texture_sampler_bindings, num_bindings);
}

void BindGPUComputeStorageTextures(SDL_GPUComputePass* compute_pass,
                                   Uint32              first_slot,
                                   SDL_GPUTexture**    storage_textures,
                                   Uint32              num_bindings)
{
    SDL_BindGPUComputeStorageTextures(
        compute_pass, first_slot, storage_textures, num_bindings);
}

void BindGPUComputeStorageBuffers(SDL_GPUComputePass* compute_pass,
                                  Uint32              first_slot,
                                  SDL_GPUBuffer**     storage_buffers,
                                  Uint32              num_bindings)
{
    SDL_BindGPUComputeStorageBuffers(
        compute_pass, first_slot, storage_buffers, num_bindings);
}

void DispatchGPUCompute(SDL_GPUComputePass* compute_pass,
                        Uint32              groupcount_x,
                        Uint32              groupcount_y,
                        Uint32              groupcount_z)
{
    SDL_DispatchGPUCompute(
        compute_pass, groupcount_x, groupcount_y, groupcount_z);
}

void DispatchGPUComputeIndirect(SDL_GPUComputePass* compute_pass,
                                SDL_GPUBuffer*      buffer,
                                Uint32              offset)
{
    SDL_DispatchGPUComputeIndirect(compute_pass, buffer, offset);
}

void EndGPUComputePass(SDL_GPUComputePass* compute_pass)
{
    SDL_EndGPUComputePass(compute_pass);
}

void* MapGPUTransferBuffer(SDL_GPUDevice*         device,
                           SDL_GPUTransferBuffer* transfer_buffer,
                           bool                   cycle)
{
    return SDL_MapGPUTransferBuffer(device, transfer_buffer, cycle);
}

void UnmapGPUTransferBuffer(SDL_GPUDevice*         device,
                            SDL_GPUTransferBuffer* transfer_buffer)
{
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
}

SDL_GPUCopyPass* BeginGPUCopyPass(SDL_GPUCommandBuffer* command_buffer)
{
    return SDL_BeginGPUCopyPass(command_buffer);
}

void UploadToGPUTexture(SDL_GPUCopyPass*                  copy_pass,
                        const SDL_GPUTextureTransferInfo* source,
                        const SDL_GPUTextureRegion*       destination,
                        bool                              cycle)
{
    SDL_UploadToGPUTexture(copy_pass, source, destination, cycle);
}

void UploadToGPUBuffer(SDL_GPUCopyPass*                     copy_pass,
                       const SDL_GPUTransferBufferLocation* source,
                       const SDL_GPUBufferRegion*           destination,
                       bool                                 cycle)
{
    SDL_UploadToGPUBuffer(copy_pass, source, destination, cycle);
}

void CopyGPUTextureToTexture(SDL_GPUCopyPass*              copy_pass,
                             const SDL_GPUTextureLocation* source,
                             const SDL_GPUTextureLocation* destination,
                             Uint32                        w,
                             Uint32                        h,
                             Uint32                        d,
                             bool                          cycle)
{
    SDL_CopyGPUTextureToTexture(copy_pass, source, destination, w, h, d, cycle);
}

void CopyGPUBufferToBuffer(SDL_GPUCopyPass*             copy_pass,
                           const SDL_GPUBufferLocation* source,
                           const SDL_GPUBufferLocation* destination,
                           Uint32                       size,
                           bool                         cycle)
{
    SDL_CopyGPUBufferToBuffer(copy_pass, source, destination, size, cycle);
}

void DownloadFromGPUTexture(SDL_GPUCopyPass*                  copy_pass,
                            const SDL_GPUTextureRegion*       source,
                            const SDL_GPUTextureTransferInfo* destination)
{
    SDL_DownloadFromGPUTexture(copy_pass, source, destination);
}

void DownloadFromGPUBuffer(SDL_GPUCopyPass*                     copy_pass,
                           const SDL_GPUBufferRegion*           source,
                           const SDL_GPUTransferBufferLocation* destination)
{
    SDL_DownloadFromGPUBuffer(copy_pass, source, destination);
}

void EndGPUCopyPass(SDL_GPUCopyPass* copy_pass)
{
    SDL_EndGPUCopyPass(copy_pass);
}

void GenerateMipmapsForGPUTexture(SDL_GPUCommandBuffer* command_buffer,
                                  SDL_GPUTexture*       texture)
{
    SDL_GenerateMipmapsForGPUTexture(command_buffer, texture);
}

void BlitGPUTexture(SDL_GPUCommandBuffer*  command_buffer,
                    const SDL_GPUBlitInfo* info)
{
    SDL_BlitGPUTexture(command_buffer, info);
}

bool WindowSupportsGPUSwapchainComposition(
    SDL_GPUDevice*          device,
    SDL_Window*             window,
    GPUSwapchainComposition swapchain_composition)
{
    return SDL_WindowSupportsGPUSwapchainComposition(
        device, window, (SDL_GPUSwapchainComposition)(swapchain_composition));
}

bool WindowSupportsGPUPresentMode(SDL_GPUDevice* device,
                                  SDL_Window*    window,
                                  GPUPresentMode present_mode)
{
    return SDL_WindowSupportsGPUPresentMode(
        device, window, (SDL_GPUPresentMode)(present_mode));
}

bool ClaimWindowForGPUDevice(SDL_GPUDevice* device, SDL_Window* window)
{
    return SDL_ClaimWindowForGPUDevice(device, window);
}

void ReleaseWindowFromGPUDevice(SDL_GPUDevice* device, SDL_Window* window)
{
    SDL_ReleaseWindowFromGPUDevice(device, window);
}

bool SetGPUSwapchainParameters(SDL_GPUDevice*          device,
                               SDL_Window*             window,
                               GPUSwapchainComposition swapchain_composition,
                               GPUPresentMode          present_mode)
{
    return SDL_SetGPUSwapchainParameters(
        device,
        window,
        (SDL_GPUSwapchainComposition)(swapchain_composition),
        (SDL_GPUPresentMode)(present_mode));
}

bool SetGPUAllowedFramesInFlight(SDL_GPUDevice* device,
                                 Uint32         allowed_frames_in_flight)
{
    return SDL_SetGPUAllowedFramesInFlight(device, allowed_frames_in_flight);
}

SDL_GPUTextureFormat GetGPUSwapchainTextureFormat(SDL_GPUDevice* device,
                                                  SDL_Window*    window)
{
    return SDL_GetGPUSwapchainTextureFormat(device, window);
}

bool AcquireGPUSwapchainTexture(SDL_GPUCommandBuffer* command_buffer,
                                SDL_Window*           window,
                                SDL_GPUTexture**      swapchain_texture,
                                Uint32*               swapchain_texture_width,
                                Uint32*               swapchain_texture_height)
{
    return SDL_AcquireGPUSwapchainTexture(command_buffer,
                                          window,
                                          swapchain_texture,
                                          swapchain_texture_width,
                                          swapchain_texture_height);
}

bool WaitForGPUSwapchain(SDL_GPUDevice* device, SDL_Window* window)
{
    return SDL_WaitForGPUSwapchain(device, window);
}

bool WaitAndAcquireGPUSwapchainTexture(SDL_GPUCommandBuffer* command_buffer,
                                       SDL_Window*           window,
                                       SDL_GPUTexture**      swapchain_texture,
                                       Uint32* swapchain_texture_width,
                                       Uint32* swapchain_texture_height)
{
    return SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer,
                                                 window,
                                                 swapchain_texture,
                                                 swapchain_texture_width,
                                                 swapchain_texture_height);
}

bool SubmitGPUCommandBuffer(SDL_GPUCommandBuffer* command_buffer)
{
    return SDL_SubmitGPUCommandBuffer(command_buffer);
}

SDL_GPUFence* SubmitGPUCommandBufferAndAcquireFence(
    SDL_GPUCommandBuffer* command_buffer)
{
    return SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
}

bool CancelGPUCommandBuffer(SDL_GPUCommandBuffer* command_buffer)
{
    return SDL_CancelGPUCommandBuffer(command_buffer);
}

bool WaitForGPUIdle(SDL_GPUDevice* device)
{
    return SDL_WaitForGPUIdle(device);
}

bool WaitForGPUFences(SDL_GPUDevice* device,
                      bool           wait_all,
                      SDL_GPUFence** fences,
                      Uint32         num_fences)
{
    return SDL_WaitForGPUFences(device, wait_all, fences, num_fences);
}

bool QueryGPUFence(SDL_GPUDevice* device, SDL_GPUFence* fence)
{
    return SDL_QueryGPUFence(device, fence);
}

void ReleaseGPUFence(SDL_GPUDevice* device, SDL_GPUFence* fence)
{
    SDL_ReleaseGPUFence(device, fence);
}

Uint32 GPUTextureFormatTexelBlockSize(GPUTextureFormat format)
{
    return SDL_GPUTextureFormatTexelBlockSize((SDL_GPUTextureFormat)(format));
}

bool GPUTextureSupportsFormat(SDL_GPUDevice*       device,
                              GPUTextureFormat     format,
                              GPUTextureType       type,
                              GPUTextureUsageFlags usage)
{
    return SDL_GPUTextureSupportsFormat(device,
                                        (SDL_GPUTextureFormat)(format),
                                        (SDL_GPUTextureType)(type),
                                        (SDL_GPUTextureUsageFlags)(usage));
}

bool GPUTextureSupportsSampleCount(SDL_GPUDevice*   device,
                                   GPUTextureFormat format,
                                   GPUSampleCount   sample_count)
{
    return SDL_GPUTextureSupportsSampleCount(
        device,
        (SDL_GPUTextureFormat)(format),
        (SDL_GPUSampleCount)(sample_count));
}

Uint32 CalculateGPUTextureFormatSize(GPUTextureFormat format,
                                     Uint32           width,
                                     Uint32           height,
                                     Uint32           depth_or_layer_count)
{
    return SDL_CalculateGPUTextureFormatSize(
        (SDL_GPUTextureFormat)(format), width, height, depth_or_layer_count);
}
#if defined(SDL_PLATFORM_GDK)

void GDKSuspendGPU(SDL_GPUDevice* device)
{
    SDL_GDKSuspendGPU(device);
}
#endif

#if defined(SDL_PLATFORM_GDK)

void GDKResumeGPU(SDL_GPUDevice* device)
{
    SDL_GDKResumeGPU(device);
}
#endif

using SDL_Haptic = SDL_Haptic;

constexpr auto HAPTIC_INFINITY()
{
    return 4294967295U;
}

using HapticEffectType = Uint16;

constexpr auto HAPTIC_CONSTANT()
{
    return (1u << 0);
}

constexpr auto HAPTIC_SINE()
{
    return (1u << 1);
}

constexpr auto HAPTIC_SQUARE()
{
    return (1u << 2);
}

constexpr auto HAPTIC_TRIANGLE()
{
    return (1u << 3);
}

constexpr auto HAPTIC_SAWTOOTHUP()
{
    return (1u << 4);
}

constexpr auto HAPTIC_SAWTOOTHDOWN()
{
    return (1u << 5);
}

constexpr auto HAPTIC_RAMP()
{
    return (1u << 6);
}

constexpr auto HAPTIC_SPRING()
{
    return (1u << 7);
}

constexpr auto HAPTIC_DAMPER()
{
    return (1u << 8);
}

constexpr auto HAPTIC_INERTIA()
{
    return (1u << 9);
}

constexpr auto HAPTIC_FRICTION()
{
    return (1u << 10);
}

constexpr auto HAPTIC_LEFTRIGHT()
{
    return (1u << 11);
}

constexpr auto HAPTIC_RESERVED1()
{
    return (1u << 12);
}

constexpr auto HAPTIC_RESERVED2()
{
    return (1u << 13);
}

constexpr auto HAPTIC_RESERVED3()
{
    return (1u << 14);
}

constexpr auto HAPTIC_CUSTOM()
{
    return (1u << 15);
}

constexpr auto HAPTIC_GAIN()
{
    return (1u << 16);
}

constexpr auto HAPTIC_AUTOCENTER()
{
    return (1u << 17);
}

constexpr auto HAPTIC_STATUS()
{
    return (1u << 18);
}

constexpr auto HAPTIC_PAUSE()
{
    return (1u << 19);
}

using HapticDirectionType = Uint8;

constexpr auto HAPTIC_POLAR()
{
    return 0;
}

constexpr auto HAPTIC_CARTESIAN()
{
    return 1;
}

constexpr auto HAPTIC_SPHERICAL()
{
    return 2;
}

constexpr auto HAPTIC_STEERING_AXIS()
{
    return 3;
}

using HapticEffectID = int;

using HapticDirection = SDL_HapticDirection;

using HapticConstant = SDL_HapticConstant;

using HapticPeriodic = SDL_HapticPeriodic;

using HapticCondition = SDL_HapticCondition;

using HapticRamp = SDL_HapticRamp;

using HapticLeftRight = SDL_HapticLeftRight;

using HapticCustom = SDL_HapticCustom;

using HapticEffect = SDL_HapticEffect;

using HapticID = Uint32;

SDL_HapticID* GetHaptics(int* count)
{
    return SDL_GetHaptics(count);
}

const char* GetHapticNameForID(SDL_HapticID instance_id)
{
    return SDL_GetHapticNameForID(instance_id);
}

SDL_Haptic* OpenHaptic(SDL_HapticID instance_id)
{
    return SDL_OpenHaptic(instance_id);
}

SDL_Haptic* GetHapticFromID(SDL_HapticID instance_id)
{
    return SDL_GetHapticFromID(instance_id);
}

SDL_HapticID GetHapticID(SDL_Haptic* haptic)
{
    return SDL_GetHapticID(haptic);
}

const char* GetHapticName(SDL_Haptic* haptic)
{
    return SDL_GetHapticName(haptic);
}

bool IsMouseHaptic(void)
{
    return SDL_IsMouseHaptic();
}

SDL_Haptic* OpenHapticFromMouse(void)
{
    return SDL_OpenHapticFromMouse();
}

bool IsJoystickHaptic(SDL_Joystick* joystick)
{
    return SDL_IsJoystickHaptic(joystick);
}

SDL_Haptic* OpenHapticFromJoystick(SDL_Joystick* joystick)
{
    return SDL_OpenHapticFromJoystick(joystick);
}

void CloseHaptic(SDL_Haptic* haptic)
{
    SDL_CloseHaptic(haptic);
}

int GetMaxHapticEffects(SDL_Haptic* haptic)
{
    return SDL_GetMaxHapticEffects(haptic);
}

int GetMaxHapticEffectsPlaying(SDL_Haptic* haptic)
{
    return SDL_GetMaxHapticEffectsPlaying(haptic);
}

Uint32 GetHapticFeatures(SDL_Haptic* haptic)
{
    return SDL_GetHapticFeatures(haptic);
}

int GetNumHapticAxes(SDL_Haptic* haptic)
{
    return SDL_GetNumHapticAxes(haptic);
}

bool HapticEffectSupported(SDL_Haptic* haptic, const SDL_HapticEffect* effect)
{
    return SDL_HapticEffectSupported(haptic, effect);
}

SDL_HapticEffectID CreateHapticEffect(SDL_Haptic*             haptic,
                                      const SDL_HapticEffect* effect)
{
    return SDL_CreateHapticEffect(haptic, effect);
}

bool UpdateHapticEffect(SDL_Haptic*             haptic,
                        SDL_HapticEffectID      effect,
                        const SDL_HapticEffect* data)
{
    return SDL_UpdateHapticEffect(haptic, effect, data);
}

bool RunHapticEffect(SDL_Haptic*        haptic,
                     SDL_HapticEffectID effect,
                     Uint32             iterations)
{
    return SDL_RunHapticEffect(haptic, effect, iterations);
}

bool StopHapticEffect(SDL_Haptic* haptic, SDL_HapticEffectID effect)
{
    return SDL_StopHapticEffect(haptic, effect);
}

void DestroyHapticEffect(SDL_Haptic* haptic, SDL_HapticEffectID effect)
{
    SDL_DestroyHapticEffect(haptic, effect);
}

bool GetHapticEffectStatus(SDL_Haptic* haptic, SDL_HapticEffectID effect)
{
    return SDL_GetHapticEffectStatus(haptic, effect);
}

bool SetHapticGain(SDL_Haptic* haptic, int gain)
{
    return SDL_SetHapticGain(haptic, gain);
}

bool SetHapticAutocenter(SDL_Haptic* haptic, int autocenter)
{
    return SDL_SetHapticAutocenter(haptic, autocenter);
}

bool PauseHaptic(SDL_Haptic* haptic)
{
    return SDL_PauseHaptic(haptic);
}

bool ResumeHaptic(SDL_Haptic* haptic)
{
    return SDL_ResumeHaptic(haptic);
}

bool StopHapticEffects(SDL_Haptic* haptic)
{
    return SDL_StopHapticEffects(haptic);
}

bool HapticRumbleSupported(SDL_Haptic* haptic)
{
    return SDL_HapticRumbleSupported(haptic);
}

bool InitHapticRumble(SDL_Haptic* haptic)
{
    return SDL_InitHapticRumble(haptic);
}

bool PlayHapticRumble(SDL_Haptic* haptic, float strength, Uint32 length)
{
    return SDL_PlayHapticRumble(haptic, strength, length);
}

bool StopHapticRumble(SDL_Haptic* haptic)
{
    return SDL_StopHapticRumble(haptic);
}

constexpr auto HINT_ALLOW_ALT_TAB_WHILE_GRABBED()
{
    return "SDL_ALLOW_ALT_TAB_WHILE_GRABBED";
}

constexpr auto HINT_ANDROID_ALLOW_RECREATE_ACTIVITY()
{
    return "SDL_ANDROID_ALLOW_RECREATE_ACTIVITY";
}

constexpr auto HINT_ANDROID_BLOCK_ON_PAUSE()
{
    return "SDL_ANDROID_BLOCK_ON_PAUSE";
}

constexpr auto HINT_ANDROID_LOW_LATENCY_AUDIO()
{
    return "SDL_ANDROID_LOW_LATENCY_AUDIO";
}

constexpr auto HINT_ANDROID_TRAP_BACK_BUTTON()
{
    return "SDL_ANDROID_TRAP_BACK_BUTTON";
}

constexpr auto HINT_APP_ID()
{
    return "SDL_APP_ID";
}

constexpr auto HINT_APP_NAME()
{
    return "SDL_APP_NAME";
}

constexpr auto HINT_APPLE_TV_CONTROLLER_UI_EVENTS()
{
    return "SDL_APPLE_TV_CONTROLLER_UI_EVENTS";
}

constexpr auto HINT_APPLE_TV_REMOTE_ALLOW_ROTATION()
{
    return "SDL_APPLE_TV_REMOTE_ALLOW_ROTATION";
}

constexpr auto HINT_AUDIO_ALSA_DEFAULT_DEVICE()
{
    return "SDL_AUDIO_ALSA_DEFAULT_DEVICE";
}

constexpr auto HINT_AUDIO_ALSA_DEFAULT_PLAYBACK_DEVICE()
{
    return "SDL_AUDIO_ALSA_DEFAULT_PLAYBACK_DEVICE";
}

constexpr auto HINT_AUDIO_ALSA_DEFAULT_RECORDING_DEVICE()
{
    return "SDL_AUDIO_ALSA_DEFAULT_RECORDING_DEVICE";
}

constexpr auto HINT_AUDIO_CATEGORY()
{
    return "SDL_AUDIO_CATEGORY";
}

constexpr auto HINT_AUDIO_CHANNELS()
{
    return "SDL_AUDIO_CHANNELS";
}

constexpr auto HINT_AUDIO_DEVICE_APP_ICON_NAME()
{
    return "SDL_AUDIO_DEVICE_APP_ICON_NAME";
}

constexpr auto HINT_AUDIO_DEVICE_SAMPLE_FRAMES()
{
    return "SDL_AUDIO_DEVICE_SAMPLE_FRAMES";
}

constexpr auto HINT_AUDIO_DEVICE_STREAM_NAME()
{
    return "SDL_AUDIO_DEVICE_STREAM_NAME";
}

constexpr auto HINT_AUDIO_DEVICE_STREAM_ROLE()
{
    return "SDL_AUDIO_DEVICE_STREAM_ROLE";
}

constexpr auto HINT_AUDIO_DISK_INPUT_FILE()
{
    return "SDL_AUDIO_DISK_INPUT_FILE";
}

constexpr auto HINT_AUDIO_DISK_OUTPUT_FILE()
{
    return "SDL_AUDIO_DISK_OUTPUT_FILE";
}

constexpr auto HINT_AUDIO_DISK_TIMESCALE()
{
    return "SDL_AUDIO_DISK_TIMESCALE";
}

constexpr auto HINT_AUDIO_DRIVER()
{
    return "SDL_AUDIO_DRIVER";
}

constexpr auto HINT_AUDIO_DUMMY_TIMESCALE()
{
    return "SDL_AUDIO_DUMMY_TIMESCALE";
}

constexpr auto HINT_AUDIO_FORMAT()
{
    return "SDL_AUDIO_FORMAT";
}

constexpr auto HINT_AUDIO_FREQUENCY()
{
    return "SDL_AUDIO_FREQUENCY";
}

constexpr auto HINT_AUDIO_INCLUDE_MONITORS()
{
    return "SDL_AUDIO_INCLUDE_MONITORS";
}

constexpr auto HINT_AUTO_UPDATE_JOYSTICKS()
{
    return "SDL_AUTO_UPDATE_JOYSTICKS";
}

constexpr auto HINT_AUTO_UPDATE_SENSORS()
{
    return "SDL_AUTO_UPDATE_SENSORS";
}

constexpr auto HINT_BMP_SAVE_LEGACY_FORMAT()
{
    return "SDL_BMP_SAVE_LEGACY_FORMAT";
}

constexpr auto HINT_CAMERA_DRIVER()
{
    return "SDL_CAMERA_DRIVER";
}

constexpr auto HINT_CPU_FEATURE_MASK()
{
    return "SDL_CPU_FEATURE_MASK";
}

constexpr auto HINT_JOYSTICK_DIRECTINPUT()
{
    return "SDL_JOYSTICK_DIRECTINPUT";
}

constexpr auto HINT_FILE_DIALOG_DRIVER()
{
    return "SDL_FILE_DIALOG_DRIVER";
}

constexpr auto HINT_DISPLAY_USABLE_BOUNDS()
{
    return "SDL_DISPLAY_USABLE_BOUNDS";
}

constexpr auto HINT_EMSCRIPTEN_ASYNCIFY()
{
    return "SDL_EMSCRIPTEN_ASYNCIFY";
}

constexpr auto HINT_EMSCRIPTEN_CANVAS_SELECTOR()
{
    return "SDL_EMSCRIPTEN_CANVAS_SELECTOR";
}

constexpr auto HINT_EMSCRIPTEN_KEYBOARD_ELEMENT()
{
    return "SDL_EMSCRIPTEN_KEYBOARD_ELEMENT";
}

constexpr auto HINT_ENABLE_SCREEN_KEYBOARD()
{
    return "SDL_ENABLE_SCREEN_KEYBOARD";
}

constexpr auto HINT_EVDEV_DEVICES()
{
    return "SDL_EVDEV_DEVICES";
}

constexpr auto HINT_EVENT_LOGGING()
{
    return "SDL_EVENT_LOGGING";
}

constexpr auto HINT_FORCE_RAISEWINDOW()
{
    return "SDL_FORCE_RAISEWINDOW";
}

constexpr auto HINT_FRAMEBUFFER_ACCELERATION()
{
    return "SDL_FRAMEBUFFER_ACCELERATION";
}

constexpr auto HINT_GAMECONTROLLERCONFIG()
{
    return "SDL_GAMECONTROLLERCONFIG";
}

constexpr auto HINT_GAMECONTROLLERCONFIG_FILE()
{
    return "SDL_GAMECONTROLLERCONFIG_FILE";
}

constexpr auto HINT_GAMECONTROLLERTYPE()
{
    return "SDL_GAMECONTROLLERTYPE";
}

constexpr auto HINT_GAMECONTROLLER_IGNORE_DEVICES()
{
    return "SDL_GAMECONTROLLER_IGNORE_DEVICES";
}

constexpr auto HINT_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT()
{
    return "SDL_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT";
}

constexpr auto HINT_GAMECONTROLLER_SENSOR_FUSION()
{
    return "SDL_GAMECONTROLLER_SENSOR_FUSION";
}

constexpr auto HINT_GDK_TEXTINPUT_DEFAULT_TEXT()
{
    return "SDL_GDK_TEXTINPUT_DEFAULT_TEXT";
}

constexpr auto HINT_GDK_TEXTINPUT_DESCRIPTION()
{
    return "SDL_GDK_TEXTINPUT_DESCRIPTION";
}

constexpr auto HINT_GDK_TEXTINPUT_MAX_LENGTH()
{
    return "SDL_GDK_TEXTINPUT_MAX_LENGTH";
}

constexpr auto HINT_GDK_TEXTINPUT_SCOPE()
{
    return "SDL_GDK_TEXTINPUT_SCOPE";
}

constexpr auto HINT_GDK_TEXTINPUT_TITLE()
{
    return "SDL_GDK_TEXTINPUT_TITLE";
}

constexpr auto HINT_HIDAPI_LIBUSB()
{
    return "SDL_HIDAPI_LIBUSB";
}

constexpr auto HINT_HIDAPI_LIBUSB_WHITELIST()
{
    return "SDL_HIDAPI_LIBUSB_WHITELIST";
}

constexpr auto HINT_HIDAPI_UDEV()
{
    return "SDL_HIDAPI_UDEV";
}

constexpr auto HINT_GPU_DRIVER()
{
    return "SDL_GPU_DRIVER";
}

constexpr auto HINT_HIDAPI_ENUMERATE_ONLY_CONTROLLERS()
{
    return "SDL_HIDAPI_ENUMERATE_ONLY_CONTROLLERS";
}

constexpr auto HINT_HIDAPI_IGNORE_DEVICES()
{
    return "SDL_HIDAPI_IGNORE_DEVICES";
}

constexpr auto HINT_IME_IMPLEMENTED_UI()
{
    return "SDL_IME_IMPLEMENTED_UI";
}

constexpr auto HINT_IOS_HIDE_HOME_INDICATOR()
{
    return "SDL_IOS_HIDE_HOME_INDICATOR";
}

constexpr auto HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS()
{
    return "SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS";
}

constexpr auto HINT_JOYSTICK_ARCADESTICK_DEVICES()
{
    return "SDL_JOYSTICK_ARCADESTICK_DEVICES";
}

constexpr auto HINT_JOYSTICK_ARCADESTICK_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_ARCADESTICK_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_BLACKLIST_DEVICES()
{
    return "SDL_JOYSTICK_BLACKLIST_DEVICES";
}

constexpr auto HINT_JOYSTICK_BLACKLIST_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_BLACKLIST_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_DEVICE()
{
    return "SDL_JOYSTICK_DEVICE";
}

constexpr auto HINT_JOYSTICK_ENHANCED_REPORTS()
{
    return "SDL_JOYSTICK_ENHANCED_REPORTS";
}

constexpr auto HINT_JOYSTICK_FLIGHTSTICK_DEVICES()
{
    return "SDL_JOYSTICK_FLIGHTSTICK_DEVICES";
}

constexpr auto HINT_JOYSTICK_FLIGHTSTICK_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_FLIGHTSTICK_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_GAMEINPUT()
{
    return "SDL_JOYSTICK_GAMEINPUT";
}

constexpr auto HINT_JOYSTICK_GAMECUBE_DEVICES()
{
    return "SDL_JOYSTICK_GAMECUBE_DEVICES";
}

constexpr auto HINT_JOYSTICK_GAMECUBE_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_GAMECUBE_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_HIDAPI()
{
    return "SDL_JOYSTICK_HIDAPI";
}

constexpr auto HINT_JOYSTICK_HIDAPI_COMBINE_JOY_CONS()
{
    return "SDL_JOYSTICK_HIDAPI_COMBINE_JOY_CONS";
}

constexpr auto HINT_JOYSTICK_HIDAPI_GAMECUBE()
{
    return "SDL_JOYSTICK_HIDAPI_GAMECUBE";
}

constexpr auto HINT_JOYSTICK_HIDAPI_GAMECUBE_RUMBLE_BRAKE()
{
    return "SDL_JOYSTICK_HIDAPI_GAMECUBE_RUMBLE_BRAKE";
}

constexpr auto HINT_JOYSTICK_HIDAPI_JOY_CONS()
{
    return "SDL_JOYSTICK_HIDAPI_JOY_CONS";
}

constexpr auto HINT_JOYSTICK_HIDAPI_JOYCON_HOME_LED()
{
    return "SDL_JOYSTICK_HIDAPI_JOYCON_HOME_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_LUNA()
{
    return "SDL_JOYSTICK_HIDAPI_LUNA";
}

constexpr auto HINT_JOYSTICK_HIDAPI_NINTENDO_CLASSIC()
{
    return "SDL_JOYSTICK_HIDAPI_NINTENDO_CLASSIC";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS3()
{
    return "SDL_JOYSTICK_HIDAPI_PS3";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS3_SIXAXIS_DRIVER()
{
    return "SDL_JOYSTICK_HIDAPI_PS3_SIXAXIS_DRIVER";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS4()
{
    return "SDL_JOYSTICK_HIDAPI_PS4";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS4_REPORT_INTERVAL()
{
    return "SDL_JOYSTICK_HIDAPI_PS4_REPORT_INTERVAL";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS5()
{
    return "SDL_JOYSTICK_HIDAPI_PS5";
}

constexpr auto HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED()
{
    return "SDL_JOYSTICK_HIDAPI_PS5_PLAYER_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_SHIELD()
{
    return "SDL_JOYSTICK_HIDAPI_SHIELD";
}

constexpr auto HINT_JOYSTICK_HIDAPI_STADIA()
{
    return "SDL_JOYSTICK_HIDAPI_STADIA";
}

constexpr auto HINT_JOYSTICK_HIDAPI_STEAM()
{
    return "SDL_JOYSTICK_HIDAPI_STEAM";
}

constexpr auto HINT_JOYSTICK_HIDAPI_STEAM_HOME_LED()
{
    return "SDL_JOYSTICK_HIDAPI_STEAM_HOME_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_STEAMDECK()
{
    return "SDL_JOYSTICK_HIDAPI_STEAMDECK";
}

constexpr auto HINT_JOYSTICK_HIDAPI_STEAM_HORI()
{
    return "SDL_JOYSTICK_HIDAPI_STEAM_HORI";
}

constexpr auto HINT_JOYSTICK_HIDAPI_LG4FF()
{
    return "SDL_JOYSTICK_HIDAPI_LG4FF";
}

constexpr auto HINT_JOYSTICK_HIDAPI_8BITDO()
{
    return "SDL_JOYSTICK_HIDAPI_8BITDO";
}

constexpr auto HINT_JOYSTICK_HIDAPI_SINPUT()
{
    return "SDL_JOYSTICK_HIDAPI_SINPUT";
}

constexpr auto HINT_JOYSTICK_HIDAPI_FLYDIGI()
{
    return "SDL_JOYSTICK_HIDAPI_FLYDIGI";
}

constexpr auto HINT_JOYSTICK_HIDAPI_SWITCH()
{
    return "SDL_JOYSTICK_HIDAPI_SWITCH";
}

constexpr auto HINT_JOYSTICK_HIDAPI_SWITCH_HOME_LED()
{
    return "SDL_JOYSTICK_HIDAPI_SWITCH_HOME_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_SWITCH_PLAYER_LED()
{
    return "SDL_JOYSTICK_HIDAPI_SWITCH_PLAYER_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_VERTICAL_JOY_CONS()
{
    return "SDL_JOYSTICK_HIDAPI_VERTICAL_JOY_CONS";
}

constexpr auto HINT_JOYSTICK_HIDAPI_WII()
{
    return "SDL_JOYSTICK_HIDAPI_WII";
}

constexpr auto HINT_JOYSTICK_HIDAPI_WII_PLAYER_LED()
{
    return "SDL_JOYSTICK_HIDAPI_WII_PLAYER_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX_360()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX_360";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX_360_PLAYER_LED()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX_360_PLAYER_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX_360_WIRELESS()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX_360_WIRELESS";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX_ONE()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX_ONE";
}

constexpr auto HINT_JOYSTICK_HIDAPI_XBOX_ONE_HOME_LED()
{
    return "SDL_JOYSTICK_HIDAPI_XBOX_ONE_HOME_LED";
}

constexpr auto HINT_JOYSTICK_HIDAPI_GIP()
{
    return "SDL_JOYSTICK_HIDAPI_GIP";
}

constexpr auto HINT_JOYSTICK_HIDAPI_GIP_RESET_FOR_METADATA()
{
    return "SDL_JOYSTICK_HIDAPI_GIP_RESET_FOR_METADATA";
}

constexpr auto HINT_JOYSTICK_IOKIT()
{
    return "SDL_JOYSTICK_IOKIT";
}

constexpr auto HINT_JOYSTICK_LINUX_CLASSIC()
{
    return "SDL_JOYSTICK_LINUX_CLASSIC";
}

constexpr auto HINT_JOYSTICK_LINUX_DEADZONES()
{
    return "SDL_JOYSTICK_LINUX_DEADZONES";
}

constexpr auto HINT_JOYSTICK_LINUX_DIGITAL_HATS()
{
    return "SDL_JOYSTICK_LINUX_DIGITAL_HATS";
}

constexpr auto HINT_JOYSTICK_LINUX_HAT_DEADZONES()
{
    return "SDL_JOYSTICK_LINUX_HAT_DEADZONES";
}

constexpr auto HINT_JOYSTICK_MFI()
{
    return "SDL_JOYSTICK_MFI";
}

constexpr auto HINT_JOYSTICK_RAWINPUT()
{
    return "SDL_JOYSTICK_RAWINPUT";
}

constexpr auto HINT_JOYSTICK_RAWINPUT_CORRELATE_XINPUT()
{
    return "SDL_JOYSTICK_RAWINPUT_CORRELATE_XINPUT";
}

constexpr auto HINT_JOYSTICK_ROG_CHAKRAM()
{
    return "SDL_JOYSTICK_ROG_CHAKRAM";
}

constexpr auto HINT_JOYSTICK_THREAD()
{
    return "SDL_JOYSTICK_THREAD";
}

constexpr auto HINT_JOYSTICK_THROTTLE_DEVICES()
{
    return "SDL_JOYSTICK_THROTTLE_DEVICES";
}

constexpr auto HINT_JOYSTICK_THROTTLE_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_THROTTLE_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_WGI()
{
    return "SDL_JOYSTICK_WGI";
}

constexpr auto HINT_JOYSTICK_WHEEL_DEVICES()
{
    return "SDL_JOYSTICK_WHEEL_DEVICES";
}

constexpr auto HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED()
{
    return "SDL_JOYSTICK_WHEEL_DEVICES_EXCLUDED";
}

constexpr auto HINT_JOYSTICK_ZERO_CENTERED_DEVICES()
{
    return "SDL_JOYSTICK_ZERO_CENTERED_DEVICES";
}

constexpr auto HINT_JOYSTICK_HAPTIC_AXES()
{
    return "SDL_JOYSTICK_HAPTIC_AXES";
}

constexpr auto HINT_KEYCODE_OPTIONS()
{
    return "SDL_KEYCODE_OPTIONS";
}

constexpr auto HINT_KMSDRM_DEVICE_INDEX()
{
    return "SDL_KMSDRM_DEVICE_INDEX";
}

constexpr auto HINT_KMSDRM_REQUIRE_DRM_MASTER()
{
    return "SDL_KMSDRM_REQUIRE_DRM_MASTER";
}

constexpr auto HINT_LOGGING()
{
    return "SDL_LOGGING";
}

constexpr auto HINT_MAC_BACKGROUND_APP()
{
    return "SDL_MAC_BACKGROUND_APP";
}

constexpr auto HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK()
{
    return "SDL_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK";
}

constexpr auto HINT_MAC_OPENGL_ASYNC_DISPATCH()
{
    return "SDL_MAC_OPENGL_ASYNC_DISPATCH";
}

constexpr auto HINT_MAC_OPTION_AS_ALT()
{
    return "SDL_MAC_OPTION_AS_ALT";
}

constexpr auto HINT_MAC_SCROLL_MOMENTUM()
{
    return "SDL_MAC_SCROLL_MOMENTUM";
}

constexpr auto HINT_MAIN_CALLBACK_RATE()
{
    return "SDL_MAIN_CALLBACK_RATE";
}

constexpr auto HINT_MOUSE_AUTO_CAPTURE()
{
    return "SDL_MOUSE_AUTO_CAPTURE";
}

constexpr auto HINT_MOUSE_DOUBLE_CLICK_RADIUS()
{
    return "SDL_MOUSE_DOUBLE_CLICK_RADIUS";
}

constexpr auto HINT_MOUSE_DOUBLE_CLICK_TIME()
{
    return "SDL_MOUSE_DOUBLE_CLICK_TIME";
}

constexpr auto HINT_MOUSE_DEFAULT_SYSTEM_CURSOR()
{
    return "SDL_MOUSE_DEFAULT_SYSTEM_CURSOR";
}

constexpr auto HINT_MOUSE_EMULATE_WARP_WITH_RELATIVE()
{
    return "SDL_MOUSE_EMULATE_WARP_WITH_RELATIVE";
}

constexpr auto HINT_MOUSE_FOCUS_CLICKTHROUGH()
{
    return "SDL_MOUSE_FOCUS_CLICKTHROUGH";
}

constexpr auto HINT_MOUSE_NORMAL_SPEED_SCALE()
{
    return "SDL_MOUSE_NORMAL_SPEED_SCALE";
}

constexpr auto HINT_MOUSE_RELATIVE_MODE_CENTER()
{
    return "SDL_MOUSE_RELATIVE_MODE_CENTER";
}

constexpr auto HINT_MOUSE_RELATIVE_SPEED_SCALE()
{
    return "SDL_MOUSE_RELATIVE_SPEED_SCALE";
}

constexpr auto HINT_MOUSE_RELATIVE_SYSTEM_SCALE()
{
    return "SDL_MOUSE_RELATIVE_SYSTEM_SCALE";
}

constexpr auto HINT_MOUSE_RELATIVE_WARP_MOTION()
{
    return "SDL_MOUSE_RELATIVE_WARP_MOTION";
}

constexpr auto HINT_MOUSE_RELATIVE_CURSOR_VISIBLE()
{
    return "SDL_MOUSE_RELATIVE_CURSOR_VISIBLE";
}

constexpr auto HINT_MOUSE_TOUCH_EVENTS()
{
    return "SDL_MOUSE_TOUCH_EVENTS";
}

constexpr auto HINT_MUTE_CONSOLE_KEYBOARD()
{
    return "SDL_MUTE_CONSOLE_KEYBOARD";
}

constexpr auto HINT_NO_SIGNAL_HANDLERS()
{
    return "SDL_NO_SIGNAL_HANDLERS";
}

constexpr auto HINT_OPENGL_LIBRARY()
{
    return "SDL_OPENGL_LIBRARY";
}

constexpr auto HINT_EGL_LIBRARY()
{
    return "SDL_EGL_LIBRARY";
}

constexpr auto HINT_OPENGL_ES_DRIVER()
{
    return "SDL_OPENGL_ES_DRIVER";
}

constexpr auto HINT_OPENVR_LIBRARY()
{
    return "SDL_OPENVR_LIBRARY";
}

constexpr auto HINT_ORIENTATIONS()
{
    return "SDL_ORIENTATIONS";
}

constexpr auto HINT_POLL_SENTINEL()
{
    return "SDL_POLL_SENTINEL";
}

constexpr auto HINT_PREFERRED_LOCALES()
{
    return "SDL_PREFERRED_LOCALES";
}

constexpr auto HINT_QUIT_ON_LAST_WINDOW_CLOSE()
{
    return "SDL_QUIT_ON_LAST_WINDOW_CLOSE";
}

constexpr auto HINT_RENDER_DIRECT3D_THREADSAFE()
{
    return "SDL_RENDER_DIRECT3D_THREADSAFE";
}

constexpr auto HINT_RENDER_DIRECT3D11_DEBUG()
{
    return "SDL_RENDER_DIRECT3D11_DEBUG";
}

constexpr auto HINT_RENDER_VULKAN_DEBUG()
{
    return "SDL_RENDER_VULKAN_DEBUG";
}

constexpr auto HINT_RENDER_GPU_DEBUG()
{
    return "SDL_RENDER_GPU_DEBUG";
}

constexpr auto HINT_RENDER_GPU_LOW_POWER()
{
    return "SDL_RENDER_GPU_LOW_POWER";
}

constexpr auto HINT_RENDER_DRIVER()
{
    return "SDL_RENDER_DRIVER";
}

constexpr auto HINT_RENDER_LINE_METHOD()
{
    return "SDL_RENDER_LINE_METHOD";
}

constexpr auto HINT_RENDER_METAL_PREFER_LOW_POWER_DEVICE()
{
    return "SDL_RENDER_METAL_PREFER_LOW_POWER_DEVICE";
}

constexpr auto HINT_RENDER_VSYNC()
{
    return "SDL_RENDER_VSYNC";
}

constexpr auto HINT_RETURN_KEY_HIDES_IME()
{
    return "SDL_RETURN_KEY_HIDES_IME";
}

constexpr auto HINT_ROG_GAMEPAD_MICE()
{
    return "SDL_ROG_GAMEPAD_MICE";
}

constexpr auto HINT_ROG_GAMEPAD_MICE_EXCLUDED()
{
    return "SDL_ROG_GAMEPAD_MICE_EXCLUDED";
}

constexpr auto HINT_RPI_VIDEO_LAYER()
{
    return "SDL_RPI_VIDEO_LAYER";
}

constexpr auto HINT_SCREENSAVER_INHIBIT_ACTIVITY_NAME()
{
    return "SDL_SCREENSAVER_INHIBIT_ACTIVITY_NAME";
}

constexpr auto HINT_SHUTDOWN_DBUS_ON_QUIT()
{
    return "SDL_SHUTDOWN_DBUS_ON_QUIT";
}

constexpr auto HINT_STORAGE_TITLE_DRIVER()
{
    return "SDL_STORAGE_TITLE_DRIVER";
}

constexpr auto HINT_STORAGE_USER_DRIVER()
{
    return "SDL_STORAGE_USER_DRIVER";
}

constexpr auto HINT_THREAD_FORCE_REALTIME_TIME_CRITICAL()
{
    return "SDL_THREAD_FORCE_REALTIME_TIME_CRITICAL";
}

constexpr auto HINT_THREAD_PRIORITY_POLICY()
{
    return "SDL_THREAD_PRIORITY_POLICY";
}

constexpr auto HINT_TIMER_RESOLUTION()
{
    return "SDL_TIMER_RESOLUTION";
}

constexpr auto HINT_TOUCH_MOUSE_EVENTS()
{
    return "SDL_TOUCH_MOUSE_EVENTS";
}

constexpr auto HINT_TRACKPAD_IS_TOUCH_ONLY()
{
    return "SDL_TRACKPAD_IS_TOUCH_ONLY";
}

constexpr auto HINT_TV_REMOTE_AS_JOYSTICK()
{
    return "SDL_TV_REMOTE_AS_JOYSTICK";
}

constexpr auto HINT_VIDEO_ALLOW_SCREENSAVER()
{
    return "SDL_VIDEO_ALLOW_SCREENSAVER";
}

constexpr auto HINT_VIDEO_DISPLAY_PRIORITY()
{
    return "SDL_VIDEO_DISPLAY_PRIORITY";
}

constexpr auto HINT_VIDEO_DOUBLE_BUFFER()
{
    return "SDL_VIDEO_DOUBLE_BUFFER";
}

constexpr auto HINT_VIDEO_DRIVER()
{
    return "SDL_VIDEO_DRIVER";
}

constexpr auto HINT_VIDEO_DUMMY_SAVE_FRAMES()
{
    return "SDL_VIDEO_DUMMY_SAVE_FRAMES";
}

constexpr auto HINT_VIDEO_EGL_ALLOW_GETDISPLAY_FALLBACK()
{
    return "SDL_VIDEO_EGL_ALLOW_GETDISPLAY_FALLBACK";
}

constexpr auto HINT_VIDEO_FORCE_EGL()
{
    return "SDL_VIDEO_FORCE_EGL";
}

constexpr auto HINT_VIDEO_MAC_FULLSCREEN_SPACES()
{
    return "SDL_VIDEO_MAC_FULLSCREEN_SPACES";
}

constexpr auto HINT_VIDEO_MAC_FULLSCREEN_MENU_VISIBILITY()
{
    return "SDL_VIDEO_MAC_FULLSCREEN_MENU_VISIBILITY";
}

constexpr auto HINT_VIDEO_MATCH_EXCLUSIVE_MODE_ON_MOVE()
{
    return "SDL_VIDEO_MATCH_EXCLUSIVE_MODE_ON_MOVE";
}

constexpr auto HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS()
{
    return "SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS";
}

constexpr auto HINT_VIDEO_OFFSCREEN_SAVE_FRAMES()
{
    return "SDL_VIDEO_OFFSCREEN_SAVE_FRAMES";
}

constexpr auto HINT_VIDEO_SYNC_WINDOW_OPERATIONS()
{
    return "SDL_VIDEO_SYNC_WINDOW_OPERATIONS";
}

constexpr auto HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR()
{
    return "SDL_VIDEO_WAYLAND_ALLOW_LIBDECOR";
}

constexpr auto HINT_VIDEO_WAYLAND_MODE_EMULATION()
{
    return "SDL_VIDEO_WAYLAND_MODE_EMULATION";
}

constexpr auto HINT_VIDEO_WAYLAND_MODE_SCALING()
{
    return "SDL_VIDEO_WAYLAND_MODE_SCALING";
}

constexpr auto HINT_VIDEO_WAYLAND_PREFER_LIBDECOR()
{
    return "SDL_VIDEO_WAYLAND_PREFER_LIBDECOR";
}

constexpr auto HINT_VIDEO_WAYLAND_SCALE_TO_DISPLAY()
{
    return "SDL_VIDEO_WAYLAND_SCALE_TO_DISPLAY";
}

constexpr auto HINT_VIDEO_WIN_D3DCOMPILER()
{
    return "SDL_VIDEO_WIN_D3DCOMPILER";
}

constexpr auto HINT_VIDEO_X11_EXTERNAL_WINDOW_INPUT()
{
    return "SDL_VIDEO_X11_EXTERNAL_WINDOW_INPUT";
}

constexpr auto HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR()
{
    return "SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR";
}

constexpr auto HINT_VIDEO_X11_NET_WM_PING()
{
    return "SDL_VIDEO_X11_NET_WM_PING";
}

constexpr auto HINT_VIDEO_X11_NODIRECTCOLOR()
{
    return "SDL_VIDEO_X11_NODIRECTCOLOR";
}

constexpr auto HINT_VIDEO_X11_SCALING_FACTOR()
{
    return "SDL_VIDEO_X11_SCALING_FACTOR";
}

constexpr auto HINT_VIDEO_X11_VISUALID()
{
    return "SDL_VIDEO_X11_VISUALID";
}

constexpr auto HINT_VIDEO_X11_WINDOW_VISUALID()
{
    return "SDL_VIDEO_X11_WINDOW_VISUALID";
}

constexpr auto HINT_VIDEO_X11_XRANDR()
{
    return "SDL_VIDEO_X11_XRANDR";
}

constexpr auto HINT_VITA_ENABLE_BACK_TOUCH()
{
    return "SDL_VITA_ENABLE_BACK_TOUCH";
}

constexpr auto HINT_VITA_ENABLE_FRONT_TOUCH()
{
    return "SDL_VITA_ENABLE_FRONT_TOUCH";
}

constexpr auto HINT_VITA_MODULE_PATH()
{
    return "SDL_VITA_MODULE_PATH";
}

constexpr auto HINT_VITA_PVR_INIT()
{
    return "SDL_VITA_PVR_INIT";
}

constexpr auto HINT_VITA_RESOLUTION()
{
    return "SDL_VITA_RESOLUTION";
}

constexpr auto HINT_VITA_PVR_OPENGL()
{
    return "SDL_VITA_PVR_OPENGL";
}

constexpr auto HINT_VITA_TOUCH_MOUSE_DEVICE()
{
    return "SDL_VITA_TOUCH_MOUSE_DEVICE";
}

constexpr auto HINT_VULKAN_DISPLAY()
{
    return "SDL_VULKAN_DISPLAY";
}

constexpr auto HINT_VULKAN_LIBRARY()
{
    return "SDL_VULKAN_LIBRARY";
}

constexpr auto HINT_WAVE_FACT_CHUNK()
{
    return "SDL_WAVE_FACT_CHUNK";
}

constexpr auto HINT_WAVE_CHUNK_LIMIT()
{
    return "SDL_WAVE_CHUNK_LIMIT";
}

constexpr auto HINT_WAVE_RIFF_CHUNK_SIZE()
{
    return "SDL_WAVE_RIFF_CHUNK_SIZE";
}

constexpr auto HINT_WAVE_TRUNCATION()
{
    return "SDL_WAVE_TRUNCATION";
}

constexpr auto HINT_WINDOW_ACTIVATE_WHEN_RAISED()
{
    return "SDL_WINDOW_ACTIVATE_WHEN_RAISED";
}

constexpr auto HINT_WINDOW_ACTIVATE_WHEN_SHOWN()
{
    return "SDL_WINDOW_ACTIVATE_WHEN_SHOWN";
}

constexpr auto HINT_WINDOW_ALLOW_TOPMOST()
{
    return "SDL_WINDOW_ALLOW_TOPMOST";
}

constexpr auto HINT_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN()
{
    return "SDL_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN";
}

constexpr auto HINT_WINDOWS_CLOSE_ON_ALT_F4()
{
    return "SDL_WINDOWS_CLOSE_ON_ALT_F4";
}

constexpr auto HINT_WINDOWS_ENABLE_MENU_MNEMONICS()
{
    return "SDL_WINDOWS_ENABLE_MENU_MNEMONICS";
}

constexpr auto HINT_WINDOWS_ENABLE_MESSAGELOOP()
{
    return "SDL_WINDOWS_ENABLE_MESSAGELOOP";
}

constexpr auto HINT_WINDOWS_GAMEINPUT()
{
    return "SDL_WINDOWS_GAMEINPUT";
}

constexpr auto HINT_WINDOWS_RAW_KEYBOARD()
{
    return "SDL_WINDOWS_RAW_KEYBOARD";
}

constexpr auto HINT_WINDOWS_FORCE_SEMAPHORE_KERNEL()
{
    return "SDL_WINDOWS_FORCE_SEMAPHORE_KERNEL";
}

constexpr auto HINT_WINDOWS_INTRESOURCE_ICON()
{
    return "SDL_WINDOWS_INTRESOURCE_ICON";
}

constexpr auto HINT_WINDOWS_INTRESOURCE_ICON_SMALL()
{
    return "SDL_WINDOWS_INTRESOURCE_ICON_SMALL";
}

constexpr auto HINT_WINDOWS_USE_D3D9EX()
{
    return "SDL_WINDOWS_USE_D3D9EX";
}

constexpr auto HINT_WINDOWS_ERASE_BACKGROUND_MODE()
{
    return "SDL_WINDOWS_ERASE_BACKGROUND_MODE";
}

constexpr auto HINT_X11_FORCE_OVERRIDE_REDIRECT()
{
    return "SDL_X11_FORCE_OVERRIDE_REDIRECT";
}

constexpr auto HINT_X11_WINDOW_TYPE()
{
    return "SDL_X11_WINDOW_TYPE";
}

constexpr auto HINT_X11_XCB_LIBRARY()
{
    return "SDL_X11_XCB_LIBRARY";
}

constexpr auto HINT_XINPUT_ENABLED()
{
    return "SDL_XINPUT_ENABLED";
}

constexpr auto HINT_ASSERT()
{
    return "SDL_ASSERT";
}

constexpr auto HINT_PEN_MOUSE_EVENTS()
{
    return "SDL_PEN_MOUSE_EVENTS";
}

constexpr auto HINT_PEN_TOUCH_EVENTS()
{
    return "SDL_PEN_TOUCH_EVENTS";
}

constexpr auto HINT_DEBUG_LOGGING()
{
    return "SDL_DEBUG_LOGGING";
}

enum class HintPriority
{
    DEFAULT  = SDL_HINT_DEFAULT,
    NORMAL   = SDL_HINT_NORMAL,
    OVERRIDE = SDL_HINT_OVERRIDE,
};
REGULAR_ENUM(HintPriority);

bool SetHintWithPriority(const char*  name,
                         const char*  value,
                         HintPriority priority)
{
    return SDL_SetHintWithPriority(name, value, (SDL_HintPriority)(priority));
}

bool SetHint(const char* name, const char* value)
{
    return SDL_SetHint(name, value);
}

bool ResetHint(const char* name)
{
    return SDL_ResetHint(name);
}

void ResetHints(void)
{
    SDL_ResetHints();
}

const char* GetHint(const char* name)
{
    return SDL_GetHint(name);
}

bool GetHintBoolean(const char* name, bool default_value)
{
    return SDL_GetHintBoolean(name, default_value);
}

bool AddHintCallback(const char*      name,
                     SDL_HintCallback callback,
                     void*            userdata)
{
    return SDL_AddHintCallback(name, callback, userdata);
}

void RemoveHintCallback(const char*      name,
                        SDL_HintCallback callback,
                        void*            userdata)
{
    SDL_RemoveHintCallback(name, callback, userdata);
}

enum class InitFlags : Uint32
{
    AUDIO    = SDL_INIT_AUDIO,
    VIDEO    = SDL_INIT_VIDEO,
    JOYSTICK = SDL_INIT_JOYSTICK,
    HAPTIC   = SDL_INIT_HAPTIC,
    GAMEPAD  = SDL_INIT_GAMEPAD,
    EVENTS   = SDL_INIT_EVENTS,
    SENSOR   = SDL_INIT_SENSOR,
    CAMERA   = SDL_INIT_CAMERA,
};
BITFLAG_ENUM(InitFlags);

enum class AppResult
{
    CONTINUE = SDL_APP_CONTINUE,
    SUCCESS  = SDL_APP_SUCCESS,
    FAILURE  = SDL_APP_FAILURE,
};
REGULAR_ENUM(AppResult);

bool Init(InitFlags flags)
{
    return SDL_Init((SDL_InitFlags)(flags));
}

bool InitSubSystem(InitFlags flags)
{
    return SDL_InitSubSystem((SDL_InitFlags)(flags));
}

void QuitSubSystem(InitFlags flags)
{
    SDL_QuitSubSystem((SDL_InitFlags)(flags));
}

SDL_InitFlags WasInit(InitFlags flags)
{
    return SDL_WasInit((SDL_InitFlags)(flags));
}

void Quit(void)
{
    SDL_Quit();
}

bool IsMainThread(void)
{
    return SDL_IsMainThread();
}

bool RunOnMainThread(SDL_MainThreadCallback callback,
                     void*                  userdata,
                     bool                   wait_complete)
{
    return SDL_RunOnMainThread(callback, userdata, wait_complete);
}

bool SetAppMetadata(const char* appname,
                    const char* appversion,
                    const char* appidentifier)
{
    return SDL_SetAppMetadata(appname, appversion, appidentifier);
}

bool SetAppMetadataProperty(const char* name, const char* value)
{
    return SDL_SetAppMetadataProperty(name, value);
}

constexpr auto PROP_APP_METADATA_NAME_STRING()
{
    return "SDL.app.metadata.name";
}

constexpr auto PROP_APP_METADATA_VERSION_STRING()
{
    return "SDL.app.metadata.version";
}

constexpr auto PROP_APP_METADATA_IDENTIFIER_STRING()
{
    return "SDL.app.metadata.identifier";
}

constexpr auto PROP_APP_METADATA_CREATOR_STRING()
{
    return "SDL.app.metadata.creator";
}

constexpr auto PROP_APP_METADATA_COPYRIGHT_STRING()
{
    return "SDL.app.metadata.copyright";
}

constexpr auto PROP_APP_METADATA_URL_STRING()
{
    return "SDL.app.metadata.url";
}

constexpr auto PROP_APP_METADATA_TYPE_STRING()
{
    return "SDL.app.metadata.type";
}

const char* GetAppMetadataProperty(const char* name)
{
    return SDL_GetAppMetadataProperty(name);
}

using SDL_SharedObject = SDL_SharedObject;

SDL_SharedObject* LoadObject(const char* sofile)
{
    return SDL_LoadObject(sofile);
}

SDL_FunctionPointer LoadFunction(SDL_SharedObject* handle, const char* name)
{
    return SDL_LoadFunction(handle, name);
}

void UnloadObject(SDL_SharedObject* handle)
{
    SDL_UnloadObject(handle);
}

using Locale = SDL_Locale;

enum class LogCategory
{
    CATEGORY_APPLICATION = SDL_LOG_CATEGORY_APPLICATION,
    CATEGORY_ERROR       = SDL_LOG_CATEGORY_ERROR,
    CATEGORY_ASSERT      = SDL_LOG_CATEGORY_ASSERT,
    CATEGORY_SYSTEM      = SDL_LOG_CATEGORY_SYSTEM,
    CATEGORY_AUDIO       = SDL_LOG_CATEGORY_AUDIO,
    CATEGORY_VIDEO       = SDL_LOG_CATEGORY_VIDEO,
    CATEGORY_RENDER      = SDL_LOG_CATEGORY_RENDER,
    CATEGORY_INPUT       = SDL_LOG_CATEGORY_INPUT,
    CATEGORY_TEST        = SDL_LOG_CATEGORY_TEST,
    CATEGORY_GPU         = SDL_LOG_CATEGORY_GPU,
    CATEGORY_RESERVED2   = SDL_LOG_CATEGORY_RESERVED2,
    CATEGORY_RESERVED3   = SDL_LOG_CATEGORY_RESERVED3,
    CATEGORY_RESERVED4   = SDL_LOG_CATEGORY_RESERVED4,
    CATEGORY_RESERVED5   = SDL_LOG_CATEGORY_RESERVED5,
    CATEGORY_RESERVED6   = SDL_LOG_CATEGORY_RESERVED6,
    CATEGORY_RESERVED7   = SDL_LOG_CATEGORY_RESERVED7,
    CATEGORY_RESERVED8   = SDL_LOG_CATEGORY_RESERVED8,
    CATEGORY_RESERVED9   = SDL_LOG_CATEGORY_RESERVED9,
    CATEGORY_RESERVED10  = SDL_LOG_CATEGORY_RESERVED10,
    CATEGORY_CUSTOM      = SDL_LOG_CATEGORY_CUSTOM,
};
REGULAR_ENUM(LogCategory);

enum class LogPriority
{
    PRIORITY_INVALID  = SDL_LOG_PRIORITY_INVALID,
    PRIORITY_TRACE    = SDL_LOG_PRIORITY_TRACE,
    PRIORITY_VERBOSE  = SDL_LOG_PRIORITY_VERBOSE,
    PRIORITY_DEBUG    = SDL_LOG_PRIORITY_DEBUG,
    PRIORITY_INFO     = SDL_LOG_PRIORITY_INFO,
    PRIORITY_WARN     = SDL_LOG_PRIORITY_WARN,
    PRIORITY_ERROR    = SDL_LOG_PRIORITY_ERROR,
    PRIORITY_CRITICAL = SDL_LOG_PRIORITY_CRITICAL,
    PRIORITY_COUNT    = SDL_LOG_PRIORITY_COUNT,
};
REGULAR_ENUM(LogPriority);

void SetLogPriorities(LogPriority priority)
{
    SDL_SetLogPriorities((SDL_LogPriority)(priority));
}

void SetLogPriority(int category, LogPriority priority)
{
    SDL_SetLogPriority(category, (SDL_LogPriority)(priority));
}

SDL_LogPriority GetLogPriority(int category)
{
    return SDL_GetLogPriority(category);
}

void ResetLogPriorities(void)
{
    SDL_ResetLogPriorities();
}

bool SetLogPriorityPrefix(LogPriority priority, const char* prefix)
{
    return SDL_SetLogPriorityPrefix((SDL_LogPriority)(priority), prefix);
}

SDL_LogOutputFunction GetDefaultLogOutputFunction(void)
{
    return SDL_GetDefaultLogOutputFunction();
}

void GetLogOutputFunction(SDL_LogOutputFunction* callback, void** userdata)
{
    SDL_GetLogOutputFunction(callback, userdata);
}

void SetLogOutputFunction(SDL_LogOutputFunction callback, void* userdata)
{
    SDL_SetLogOutputFunction(callback, userdata);
}

enum class MessageBoxFlags : Uint32
{
    ERROR                 = SDL_MESSAGEBOX_ERROR,
    WARNING               = SDL_MESSAGEBOX_WARNING,
    INFORMATION           = SDL_MESSAGEBOX_INFORMATION,
    BUTTONS_LEFT_TO_RIGHT = SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT,
    BUTTONS_RIGHT_TO_LEFT = SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT,
};
BITFLAG_ENUM(MessageBoxFlags);

enum class MessageBoxButtonFlags : Uint32
{
    BUTTON_RETURNKEY_DEFAULT = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
    BUTTON_ESCAPEKEY_DEFAULT = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
};
BITFLAG_ENUM(MessageBoxButtonFlags);

using MessageBoxButtonData = SDL_MessageBoxButtonData;

using MessageBoxColor = SDL_MessageBoxColor;

enum class MessageBoxColorType
{
    COLOR_BACKGROUND        = SDL_MESSAGEBOX_COLOR_BACKGROUND,
    COLOR_TEXT              = SDL_MESSAGEBOX_COLOR_TEXT,
    COLOR_BUTTON_BORDER     = SDL_MESSAGEBOX_COLOR_BUTTON_BORDER,
    COLOR_BUTTON_BACKGROUND = SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND,
    COLOR_BUTTON_SELECTED   = SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED,
    COLOR_COUNT             = SDL_MESSAGEBOX_COLOR_COUNT,
};
REGULAR_ENUM(MessageBoxColorType);

using MessageBoxColorScheme = SDL_MessageBoxColorScheme;

using MessageBoxData = SDL_MessageBoxData;

bool ShowMessageBox(const SDL_MessageBoxData* messageboxdata, int* buttonid)
{
    return SDL_ShowMessageBox(messageboxdata, buttonid);
}

bool ShowSimpleMessageBox(MessageBoxFlags flags,
                          const char*     title,
                          const char*     message,
                          SDL_Window*     window)
{
    return SDL_ShowSimpleMessageBox(
        (SDL_MessageBoxFlags)(flags), title, message, window);
}

using MetalView = void;

SDL_MetalView Metal_CreateView(SDL_Window* window)
{
    return SDL_Metal_CreateView(window);
}

void Metal_DestroyView(SDL_MetalView view)
{
    SDL_Metal_DestroyView(view);
}

void* Metal_GetLayer(SDL_MetalView view)
{
    return SDL_Metal_GetLayer(view);
}

bool OpenURL(const char* url)
{
    return SDL_OpenURL(url);
}

const char* GetPlatform(void)
{
    return SDL_GetPlatform();
}

using SDL_Process = SDL_Process;

SDL_Process* CreateProcess(const char** args, bool pipe_stdio)
{
    return SDL_CreateProcess(args, pipe_stdio);
}

enum class ProcessIO
{
    STDIO_INHERITED = SDL_PROCESS_STDIO_INHERITED,
    STDIO_NULL      = SDL_PROCESS_STDIO_NULL,
    STDIO_APP       = SDL_PROCESS_STDIO_APP,
    STDIO_REDIRECT  = SDL_PROCESS_STDIO_REDIRECT,
};
REGULAR_ENUM(ProcessIO);

SDL_Process* CreateProcessWithProperties(SDL_PropertiesID props)
{
    return SDL_CreateProcessWithProperties(props);
}

constexpr auto PROP_PROCESS_CREATE_ARGS_POINTER()
{
    return "SDL.process.create.args";
}

constexpr auto PROP_PROCESS_CREATE_ENVIRONMENT_POINTER()
{
    return "SDL.process.create.environment";
}

constexpr auto PROP_PROCESS_CREATE_WORKING_DIRECTORY_STRING()
{
    return "SDL.process.create.working_directory";
}

constexpr auto PROP_PROCESS_CREATE_STDIN_NUMBER()
{
    return "SDL.process.create.stdin_option";
}

constexpr auto PROP_PROCESS_CREATE_STDIN_POINTER()
{
    return "SDL.process.create.stdin_source";
}

constexpr auto PROP_PROCESS_CREATE_STDOUT_NUMBER()
{
    return "SDL.process.create.stdout_option";
}

constexpr auto PROP_PROCESS_CREATE_STDOUT_POINTER()
{
    return "SDL.process.create.stdout_source";
}

constexpr auto PROP_PROCESS_CREATE_STDERR_NUMBER()
{
    return "SDL.process.create.stderr_option";
}

constexpr auto PROP_PROCESS_CREATE_STDERR_POINTER()
{
    return "SDL.process.create.stderr_source";
}

constexpr auto PROP_PROCESS_CREATE_STDERR_TO_STDOUT_BOOLEAN()
{
    return "SDL.process.create.stderr_to_stdout";
}

constexpr auto PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN()
{
    return "SDL.process.create.background";
}

constexpr auto PROP_PROCESS_CREATE_CMDLINE_STRING()
{
    return "SDL.process.create.cmdline";
}

SDL_PropertiesID GetProcessProperties(SDL_Process* process)
{
    return SDL_GetProcessProperties(process);
}

constexpr auto PROP_PROCESS_PID_NUMBER()
{
    return "SDL.process.pid";
}

constexpr auto PROP_PROCESS_STDIN_POINTER()
{
    return "SDL.process.stdin";
}

constexpr auto PROP_PROCESS_STDOUT_POINTER()
{
    return "SDL.process.stdout";
}

constexpr auto PROP_PROCESS_STDERR_POINTER()
{
    return "SDL.process.stderr";
}

constexpr auto PROP_PROCESS_BACKGROUND_BOOLEAN()
{
    return "SDL.process.background";
}

void* ReadProcess(SDL_Process* process, size_t* datasize, int* exitcode)
{
    return SDL_ReadProcess(process, datasize, exitcode);
}

SDL_IOStream* GetProcessInput(SDL_Process* process)
{
    return SDL_GetProcessInput(process);
}

SDL_IOStream* GetProcessOutput(SDL_Process* process)
{
    return SDL_GetProcessOutput(process);
}

bool KillProcess(SDL_Process* process, bool force)
{
    return SDL_KillProcess(process, force);
}

bool WaitProcess(SDL_Process* process, bool block, int* exitcode)
{
    return SDL_WaitProcess(process, block, exitcode);
}

void DestroyProcess(SDL_Process* process)
{
    SDL_DestroyProcess(process);
}

constexpr auto SOFTWARE_RENDERER()
{
    return "software";
}

using Vertex = SDL_Vertex;

enum class TextureAccess
{
    STATIC    = SDL_TEXTUREACCESS_STATIC,
    STREAMING = SDL_TEXTUREACCESS_STREAMING,
    TARGET    = SDL_TEXTUREACCESS_TARGET,
};
REGULAR_ENUM(TextureAccess);

enum class TextureAddressMode
{
    ADDRESS_INVALID = SDL_TEXTURE_ADDRESS_INVALID,
    ADDRESS_AUTO    = SDL_TEXTURE_ADDRESS_AUTO,
    ADDRESS_CLAMP   = SDL_TEXTURE_ADDRESS_CLAMP,
    ADDRESS_WRAP    = SDL_TEXTURE_ADDRESS_WRAP,
};
REGULAR_ENUM(TextureAddressMode);

enum class RendererLogicalPresentation
{
    LOGICAL_PRESENTATION_DISABLED      = SDL_LOGICAL_PRESENTATION_DISABLED,
    LOGICAL_PRESENTATION_STRETCH       = SDL_LOGICAL_PRESENTATION_STRETCH,
    LOGICAL_PRESENTATION_LETTERBOX     = SDL_LOGICAL_PRESENTATION_LETTERBOX,
    LOGICAL_PRESENTATION_OVERSCAN      = SDL_LOGICAL_PRESENTATION_OVERSCAN,
    LOGICAL_PRESENTATION_INTEGER_SCALE = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE,
};
REGULAR_ENUM(RendererLogicalPresentation);

using SDL_Renderer = SDL_Renderer;

using SDL_Texture = SDL_Texture;

int GetNumRenderDrivers(void)
{
    return SDL_GetNumRenderDrivers();
}

const char* GetRenderDriver(int index)
{
    return SDL_GetRenderDriver(index);
}

bool CreateWindowAndRenderer(const char*    title,
                             int            width,
                             int            height,
                             WindowFlags    window_flags,
                             SDL_Window**   window,
                             SDL_Renderer** renderer)
{
    return SDL_CreateWindowAndRenderer(title,
                                       width,
                                       height,
                                       (SDL_WindowFlags)(window_flags),
                                       window,
                                       renderer);
}

SDL_Renderer* CreateRenderer(SDL_Window* window, const char* name)
{
    return SDL_CreateRenderer(window, name);
}

SDL_Renderer* CreateRendererWithProperties(SDL_PropertiesID props)
{
    return SDL_CreateRendererWithProperties(props);
}

constexpr auto PROP_RENDERER_CREATE_NAME_STRING()
{
    return "SDL.renderer.create.name";
}

constexpr auto PROP_RENDERER_CREATE_WINDOW_POINTER()
{
    return "SDL.renderer.create.window";
}

constexpr auto PROP_RENDERER_CREATE_SURFACE_POINTER()
{
    return "SDL.renderer.create.surface";
}

constexpr auto PROP_RENDERER_CREATE_OUTPUT_COLORSPACE_NUMBER()
{
    return "SDL.renderer.create.output_colorspace";
}

constexpr auto PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER()
{
    return "SDL.renderer.create.present_vsync";
}

constexpr auto PROP_RENDERER_CREATE_GPU_SHADERS_SPIRV_BOOLEAN()
{
    return "SDL.renderer.create.gpu.shaders_spirv";
}

constexpr auto PROP_RENDERER_CREATE_GPU_SHADERS_DXIL_BOOLEAN()
{
    return "SDL.renderer.create.gpu.shaders_dxil";
}

constexpr auto PROP_RENDERER_CREATE_GPU_SHADERS_MSL_BOOLEAN()
{
    return "SDL.renderer.create.gpu.shaders_msl";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_INSTANCE_POINTER()
{
    return "SDL.renderer.create.vulkan.instance";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_SURFACE_NUMBER()
{
    return "SDL.renderer.create.vulkan.surface";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_PHYSICAL_DEVICE_POINTER()
{
    return "SDL.renderer.create.vulkan.physical_device";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_DEVICE_POINTER()
{
    return "SDL.renderer.create.vulkan.device";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_GRAPHICS_QUEUE_FAMILY_INDEX_NUMBER()
{
    return "SDL.renderer.create.vulkan.graphics_queue_family_index";
}

constexpr auto PROP_RENDERER_CREATE_VULKAN_PRESENT_QUEUE_FAMILY_INDEX_NUMBER()
{
    return "SDL.renderer.create.vulkan.present_queue_family_index";
}

SDL_Renderer* CreateGPURenderer(SDL_Window*     window,
                                GPUShaderFormat format_flags,
                                SDL_GPUDevice** device)
{
    return SDL_CreateGPURenderer(*device, window);
}

SDL_Renderer* CreateSoftwareRenderer(SDL_Surface* surface)
{
    return SDL_CreateSoftwareRenderer(surface);
}

SDL_Renderer* GetRenderer(SDL_Window* window)
{
    return SDL_GetRenderer(window);
}

SDL_Window* GetRenderWindow(SDL_Renderer* renderer)
{
    return SDL_GetRenderWindow(renderer);
}

const char* GetRendererName(SDL_Renderer* renderer)
{
    return SDL_GetRendererName(renderer);
}

SDL_PropertiesID GetRendererProperties(SDL_Renderer* renderer)
{
    return SDL_GetRendererProperties(renderer);
}

constexpr auto PROP_RENDERER_NAME_STRING()
{
    return "SDL.renderer.name";
}

constexpr auto PROP_RENDERER_WINDOW_POINTER()
{
    return "SDL.renderer.window";
}

constexpr auto PROP_RENDERER_SURFACE_POINTER()
{
    return "SDL.renderer.surface";
}

constexpr auto PROP_RENDERER_VSYNC_NUMBER()
{
    return "SDL.renderer.vsync";
}

constexpr auto PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER()
{
    return "SDL.renderer.max_texture_size";
}

constexpr auto PROP_RENDERER_TEXTURE_FORMATS_POINTER()
{
    return "SDL.renderer.texture_formats";
}

constexpr auto PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER()
{
    return "SDL.renderer.output_colorspace";
}

constexpr auto PROP_RENDERER_HDR_ENABLED_BOOLEAN()
{
    return "SDL.renderer.HDR_enabled";
}

constexpr auto PROP_RENDERER_SDR_WHITE_POINT_FLOAT()
{
    return "SDL.renderer.SDR_white_point";
}

constexpr auto PROP_RENDERER_HDR_HEADROOM_FLOAT()
{
    return "SDL.renderer.HDR_headroom";
}

constexpr auto PROP_RENDERER_D3D9_DEVICE_POINTER()
{
    return "SDL.renderer.d3d9.device";
}

constexpr auto PROP_RENDERER_D3D11_DEVICE_POINTER()
{
    return "SDL.renderer.d3d11.device";
}

constexpr auto PROP_RENDERER_D3D11_SWAPCHAIN_POINTER()
{
    return "SDL.renderer.d3d11.swap_chain";
}

constexpr auto PROP_RENDERER_D3D12_DEVICE_POINTER()
{
    return "SDL.renderer.d3d12.device";
}

constexpr auto PROP_RENDERER_D3D12_SWAPCHAIN_POINTER()
{
    return "SDL.renderer.d3d12.swap_chain";
}

constexpr auto PROP_RENDERER_D3D12_COMMAND_QUEUE_POINTER()
{
    return "SDL.renderer.d3d12.command_queue";
}

constexpr auto PROP_RENDERER_VULKAN_INSTANCE_POINTER()
{
    return "SDL.renderer.vulkan.instance";
}

constexpr auto PROP_RENDERER_VULKAN_SURFACE_NUMBER()
{
    return "SDL.renderer.vulkan.surface";
}

constexpr auto PROP_RENDERER_VULKAN_PHYSICAL_DEVICE_POINTER()
{
    return "SDL.renderer.vulkan.physical_device";
}

constexpr auto PROP_RENDERER_VULKAN_DEVICE_POINTER()
{
    return "SDL.renderer.vulkan.device";
}

constexpr auto PROP_RENDERER_VULKAN_GRAPHICS_QUEUE_FAMILY_INDEX_NUMBER()
{
    return "SDL.renderer.vulkan.graphics_queue_family_index";
}

constexpr auto PROP_RENDERER_VULKAN_PRESENT_QUEUE_FAMILY_INDEX_NUMBER()
{
    return "SDL.renderer.vulkan.present_queue_family_index";
}

constexpr auto PROP_RENDERER_VULKAN_SWAPCHAIN_IMAGE_COUNT_NUMBER()
{
    return "SDL.renderer.vulkan.swapchain_image_count";
}

constexpr auto PROP_RENDERER_GPU_DEVICE_POINTER()
{
    return "SDL.renderer.gpu.device";
}

bool GetRenderOutputSize(SDL_Renderer* renderer, int* w, int* h)
{
    return SDL_GetRenderOutputSize(renderer, w, h);
}

bool GetCurrentRenderOutputSize(SDL_Renderer* renderer, int* w, int* h)
{
    return SDL_GetCurrentRenderOutputSize(renderer, w, h);
}

SDL_Texture* CreateTexture(SDL_Renderer* renderer,
                           PixelFormat   format,
                           TextureAccess access,
                           int           w,
                           int           h)
{
    return SDL_CreateTexture(
        renderer, (SDL_PixelFormat)(format), (SDL_TextureAccess)(access), w, h);
}

SDL_Texture* CreateTextureFromSurface(SDL_Renderer* renderer,
                                      SDL_Surface*  surface)
{
    return SDL_CreateTextureFromSurface(renderer, surface);
}

SDL_Texture* CreateTextureWithProperties(SDL_Renderer*    renderer,
                                         SDL_PropertiesID props)
{
    return SDL_CreateTextureWithProperties(renderer, props);
}

constexpr auto PROP_TEXTURE_CREATE_COLORSPACE_NUMBER()
{
    return "SDL.texture.create.colorspace";
}

constexpr auto PROP_TEXTURE_CREATE_FORMAT_NUMBER()
{
    return "SDL.texture.create.format";
}

constexpr auto PROP_TEXTURE_CREATE_ACCESS_NUMBER()
{
    return "SDL.texture.create.access";
}

constexpr auto PROP_TEXTURE_CREATE_WIDTH_NUMBER()
{
    return "SDL.texture.create.width";
}

constexpr auto PROP_TEXTURE_CREATE_HEIGHT_NUMBER()
{
    return "SDL.texture.create.height";
}

constexpr auto PROP_TEXTURE_CREATE_SDR_WHITE_POINT_FLOAT()
{
    return "SDL.texture.create.SDR_white_point";
}

constexpr auto PROP_TEXTURE_CREATE_HDR_HEADROOM_FLOAT()
{
    return "SDL.texture.create.HDR_headroom";
}

constexpr auto PROP_TEXTURE_CREATE_D3D11_TEXTURE_POINTER()
{
    return "SDL.texture.create.d3d11.texture";
}

constexpr auto PROP_TEXTURE_CREATE_D3D11_TEXTURE_U_POINTER()
{
    return "SDL.texture.create.d3d11.texture_u";
}

constexpr auto PROP_TEXTURE_CREATE_D3D11_TEXTURE_V_POINTER()
{
    return "SDL.texture.create.d3d11.texture_v";
}

constexpr auto PROP_TEXTURE_CREATE_D3D12_TEXTURE_POINTER()
{
    return "SDL.texture.create.d3d12.texture";
}

constexpr auto PROP_TEXTURE_CREATE_D3D12_TEXTURE_U_POINTER()
{
    return "SDL.texture.create.d3d12.texture_u";
}

constexpr auto PROP_TEXTURE_CREATE_D3D12_TEXTURE_V_POINTER()
{
    return "SDL.texture.create.d3d12.texture_v";
}

constexpr auto PROP_TEXTURE_CREATE_METAL_PIXELBUFFER_POINTER()
{
    return "SDL.texture.create.metal.pixelbuffer";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGL_TEXTURE_NUMBER()
{
    return "SDL.texture.create.opengl.texture";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGL_TEXTURE_UV_NUMBER()
{
    return "SDL.texture.create.opengl.texture_uv";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGL_TEXTURE_U_NUMBER()
{
    return "SDL.texture.create.opengl.texture_u";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGL_TEXTURE_V_NUMBER()
{
    return "SDL.texture.create.opengl.texture_v";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGLES2_TEXTURE_NUMBER()
{
    return "SDL.texture.create.opengles2.texture";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGLES2_TEXTURE_UV_NUMBER()
{
    return "SDL.texture.create.opengles2.texture_uv";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGLES2_TEXTURE_U_NUMBER()
{
    return "SDL.texture.create.opengles2.texture_u";
}

constexpr auto PROP_TEXTURE_CREATE_OPENGLES2_TEXTURE_V_NUMBER()
{
    return "SDL.texture.create.opengles2.texture_v";
}

constexpr auto PROP_TEXTURE_CREATE_VULKAN_TEXTURE_NUMBER()
{
    return "SDL.texture.create.vulkan.texture";
}

SDL_PropertiesID GetTextureProperties(SDL_Texture* texture)
{
    return SDL_GetTextureProperties(texture);
}

constexpr auto PROP_TEXTURE_COLORSPACE_NUMBER()
{
    return "SDL.texture.colorspace";
}

constexpr auto PROP_TEXTURE_FORMAT_NUMBER()
{
    return "SDL.texture.format";
}

constexpr auto PROP_TEXTURE_ACCESS_NUMBER()
{
    return "SDL.texture.access";
}

constexpr auto PROP_TEXTURE_WIDTH_NUMBER()
{
    return "SDL.texture.width";
}

constexpr auto PROP_TEXTURE_HEIGHT_NUMBER()
{
    return "SDL.texture.height";
}

constexpr auto PROP_TEXTURE_SDR_WHITE_POINT_FLOAT()
{
    return "SDL.texture.SDR_white_point";
}

constexpr auto PROP_TEXTURE_HDR_HEADROOM_FLOAT()
{
    return "SDL.texture.HDR_headroom";
}

constexpr auto PROP_TEXTURE_D3D11_TEXTURE_POINTER()
{
    return "SDL.texture.d3d11.texture";
}

constexpr auto PROP_TEXTURE_D3D11_TEXTURE_U_POINTER()
{
    return "SDL.texture.d3d11.texture_u";
}

constexpr auto PROP_TEXTURE_D3D11_TEXTURE_V_POINTER()
{
    return "SDL.texture.d3d11.texture_v";
}

constexpr auto PROP_TEXTURE_D3D12_TEXTURE_POINTER()
{
    return "SDL.texture.d3d12.texture";
}

constexpr auto PROP_TEXTURE_D3D12_TEXTURE_U_POINTER()
{
    return "SDL.texture.d3d12.texture_u";
}

constexpr auto PROP_TEXTURE_D3D12_TEXTURE_V_POINTER()
{
    return "SDL.texture.d3d12.texture_v";
}

constexpr auto PROP_TEXTURE_OPENGL_TEXTURE_NUMBER()
{
    return "SDL.texture.opengl.texture";
}

constexpr auto PROP_TEXTURE_OPENGL_TEXTURE_UV_NUMBER()
{
    return "SDL.texture.opengl.texture_uv";
}

constexpr auto PROP_TEXTURE_OPENGL_TEXTURE_U_NUMBER()
{
    return "SDL.texture.opengl.texture_u";
}

constexpr auto PROP_TEXTURE_OPENGL_TEXTURE_V_NUMBER()
{
    return "SDL.texture.opengl.texture_v";
}

constexpr auto PROP_TEXTURE_OPENGL_TEXTURE_TARGET_NUMBER()
{
    return "SDL.texture.opengl.target";
}

constexpr auto PROP_TEXTURE_OPENGL_TEX_W_FLOAT()
{
    return "SDL.texture.opengl.tex_w";
}

constexpr auto PROP_TEXTURE_OPENGL_TEX_H_FLOAT()
{
    return "SDL.texture.opengl.tex_h";
}

constexpr auto PROP_TEXTURE_OPENGLES2_TEXTURE_NUMBER()
{
    return "SDL.texture.opengles2.texture";
}

constexpr auto PROP_TEXTURE_OPENGLES2_TEXTURE_UV_NUMBER()
{
    return "SDL.texture.opengles2.texture_uv";
}

constexpr auto PROP_TEXTURE_OPENGLES2_TEXTURE_U_NUMBER()
{
    return "SDL.texture.opengles2.texture_u";
}

constexpr auto PROP_TEXTURE_OPENGLES2_TEXTURE_V_NUMBER()
{
    return "SDL.texture.opengles2.texture_v";
}

constexpr auto PROP_TEXTURE_OPENGLES2_TEXTURE_TARGET_NUMBER()
{
    return "SDL.texture.opengles2.target";
}

constexpr auto PROP_TEXTURE_VULKAN_TEXTURE_NUMBER()
{
    return "SDL.texture.vulkan.texture";
}

SDL_Renderer* GetRendererFromTexture(SDL_Texture* texture)
{
    return SDL_GetRendererFromTexture(texture);
}

bool GetTextureSize(SDL_Texture* texture, float* w, float* h)
{
    return SDL_GetTextureSize(texture, w, h);
}

bool SetTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_SetTextureColorMod(texture, r, g, b);
}

bool SetTextureColorModFloat(SDL_Texture* texture, float r, float g, float b)
{
    return SDL_SetTextureColorModFloat(texture, r, g, b);
}

bool GetTextureColorMod(SDL_Texture* texture, Uint8* r, Uint8* g, Uint8* b)
{
    return SDL_GetTextureColorMod(texture, r, g, b);
}

bool GetTextureColorModFloat(SDL_Texture* texture, float* r, float* g, float* b)
{
    return SDL_GetTextureColorModFloat(texture, r, g, b);
}

bool SetTextureAlphaMod(SDL_Texture* texture, Uint8 alpha)
{
    return SDL_SetTextureAlphaMod(texture, alpha);
}

bool SetTextureAlphaModFloat(SDL_Texture* texture, float alpha)
{
    return SDL_SetTextureAlphaModFloat(texture, alpha);
}

bool GetTextureAlphaMod(SDL_Texture* texture, Uint8* alpha)
{
    return SDL_GetTextureAlphaMod(texture, alpha);
}

bool GetTextureAlphaModFloat(SDL_Texture* texture, float* alpha)
{
    return SDL_GetTextureAlphaModFloat(texture, alpha);
}

bool SetTextureBlendMode(SDL_Texture* texture, BlendMode blendMode)
{
    return SDL_SetTextureBlendMode(texture, (SDL_BlendMode)(blendMode));
}

bool GetTextureBlendMode(SDL_Texture* texture, BlendMode* blendMode)
{
    return SDL_GetTextureBlendMode(texture, (SDL_BlendMode*)(blendMode));
}

bool SetTextureScaleMode(SDL_Texture* texture, ScaleMode scaleMode)
{
    return SDL_SetTextureScaleMode(texture, (SDL_ScaleMode)(scaleMode));
}

bool GetTextureScaleMode(SDL_Texture* texture, ScaleMode* scaleMode)
{
    return SDL_GetTextureScaleMode(texture, (SDL_ScaleMode*)(scaleMode));
}

bool UpdateTexture(SDL_Texture*    texture,
                   const SDL_Rect* rect,
                   const void*     pixels,
                   int             pitch)
{
    return SDL_UpdateTexture(texture, rect, pixels, pitch);
}

bool UpdateYUVTexture(SDL_Texture*    texture,
                      const SDL_Rect* rect,
                      const Uint8*    Yplane,
                      int             Ypitch,
                      const Uint8*    Uplane,
                      int             Upitch,
                      const Uint8*    Vplane,
                      int             Vpitch)
{
    return SDL_UpdateYUVTexture(
        texture, rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch);
}

bool UpdateNVTexture(SDL_Texture*    texture,
                     const SDL_Rect* rect,
                     const Uint8*    Yplane,
                     int             Ypitch,
                     const Uint8*    UVplane,
                     int             UVpitch)
{
    return SDL_UpdateNVTexture(texture, rect, Yplane, Ypitch, UVplane, UVpitch);
}

bool LockTexture(SDL_Texture*    texture,
                 const SDL_Rect* rect,
                 void**          pixels,
                 int*            pitch)
{
    return SDL_LockTexture(texture, rect, pixels, pitch);
}

bool LockTextureToSurface(SDL_Texture*    texture,
                          const SDL_Rect* rect,
                          SDL_Surface**   surface)
{
    return SDL_LockTextureToSurface(texture, rect, surface);
}

void UnlockTexture(SDL_Texture* texture)
{
    SDL_UnlockTexture(texture);
}

bool SetRenderTarget(SDL_Renderer* renderer, SDL_Texture* texture)
{
    return SDL_SetRenderTarget(renderer, texture);
}

SDL_Texture* GetRenderTarget(SDL_Renderer* renderer)
{
    return SDL_GetRenderTarget(renderer);
}

bool SetRenderLogicalPresentation(SDL_Renderer*               renderer,
                                  int                         w,
                                  int                         h,
                                  RendererLogicalPresentation mode)
{
    return SDL_SetRenderLogicalPresentation(
        renderer, w, h, (SDL_RendererLogicalPresentation)(mode));
}

bool GetRenderLogicalPresentation(SDL_Renderer*                renderer,
                                  int*                         w,
                                  int*                         h,
                                  RendererLogicalPresentation* mode)
{
    return SDL_GetRenderLogicalPresentation(
        renderer, w, h, (SDL_RendererLogicalPresentation*)(mode));
}

bool GetRenderLogicalPresentationRect(SDL_Renderer* renderer, SDL_FRect* rect)
{
    return SDL_GetRenderLogicalPresentationRect(renderer, rect);
}

bool RenderCoordinatesFromWindow(
    SDL_Renderer* renderer, float window_x, float window_y, float* x, float* y)
{
    return SDL_RenderCoordinatesFromWindow(renderer, window_x, window_y, x, y);
}

bool RenderCoordinatesToWindow(
    SDL_Renderer* renderer, float x, float y, float* window_x, float* window_y)
{
    return SDL_RenderCoordinatesToWindow(renderer, x, y, window_x, window_y);
}

bool ConvertEventToRenderCoordinates(SDL_Renderer* renderer, SDL_Event* event)
{
    return SDL_ConvertEventToRenderCoordinates(renderer, event);
}

bool SetRenderViewport(SDL_Renderer* renderer, const SDL_Rect* rect)
{
    return SDL_SetRenderViewport(renderer, rect);
}

bool GetRenderViewport(SDL_Renderer* renderer, SDL_Rect* rect)
{
    return SDL_GetRenderViewport(renderer, rect);
}

bool RenderViewportSet(SDL_Renderer* renderer)
{
    return SDL_RenderViewportSet(renderer);
}

bool GetRenderSafeArea(SDL_Renderer* renderer, SDL_Rect* rect)
{
    return SDL_GetRenderSafeArea(renderer, rect);
}

bool SetRenderClipRect(SDL_Renderer* renderer, const SDL_Rect* rect)
{
    return SDL_SetRenderClipRect(renderer, rect);
}

bool GetRenderClipRect(SDL_Renderer* renderer, SDL_Rect* rect)
{
    return SDL_GetRenderClipRect(renderer, rect);
}

bool RenderClipEnabled(SDL_Renderer* renderer)
{
    return SDL_RenderClipEnabled(renderer);
}

bool SetRenderScale(SDL_Renderer* renderer, float scaleX, float scaleY)
{
    return SDL_SetRenderScale(renderer, scaleX, scaleY);
}

bool GetRenderScale(SDL_Renderer* renderer, float* scaleX, float* scaleY)
{
    return SDL_GetRenderScale(renderer, scaleX, scaleY);
}

bool SetRenderDrawColor(
    SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

bool SetRenderDrawColorFloat(
    SDL_Renderer* renderer, float r, float g, float b, float a)
{
    return SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
}

bool GetRenderDrawColor(
    SDL_Renderer* renderer, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    return SDL_GetRenderDrawColor(renderer, r, g, b, a);
}

bool GetRenderDrawColorFloat(
    SDL_Renderer* renderer, float* r, float* g, float* b, float* a)
{
    return SDL_GetRenderDrawColorFloat(renderer, r, g, b, a);
}

bool SetRenderColorScale(SDL_Renderer* renderer, float scale)
{
    return SDL_SetRenderColorScale(renderer, scale);
}

bool GetRenderColorScale(SDL_Renderer* renderer, float* scale)
{
    return SDL_GetRenderColorScale(renderer, scale);
}

bool SetRenderDrawBlendMode(SDL_Renderer* renderer, BlendMode blendMode)
{
    return SDL_SetRenderDrawBlendMode(renderer, (SDL_BlendMode)(blendMode));
}

bool GetRenderDrawBlendMode(SDL_Renderer* renderer, BlendMode* blendMode)
{
    return SDL_GetRenderDrawBlendMode(renderer, (SDL_BlendMode*)(blendMode));
}

bool RenderClear(SDL_Renderer* renderer)
{
    return SDL_RenderClear(renderer);
}

bool RenderPoint(SDL_Renderer* renderer, float x, float y)
{
    return SDL_RenderPoint(renderer, x, y);
}

bool RenderPoints(SDL_Renderer* renderer, const SDL_FPoint* points, int count)
{
    return SDL_RenderPoints(renderer, points, count);
}

bool RenderLine(SDL_Renderer* renderer, float x1, float y1, float x2, float y2)
{
    return SDL_RenderLine(renderer, x1, y1, x2, y2);
}

bool RenderLines(SDL_Renderer* renderer, const SDL_FPoint* points, int count)
{
    return SDL_RenderLines(renderer, points, count);
}

bool RenderRect(SDL_Renderer* renderer, const SDL_FRect* rect)
{
    return SDL_RenderRect(renderer, rect);
}

bool RenderRects(SDL_Renderer* renderer, const SDL_FRect* rects, int count)
{
    return SDL_RenderRects(renderer, rects, count);
}

bool RenderFillRect(SDL_Renderer* renderer, const SDL_FRect* rect)
{
    return SDL_RenderFillRect(renderer, rect);
}

bool RenderFillRects(SDL_Renderer* renderer, const SDL_FRect* rects, int count)
{
    return SDL_RenderFillRects(renderer, rects, count);
}

bool RenderTexture(SDL_Renderer*    renderer,
                   SDL_Texture*     texture,
                   const SDL_FRect* srcrect,
                   const SDL_FRect* dstrect)
{
    return SDL_RenderTexture(renderer, texture, srcrect, dstrect);
}

bool RenderTextureRotated(SDL_Renderer*     renderer,
                          SDL_Texture*      texture,
                          const SDL_FRect*  srcrect,
                          const SDL_FRect*  dstrect,
                          double            angle,
                          const SDL_FPoint* center,
                          FlipMode          flip)
{
    return SDL_RenderTextureRotated(renderer,
                                    texture,
                                    srcrect,
                                    dstrect,
                                    angle,
                                    center,
                                    (SDL_FlipMode)(flip));
}

bool RenderTextureAffine(SDL_Renderer*     renderer,
                         SDL_Texture*      texture,
                         const SDL_FRect*  srcrect,
                         const SDL_FPoint* origin,
                         const SDL_FPoint* right,
                         const SDL_FPoint* down)
{
    return SDL_RenderTextureAffine(
        renderer, texture, srcrect, origin, right, down);
}

bool RenderTextureTiled(SDL_Renderer*    renderer,
                        SDL_Texture*     texture,
                        const SDL_FRect* srcrect,
                        float            scale,
                        const SDL_FRect* dstrect)
{
    return SDL_RenderTextureTiled(renderer, texture, srcrect, scale, dstrect);
}

bool RenderTexture9Grid(SDL_Renderer*    renderer,
                        SDL_Texture*     texture,
                        const SDL_FRect* srcrect,
                        float            left_width,
                        float            right_width,
                        float            top_height,
                        float            bottom_height,
                        float            scale,
                        const SDL_FRect* dstrect)
{
    return SDL_RenderTexture9Grid(renderer,
                                  texture,
                                  srcrect,
                                  left_width,
                                  right_width,
                                  top_height,
                                  bottom_height,
                                  scale,
                                  dstrect);
}

bool RenderTexture9GridTiled(SDL_Renderer*    renderer,
                             SDL_Texture*     texture,
                             const SDL_FRect* srcrect,
                             float            left_width,
                             float            right_width,
                             float            top_height,
                             float            bottom_height,
                             float            scale,
                             const SDL_FRect* dstrect,
                             float            tileScale)
{
    return SDL_RenderTexture9GridTiled(renderer,
                                       texture,
                                       srcrect,
                                       left_width,
                                       right_width,
                                       top_height,
                                       bottom_height,
                                       scale,
                                       dstrect,
                                       tileScale);
}

bool RenderGeometry(SDL_Renderer*     renderer,
                    SDL_Texture*      texture,
                    const SDL_Vertex* vertices,
                    int               num_vertices,
                    const int*        indices,
                    int               num_indices)
{
    return SDL_RenderGeometry(
        renderer, texture, vertices, num_vertices, indices, num_indices);
}

bool RenderGeometryRaw(SDL_Renderer*     renderer,
                       SDL_Texture*      texture,
                       const float*      xy,
                       int               xy_stride,
                       const SDL_FColor* color,
                       int               color_stride,
                       const float*      uv,
                       int               uv_stride,
                       int               num_vertices,
                       const void*       indices,
                       int               num_indices,
                       int               size_indices)
{
    return SDL_RenderGeometryRaw(renderer,
                                 texture,
                                 xy,
                                 xy_stride,
                                 color,
                                 color_stride,
                                 uv,
                                 uv_stride,
                                 num_vertices,
                                 indices,
                                 num_indices,
                                 size_indices);
}

bool SetRenderTextureAddressMode(SDL_Renderer*      renderer,
                                 TextureAddressMode u_mode,
                                 TextureAddressMode v_mode)
{
    return SDL_SetRenderTextureAddressMode(renderer,
                                           (SDL_TextureAddressMode)(u_mode),
                                           (SDL_TextureAddressMode)(v_mode));
}

bool GetRenderTextureAddressMode(SDL_Renderer*       renderer,
                                 TextureAddressMode* u_mode,
                                 TextureAddressMode* v_mode)
{
    return SDL_GetRenderTextureAddressMode(renderer,
                                           (SDL_TextureAddressMode*)(u_mode),
                                           (SDL_TextureAddressMode*)(v_mode));
}

SDL_Surface* RenderReadPixels(SDL_Renderer* renderer, const SDL_Rect* rect)
{
    return SDL_RenderReadPixels(renderer, rect);
}

bool RenderPresent(SDL_Renderer* renderer)
{
    return SDL_RenderPresent(renderer);
}

void DestroyTexture(SDL_Texture* texture)
{
    SDL_DestroyTexture(texture);
}

void DestroyRenderer(SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
}

bool FlushRenderer(SDL_Renderer* renderer)
{
    return SDL_FlushRenderer(renderer);
}

void* GetRenderMetalLayer(SDL_Renderer* renderer)
{
    return SDL_GetRenderMetalLayer(renderer);
}

void* GetRenderMetalCommandEncoder(SDL_Renderer* renderer)
{
    return SDL_GetRenderMetalCommandEncoder(renderer);
}

bool AddVulkanRenderSemaphores(SDL_Renderer* renderer,
                               Uint32        wait_stage_mask,
                               Sint64        wait_semaphore,
                               Sint64        signal_semaphore)
{
    return SDL_AddVulkanRenderSemaphores(
        renderer, wait_stage_mask, wait_semaphore, signal_semaphore);
}

bool SetRenderVSync(SDL_Renderer* renderer, int vsync)
{
    return SDL_SetRenderVSync(renderer, vsync);
}

constexpr auto RENDERER_VSYNC_DISABLED()
{
    return 0;
}

constexpr auto RENDERER_VSYNC_ADAPTIVE()
{
    return (-1);
}

bool GetRenderVSync(SDL_Renderer* renderer, int* vsync)
{
    return SDL_GetRenderVSync(renderer, vsync);
}

constexpr auto DEBUG_TEXT_FONT_CHARACTER_SIZE()
{
    return 8;
}

bool RenderDebugText(SDL_Renderer* renderer, float x, float y, const char* str)
{
    return SDL_RenderDebugText(renderer, x, y, str);
}

bool SetDefaultTextureScaleMode(SDL_Renderer* renderer, ScaleMode scale_mode)
{
    return SDL_SetDefaultTextureScaleMode(renderer,
                                          (SDL_ScaleMode)(scale_mode));
}

bool GetDefaultTextureScaleMode(SDL_Renderer* renderer, ScaleMode* scale_mode)
{
    return SDL_GetDefaultTextureScaleMode(renderer,
                                          (SDL_ScaleMode*)(scale_mode));
}

using GPURenderStateCreateInfo = SDL_GPURenderStateCreateInfo;

using SDL_GPURenderState = SDL_GPURenderState;

SDL_GPURenderState* CreateGPURenderState(SDL_Renderer*                 renderer,
                                         SDL_GPURenderStateCreateInfo* desc)
{
    return SDL_CreateGPURenderState(renderer, desc);
}

bool SetGPURenderStateFragmentUniforms(SDL_GPURenderState* state,
                                       Uint32              slot_index,
                                       const void*         data,
                                       Uint32              length)
{
    return SDL_SetGPURenderStateFragmentUniforms(
        state, slot_index, data, length);
}

void DestroyGPURenderState(SDL_GPURenderState* state)
{
    SDL_DestroyGPURenderState(state);
}

using StorageInterface = SDL_StorageInterface;

using SDL_Storage = SDL_Storage;

SDL_Storage* OpenTitleStorage(const char* override, SDL_PropertiesID props)
{
    return SDL_OpenTitleStorage(override, props);
}

SDL_Storage* OpenUserStorage(const char*      org,
                             const char*      app,
                             SDL_PropertiesID props)
{
    return SDL_OpenUserStorage(org, app, props);
}

SDL_Storage* OpenFileStorage(const char* path)
{
    return SDL_OpenFileStorage(path);
}

SDL_Storage* OpenStorage(const SDL_StorageInterface* iface, void* userdata)
{
    return SDL_OpenStorage(iface, userdata);
}

bool CloseStorage(SDL_Storage* storage)
{
    return SDL_CloseStorage(storage);
}

bool StorageReady(SDL_Storage* storage)
{
    return SDL_StorageReady(storage);
}

bool GetStorageFileSize(SDL_Storage* storage, const char* path, Uint64* length)
{
    return SDL_GetStorageFileSize(storage, path, length);
}

bool ReadStorageFile(SDL_Storage* storage,
                     const char*  path,
                     void*        destination,
                     Uint64       length)
{
    return SDL_ReadStorageFile(storage, path, destination, length);
}

bool WriteStorageFile(SDL_Storage* storage,
                      const char*  path,
                      const void*  source,
                      Uint64       length)
{
    return SDL_WriteStorageFile(storage, path, source, length);
}

bool CreateStorageDirectory(SDL_Storage* storage, const char* path)
{
    return SDL_CreateStorageDirectory(storage, path);
}

bool EnumerateStorageDirectory(SDL_Storage*                   storage,
                               const char*                    path,
                               SDL_EnumerateDirectoryCallback callback,
                               void*                          userdata)
{
    return SDL_EnumerateStorageDirectory(storage, path, callback, userdata);
}

bool RemoveStoragePath(SDL_Storage* storage, const char* path)
{
    return SDL_RemoveStoragePath(storage, path);
}

bool RenameStoragePath(SDL_Storage* storage,
                       const char*  oldpath,
                       const char*  newpath)
{
    return SDL_RenameStoragePath(storage, oldpath, newpath);
}

bool CopyStorageFile(SDL_Storage* storage,
                     const char*  oldpath,
                     const char*  newpath)
{
    return SDL_CopyStorageFile(storage, oldpath, newpath);
}

bool GetStoragePathInfo(SDL_Storage*  storage,
                        const char*   path,
                        SDL_PathInfo* info)
{
    return SDL_GetStoragePathInfo(storage, path, info);
}

Uint64 GetStorageSpaceRemaining(SDL_Storage* storage)
{
    return SDL_GetStorageSpaceRemaining(storage);
}

#if defined(SDL_PLATFORM_WINDOWS)

using MSG = MSG;
#endif

#if defined(SDL_PLATFORM_WINDOWS)
#endif

#if defined(SDL_PLATFORM_WINDOWS)

void SetWindowsMessageHook(SDL_WindowsMessageHook callback, void* userdata)
{
    SDL_SetWindowsMessageHook(callback, userdata);
}
#endif

#if defined(SDL_PLATFORM_WIN32) || defined(SDL_PLATFORM_WINGDK)

int GetDirect3D9AdapterIndex(SDL_DisplayID displayID)
{
    return SDL_GetDirect3D9AdapterIndex(displayID);
}
#endif

#if defined(SDL_PLATFORM_WIN32) || defined(SDL_PLATFORM_WINGDK)

bool GetDXGIOutputInfo(SDL_DisplayID displayID,
                       int*          adapterIndex,
                       int*          outputIndex)
{
    return SDL_GetDXGIOutputInfo(displayID, adapterIndex, outputIndex);
}
#endif

void SetX11EventHook(SDL_X11EventHook callback, void* userdata)
{
    SDL_SetX11EventHook(callback, userdata);
}
#if defined(SDL_PLATFORM_LINUX)

bool SetLinuxThreadPriority(Sint64 threadID, int priority)
{
    return SDL_SetLinuxThreadPriority(threadID, priority);
}
#endif

#if defined(SDL_PLATFORM_LINUX)

bool SetLinuxThreadPriorityAndPolicy(Sint64 threadID,
                                     int    sdlPriority,
                                     int    schedPolicy)
{
    return SDL_SetLinuxThreadPriorityAndPolicy(
        threadID, sdlPriority, schedPolicy);
}
#endif

#if defined(SDL_PLATFORM_IOS)
#endif

#if defined(SDL_PLATFORM_IOS)

bool SetiOSAnimationCallback(SDL_Window*              window,
                             int                      interval,
                             SDL_iOSAnimationCallback callback,
                             void*                    callbackParam)
{
    return SDL_SetiOSAnimationCallback(
        window, interval, callback, callbackParam);
}
#endif

#if defined(SDL_PLATFORM_IOS)

void SetiOSEventPump(bool enabled)
{
    SDL_SetiOSEventPump(enabled);
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

void* GetAndroidJNIEnv(void)
{
    return SDL_GetAndroidJNIEnv();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

void* GetAndroidActivity(void)
{
    return SDL_GetAndroidActivity();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

int GetAndroidSDKVersion(void)
{
    return SDL_GetAndroidSDKVersion();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

bool IsChromebook(void)
{
    return SDL_IsChromebook();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

bool IsDeXMode(void)
{
    return SDL_IsDeXMode();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

void SendAndroidBackButton(void)
{
    SDL_SendAndroidBackButton();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

constexpr auto ANDROID_EXTERNAL_STORAGE_READ()
{
    return 0x01;
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

constexpr auto ANDROID_EXTERNAL_STORAGE_WRITE()
{
    return 0x02;
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

const char* GetAndroidInternalStoragePath(void)
{
    return SDL_GetAndroidInternalStoragePath();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

Uint32 GetAndroidExternalStorageState(void)
{
    return SDL_GetAndroidExternalStorageState();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

const char* GetAndroidExternalStoragePath(void)
{
    return SDL_GetAndroidExternalStoragePath();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

const char* GetAndroidCachePath(void)
{
    return SDL_GetAndroidCachePath();
}
#endif

#if defined(SDL_PLATFORM_ANDROID)
#endif

#if defined(SDL_PLATFORM_ANDROID)

bool RequestAndroidPermission(const char*                          permission,
                              SDL_RequestAndroidPermissionCallback cb,
                              void*                                userdata)
{
    return SDL_RequestAndroidPermission(permission, cb, userdata);
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

bool ShowAndroidToast(
    const char* message, int duration, int gravity, int xoffset, int yoffset)
{
    return SDL_ShowAndroidToast(message, duration, gravity, xoffset, yoffset);
}
#endif

#if defined(SDL_PLATFORM_ANDROID)

bool SendAndroidMessage(Uint32 command, int param)
{
    return SDL_SendAndroidMessage(command, param);
}
#endif

bool IsTablet(void)
{
    return SDL_IsTablet();
}

bool IsTV(void)
{
    return SDL_IsTV();
}

enum class Sandbox
{
    NONE              = SDL_SANDBOX_NONE,
    UNKNOWN_CONTAINER = SDL_SANDBOX_UNKNOWN_CONTAINER,
    FLATPAK           = SDL_SANDBOX_FLATPAK,
    SNAP              = SDL_SANDBOX_SNAP,
    MACOS             = SDL_SANDBOX_MACOS,
};
REGULAR_ENUM(Sandbox);

SDL_Sandbox GetSandbox(void)
{
    return SDL_GetSandbox();
}

void OnApplicationWillTerminate(void)
{
    SDL_OnApplicationWillTerminate();
}

void OnApplicationDidReceiveMemoryWarning(void)
{
    SDL_OnApplicationDidReceiveMemoryWarning();
}

void OnApplicationWillEnterBackground(void)
{
    SDL_OnApplicationWillEnterBackground();
}

void OnApplicationDidEnterBackground(void)
{
    SDL_OnApplicationDidEnterBackground();
}

void OnApplicationWillEnterForeground(void)
{
    SDL_OnApplicationWillEnterForeground();
}

void OnApplicationDidEnterForeground(void)
{
    SDL_OnApplicationDidEnterForeground();
}
#if defined(SDL_PLATFORM_IOS)

void OnApplicationDidChangeStatusBarOrientation(void)
{
    SDL_OnApplicationDidChangeStatusBarOrientation();
}
#endif

#if defined(SDL_PLATFORM_GDK)

using XTaskQueueHandle = XTaskQueueHandle;
#endif

#if defined(SDL_PLATFORM_GDK)

using XUserHandle = XUserHandle;
#endif

#if defined(SDL_PLATFORM_GDK)

bool GetGDKTaskQueue(XTaskQueueHandle* outTaskQueue)
{
    return SDL_GetGDKTaskQueue(outTaskQueue);
}
#endif

#if defined(SDL_PLATFORM_GDK)

bool GetGDKDefaultUser(XUserHandle* outUserHandle)
{
    return SDL_GetGDKDefaultUser(outUserHandle);
}
#endif

using DateTime = SDL_DateTime;

enum class DateFormat
{
    FORMAT_YYYYMMDD = SDL_DATE_FORMAT_YYYYMMDD,
    FORMAT_DDMMYYYY = SDL_DATE_FORMAT_DDMMYYYY,
    FORMAT_MMDDYYYY = SDL_DATE_FORMAT_MMDDYYYY,
};
REGULAR_ENUM(DateFormat);

enum class TimeFormat
{
    FORMAT_24HR = SDL_TIME_FORMAT_24HR,
    FORMAT_12HR = SDL_TIME_FORMAT_12HR,
};
REGULAR_ENUM(TimeFormat);

bool GetDateTimeLocalePreferences(DateFormat* dateFormat,
                                  TimeFormat* timeFormat)
{
    return SDL_GetDateTimeLocalePreferences((SDL_DateFormat*)(dateFormat),
                                            (SDL_TimeFormat*)(timeFormat));
}

bool GetCurrentTime(SDL_Time* ticks)
{
    return SDL_GetCurrentTime(ticks);
}

bool TimeToDateTime(SDL_Time ticks, SDL_DateTime* dt, bool localTime)
{
    return SDL_TimeToDateTime(ticks, dt, localTime);
}

bool DateTimeToTime(const SDL_DateTime* dt, SDL_Time* ticks)
{
    return SDL_DateTimeToTime(dt, ticks);
}

void TimeToWindows(SDL_Time ticks,
                   Uint32*  dwLowDateTime,
                   Uint32*  dwHighDateTime)
{
    SDL_TimeToWindows(ticks, dwLowDateTime, dwHighDateTime);
}

SDL_Time TimeFromWindows(Uint32 dwLowDateTime, Uint32 dwHighDateTime)
{
    return SDL_TimeFromWindows(dwLowDateTime, dwHighDateTime);
}

int GetDaysInMonth(int year, int month)
{
    return SDL_GetDaysInMonth(year, month);
}

int GetDayOfYear(int year, int month, int day)
{
    return SDL_GetDayOfYear(year, month, day);
}

int GetDayOfWeek(int year, int month, int day)
{
    return SDL_GetDayOfWeek(year, month, day);
}

constexpr auto MS_PER_SECOND()
{
    return 1000;
}

constexpr auto US_PER_SECOND()
{
    return 1000000;
}

constexpr auto NS_PER_SECOND()
{
    return 1000000000LL;
}

constexpr auto NS_PER_MS()
{
    return 1000000;
}

constexpr auto NS_PER_US()
{
    return 1000;
}

Uint64 GetTicks(void)
{
    return SDL_GetTicks();
}

Uint64 GetTicksNS(void)
{
    return SDL_GetTicksNS();
}

Uint64 GetPerformanceCounter(void)
{
    return SDL_GetPerformanceCounter();
}

Uint64 GetPerformanceFrequency(void)
{
    return SDL_GetPerformanceFrequency();
}

void Delay(Uint32 ms)
{
    SDL_Delay(ms);
}

void DelayNS(Uint64 ns)
{
    SDL_DelayNS(ns);
}

void DelayPrecise(Uint64 ns)
{
    SDL_DelayPrecise(ns);
}

using TimerID = Uint32;

SDL_TimerID AddTimer(Uint32            interval,
                     SDL_TimerCallback callback,
                     void*             userdata)
{
    return SDL_AddTimer(interval, callback, userdata);
}

SDL_TimerID AddTimerNS(Uint64              interval,
                       SDL_NSTimerCallback callback,
                       void*               userdata)
{
    return SDL_AddTimerNS(interval, callback, userdata);
}

bool RemoveTimer(SDL_TimerID id)
{
    return SDL_RemoveTimer(id);
}

using SDL_Tray = SDL_Tray;

using SDL_TrayMenu = SDL_TrayMenu;

using SDL_TrayEntry = SDL_TrayEntry;

enum class TrayEntryFlags : Uint32
{
    BUTTON   = SDL_TRAYENTRY_BUTTON,
    CHECKBOX = SDL_TRAYENTRY_CHECKBOX,
    SUBMENU  = SDL_TRAYENTRY_SUBMENU,
    DISABLED = SDL_TRAYENTRY_DISABLED,
    CHECKED  = SDL_TRAYENTRY_CHECKED,
};
BITFLAG_ENUM(TrayEntryFlags);

SDL_Tray* CreateTray(SDL_Surface* icon, const char* tooltip)
{
    return SDL_CreateTray(icon, tooltip);
}

void SetTrayIcon(SDL_Tray* tray, SDL_Surface* icon)
{
    SDL_SetTrayIcon(tray, icon);
}

void SetTrayTooltip(SDL_Tray* tray, const char* tooltip)
{
    SDL_SetTrayTooltip(tray, tooltip);
}

SDL_TrayMenu* CreateTrayMenu(SDL_Tray* tray)
{
    return SDL_CreateTrayMenu(tray);
}

SDL_TrayMenu* CreateTraySubmenu(SDL_TrayEntry* entry)
{
    return SDL_CreateTraySubmenu(entry);
}

SDL_TrayMenu* GetTrayMenu(SDL_Tray* tray)
{
    return SDL_GetTrayMenu(tray);
}

SDL_TrayMenu* GetTraySubmenu(SDL_TrayEntry* entry)
{
    return SDL_GetTraySubmenu(entry);
}

void RemoveTrayEntry(SDL_TrayEntry* entry)
{
    SDL_RemoveTrayEntry(entry);
}

SDL_TrayEntry* InsertTrayEntryAt(SDL_TrayMenu*  menu,
                                 int            pos,
                                 const char*    label,
                                 TrayEntryFlags flags)
{
    return SDL_InsertTrayEntryAt(menu, pos, label, (SDL_TrayEntryFlags)(flags));
}

void SetTrayEntryLabel(SDL_TrayEntry* entry, const char* label)
{
    SDL_SetTrayEntryLabel(entry, label);
}

const char* GetTrayEntryLabel(SDL_TrayEntry* entry)
{
    return SDL_GetTrayEntryLabel(entry);
}

void SetTrayEntryChecked(SDL_TrayEntry* entry, bool checked)
{
    SDL_SetTrayEntryChecked(entry, checked);
}

bool GetTrayEntryChecked(SDL_TrayEntry* entry)
{
    return SDL_GetTrayEntryChecked(entry);
}

void SetTrayEntryEnabled(SDL_TrayEntry* entry, bool enabled)
{
    SDL_SetTrayEntryEnabled(entry, enabled);
}

bool GetTrayEntryEnabled(SDL_TrayEntry* entry)
{
    return SDL_GetTrayEntryEnabled(entry);
}

void SetTrayEntryCallback(SDL_TrayEntry*   entry,
                          SDL_TrayCallback callback,
                          void*            userdata)
{
    SDL_SetTrayEntryCallback(entry, callback, userdata);
}

void ClickTrayEntry(SDL_TrayEntry* entry)
{
    SDL_ClickTrayEntry(entry);
}

void DestroyTray(SDL_Tray* tray)
{
    SDL_DestroyTray(tray);
}

SDL_TrayMenu* GetTrayEntryParent(SDL_TrayEntry* entry)
{
    return SDL_GetTrayEntryParent(entry);
}

SDL_TrayEntry* GetTrayMenuParentEntry(SDL_TrayMenu* menu)
{
    return SDL_GetTrayMenuParentEntry(menu);
}

SDL_Tray* GetTrayMenuParentTray(SDL_TrayMenu* menu)
{
    return SDL_GetTrayMenuParentTray(menu);
}

void UpdateTrays(void)
{
    SDL_UpdateTrays();
}
} // namespace sdl
