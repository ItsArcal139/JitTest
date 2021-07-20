#include <iostream>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #ifdef __APPLE__
        #include <sandbox.h>
    #endif
#endif

#include "Text.h"
#include "Logger.h"

std::string loggerName = "Main";

typedef unsigned char byte;

union callable {
    byte* pointer;
    void (*run)(void);
};

template <typename T>
inline std::string to_hex(T n) {
    std::ostringstream stream;
    stream << std::hex << n;
    return "0x" + stream.str();
}

byte* allocate_exec(size_t size, byte* p) {
#ifdef _WIN32
    return (byte*) VirtualAllocEx(GetCurrentProcess(), 0, 1 << 16, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
    Logger::log("AllocateExec-Unix", LiteralText::of("Allocating executable memory on UNIX platform..."));
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(ptr == (void*)-1) {
        perror("mmap");
        Logger::error("AllocateExec-Unix",
                    TranslateText::of("Failed to allocate executable memory. Error code: %s")
                            ->addWith(LiteralText::of(to_hex(errno))->setColor(TextColor::Gold))
                    );
        return nullptr;
    }

    byte* target = (byte*)ptr;
    for(int i=0; i<size; i++) {
        *(target++) = *(p++);
    }

    int rv = mprotect(ptr, size, PROT_READ | PROT_EXEC);
    if (rv != 0) {
        perror("mprotect");
        Logger::error("AllocateExec-Unix",
                      TranslateText::of("Failed to allocate executable memory. Error code: %s")
                      ->addWith(LiteralText::of(to_hex(errno))->setColor(TextColor::Gold))
                      );
        return nullptr;
    }
    return (byte*)ptr;
#endif
}

void signalHandler(int signum) {
    std::string sigName = "unknown";
    switch(signum) {
        case SIGILL:
            sigName = "SIGILL";
            Logger::error("SignalHandler", LiteralText::of("SIGILL caught. Make sure the instructions are valid on your CPU architecture."));
            break;
    }
    Logger::error("SignalHandler", LiteralText::of("Signal caught: " + to_hex(signum) + " (" + sigName + ")"));
    Logger::error("SignalHandler", LiteralText::of("The program will now exit."));
    exit(1);
}

typedef uint64_t (*Function)(uint64_t);

uint64_t test(uint64_t d) {
    return d + 4;
}

int main() {
    Logger::info(loggerName, LiteralText::of("Hello, world!"));
    Logger::info(loggerName, LiteralText::of("This is a pure C++ playground!!"));
    Logger::info(loggerName, LiteralText::of("-----------------"));
    Logger::info(loggerName, LiteralText::of("Allocating memory..."));

#ifdef __ARM_ARCH
    Logger::log(loggerName, LiteralText::of("Loading ARM instructions..."));
#else
    Logger::log(loggerName, LiteralText::of("Loading x86-64 instructions..."));
#endif

    byte code[] = {
#ifdef __ARM_ARCH
        // sub sp, sp, #16
        0xff, 0x43, 0x00, 0xd1,
        // str x0, [sp, #8]
        0xe0, 0x07, 0x00, 0xf9,
        // ldr x8, [sp, 1]
        0xe8, 0x07, 0x40, 0xf9,
        // add x0, x8, 4
        0x00, 0x11, 0x00, 0x91,
        // add sp, sp, 16
        0xff, 0x43, 0x00, 0x91,
        // ret lr
        0xc0, 0x03, 0x5f, 0xd6,
#else
        // movq %rdi, %rax
        0x48, 0x89, 0xf8,
        // addq $4, %rax
        0x48, 0x83, 0xc0, 0x04,
        // ret
        0xc3,
#endif
    };
    byte* p = allocate_exec(1024, code);

    if(p == nullptr) {
        Logger::error(loggerName, LiteralText::of("Failed to allocate memory. The program will now exit..."));
        return 0;
    }

    Logger::info(loggerName, TranslateText::of("Allocated executable memory at %s")
                 ->addWith(LiteralText::of(to_hex((long)p))->setColor(TextColor::Gold)));

#ifdef __APPLE__
    Logger::info(loggerName + "-macOS", LiteralText::of("Enabling sandbox..."));
    char *err;
    int rv = sandbox_init(R"(
(version 1)
(deny default)
(deny dynamic-code-generation)
)", 0, &err);
    if(rv) {
        Logger::error(loggerName + "-macOS", LiteralText::of("Failed to initiate sandbox."));
        return 0;
    }
#endif

    signal(SIGILL, signalHandler);
    auto func = (Function)p;

    srand(time(nullptr));
    uint64_t payload = rand();
    uint64_t result = func(payload);

    Logger::info(loggerName, LiteralText::of("JIT function returns " + std::to_string(payload) + " + 4 = " + std::to_string(result)));

    return 0;
}
