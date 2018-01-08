#include <csignal>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <chrono>

#ifdef _WIN32

#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64

#include <windows.h>

#endif

#include <Processing.NDI.Lib.h>

static std::atomic<bool> exit_loop(false);
static void sigint_handler(int)
{	exit_loop = true;
}

int main(int argc, char* argv[])
{	// Not required, but "correct" (see the SDK documentation.
	if (!NDIlib_initialize())
	{	// Cannot run NDI. Most likely because the CPU is not sufficient (see SDK documentation).
		// you can check this directly with a call to NDIlib_is_supported_CPU()
		printf("Cannot run NDI.");
		return 0;
	}

	// Catch interrupt so that we can shut down gracefully
	signal(SIGINT, sigint_handler);
	
	// Create an NDI source that is called "My Video" and is clocked to the video.
	NDIlib_send_create_t NDI_send_create_desc;
	NDI_send_create_desc.p_ndi_name = "My Video";

	// We create the NDI sender
	NDIlib_send_instance_t pNDI_send = NDIlib_send_create(&NDI_send_create_desc);
	if (!pNDI_send) return 0;

	// The resolution to work with
	const int xres = 720;
	const int yres = 480;

	// We are going to create a 1920x1080 interlaced frame at 29.97Hz.
	NDIlib_video_frame_v2_t NDI_video_frame;
	NDI_video_frame.xres = xres;
	NDI_video_frame.yres = yres;
	NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRX;
	NDI_video_frame.frame_format_type = NDIlib_frame_format_type_interleaved;
	NDI_video_frame.p_data = (uint8_t*)malloc(xres * yres * 4);
	NDI_video_frame.line_stride_in_bytes = xres * 4;

	// We will send 1000 frames of video. 
	while (!exit_loop)
	{	// Get the current time
		const auto start_time = std::chrono::high_resolution_clock::now();

		// Send 200 frames
		for (int idx = 0; !exit_loop && idx < 200; idx++)
		{	// Fill in the buffer. It is likely that you would do something much smarter than this.
			memset((void*)NDI_video_frame.p_data, (idx & 1) ? 255 : 0, xres * yres * 4);

			// We now submit the frame. Note that this call will be clocked so that we end up submitting at exactly 29.97fps.
			NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);
		}

		// Get the end time
		const auto end_time = std::chrono::high_resolution_clock::now();

		// Just display something helpful
		printf("200 frames sent, average fps=%1.2f\n", 200.0f / std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time).count());
	}

	// Free the video frame
	free(NDI_video_frame.p_data);

	// Destroy the NDI sender
	NDIlib_send_destroy(pNDI_send);

	// Not required, but nice
	NDIlib_destroy();

	// Success
	return 0;
}

