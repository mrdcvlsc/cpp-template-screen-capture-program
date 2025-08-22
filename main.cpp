#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <set>
#include <mutex>
#include <ScreenCapture.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb/stb_image_write.h"

int main() {
    std::atomic<bool> captured{false};
    std::set<int> captured_monitors;
    std::mutex monitor_mutex;

    auto framgrabber = SL::Screen_Capture::CreateCaptureConfiguration([]() {
        auto monitors = SL::Screen_Capture::GetMonitors();
        std::cout << "Found " << monitors.size() << " monitors\n";
        return monitors; // return all monitors
    })
    ->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
        std::lock_guard<std::mutex> lock(monitor_mutex);
        
        // check if we've already captured from this monitor
        if (captured_monitors.find(monitor.Index) != captured_monitors.end()) {
            return; // skip monitor if already captured
        }
        
        std::cout << "Capturing from monitor: " << monitor.Name << " (Index: " << monitor.Index << ")\n";
        
        int width = SL::Screen_Capture::Width(img);
        int height = SL::Screen_Capture::Height(img);
        size_t dataSize = width * height * sizeof(SL::Screen_Capture::ImageBGRA);
        std::vector<unsigned char> buffer(dataSize);
        SL::Screen_Capture::Extract(img, buffer.data(), buffer.size());

        // convert BGRA to RGBA for stb_image_write
        std::vector<unsigned char> rgba_buffer(width * height * 4);
        for (int i = 0; i < width * height; ++i) {
            rgba_buffer[i * 4 + 0] = buffer[i * 4 + 2]; // R
            rgba_buffer[i * 4 + 1] = buffer[i * 4 + 1]; // G
            rgba_buffer[i * 4 + 2] = buffer[i * 4 + 0]; // B
            rgba_buffer[i * 4 + 3] = buffer[i * 4 + 3]; // A
        }

        // get current time for filename
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &now_c);
#else
        localtime_r(&now_c, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S");
        std::string filename = oss.str() + "_monitor_" + std::to_string(monitor.Index) + ".png";

        if (stbi_write_png(filename.c_str(), width, height, 4, rgba_buffer.data(), width * 4)) {
            std::cout << "Screenshot saved as " << filename << " (" << width << "x" << height << ")\n";
        } else {
            std::cerr << "Failed to save screenshot as PNG!\n";
        }
        
        // mark this monitor as captured
        captured_monitors.insert(monitor.Index);
        
        // check if we've captured from all monitors
        auto all_monitors = SL::Screen_Capture::GetMonitors();
        if (captured_monitors.size() >= all_monitors.size()) {
            captured = true;
            std::cout << "All monitors captured. Exiting...\n";
        }
    })
    ->start_capturing();

    // wait until screenshots from all monitors are captured
    while (!captured) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    // give some time for the library to clean up
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
