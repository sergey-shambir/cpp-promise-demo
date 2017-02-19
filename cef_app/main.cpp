#include "simple_app.h"
#include "include/base/cef_logging.h"
#include "cef_utils.h"

#if defined(__linux__)
#include <X11/Xlib.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace
{
#if defined(__linux__)
int XErrorHandlerImpl(Display *display, XErrorEvent *event)
{
    LOG(WARNING)
        << "X error received: "
        << "type " << event->type << ", "
        << "serial " << event->serial << ", "
        << "error_code " << static_cast<int>(event->error_code) << ", "
        << "request_code " << static_cast<int>(event->request_code) << ", "
        << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display *display)
{
    return 0;
}
#endif
} // namespace

int RunApplication(const CefMainArgs &args)
{

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line and,
    // if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(args, nullptr, nullptr);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }

#if defined(__linux__)
    // Install xlib error handlers so that the application won't be terminated
    // on non-fatal errors.
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

    // Specify CEF global settings here.
    CefSettings settings;
    settings.no_sandbox = true;

    // SimpleApp implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // CEF has initialized.
    auto app = MakeCefRef<SimpleApp>();

    // Initialize CEF for the browser process.
    CefInitialize(args, settings, app.get(), nullptr);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}

#if defined(__linux__)

// Entry point function for all processes.
int main(int argc, char* argv[])
{
    // Provide CEF with command-line arguments.
    CefMainArgs mainArgs(argc, argv);
    return RunApplication(mainArgs);
}

#endif

#if defined(_WIN32)

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Enable High-DPI support on Windows 7 or newer.
    CefEnableHighDPISupport();

    // Provide CEF with command-line arguments.
    CefMainArgs mainArgs(hInstance);
    return RunApplication(mainArgs);
}

#endif
