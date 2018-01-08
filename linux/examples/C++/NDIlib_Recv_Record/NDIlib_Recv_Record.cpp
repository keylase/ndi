#include <csignal>
#include <cstddef>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <string>

#ifdef _WIN32
#include <windows.h>

#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64

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
	
	// Create a finder
	const NDIlib_find_create_t NDI_find_create_desc; /* Use defaults */
	NDIlib_find_instance_t pNDI_find = NDIlib_find_create_v2(&NDI_find_create_desc);
	if (!pNDI_find) return 0;

	// We wait until there is at least one source on the network
	uint32_t no_sources = 0;
	const NDIlib_source_t* p_sources = NULL;
	while (!exit_loop && !no_sources)
	{	// Wait until the sources on the nwtork have changed
		NDIlib_find_wait_for_sources(pNDI_find, 1000);
		p_sources = NDIlib_find_get_current_sources(pNDI_find, &no_sources);
	}

	// We need at least one source
	if (!p_sources) return 0;

	// We now have at least one source, so we create a receiver to look at it.
	// We tell it that we prefer YCbCr video since it is more efficient for us. If the source has an alpha channel
	// it will still be provided in BGRA
	NDIlib_recv_create_v3_t NDI_recv_create_desc;
	NDI_recv_create_desc.source_to_connect_to = p_sources[0];
	NDI_recv_create_desc.p_ndi_name = "Example Record Receiver";
	NDIlib_recv_instance_t pNDI_recv = NDIlib_recv_create_v3(&NDI_recv_create_desc);
	if (!pNDI_recv) return 0;

	// Destroy the NDI finder. We needed to have access to the pointers to p_sources[0]
	NDIlib_find_destroy(pNDI_find);	

	// The current Web URL for this source
	bool is_recording = false;

	// Run for one minute
	const auto start = std::chrono::high_resolution_clock::now();
	while (!exit_loop && std::chrono::high_resolution_clock::now() - start < std::chrono::seconds(30))
	{	// Receive something
		switch (NDIlib_recv_capture_v2(pNDI_recv, NULL, NULL, NULL, is_recording ? 250 : 1000))
		{	// There is a status change on the receiver (e.g. new web interface)
			case NDIlib_frame_type_status_change:
			{	// Get the Web UR
				if ((!is_recording) && (NDIlib_recv_recording_is_supported(pNDI_recv)))
				{	// Display the details
					printf("This source supports recording !\n");

					// Start recording
					if (NDIlib_recv_recording_start(pNDI_recv, "Record Example"))
					{	// We are now recording
						is_recording = true;
						printf("Starting recording !\n");
					}
				}

			}	break;

			// Everything else
			default:
				break;
		}

		if (is_recording)
		{	// Get the record time
			NDIlib_recv_recording_time_t record_time;
			if (NDIlib_recv_recording_get_times(pNDI_recv, &record_time))
			{	// Display the details
				printf("Recorded %d frames, %f seconds.\n", (int)record_time.no_frames, (float)(record_time.last_time - record_time.start_time)/100000000.0f);
			}
		}
	}

	// Stop recording
	if (is_recording)
	{	// Stop the recording please 
		printf("Stop recording now !\n");
		NDIlib_recv_recording_stop(pNDI_recv);

		// Get the filename (you could get it even while recording if you want).
		const char* p_filename = NDIlib_recv_recording_get_filename(pNDI_recv);
		if (p_filename)
		{	// Display the filename
			printf("The file is stored at : %s\n", p_filename);

			// Show it on windows
#ifdef _WIN32
			// Get the item in the foledr
			std::string args = "/n,/select,\"";
			args += p_filename;
			args += "\"";

			// Open explorer with this file selected.
			ShellExecuteA(NULL, "open", "explorer.exe", args.c_str(), 0, SW_NORMAL);
#endif // _WINre

			// Free the filename
			NDIlib_recv_free_string(pNDI_recv, p_filename);
		}
	}

	// Destroy the receiver
	NDIlib_recv_destroy(pNDI_recv);

	// Not required, but nice
	NDIlib_destroy();

	// Finished
	return 0;
}

