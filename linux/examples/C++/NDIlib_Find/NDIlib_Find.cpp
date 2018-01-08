#include <csignal>
#include <cstddef>
#include <cstdio>
#include <atomic>
#include <chrono>

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
	
	// We are going to create an NDI finder that locates sources on the network.
	// including ones that are available on this machine itself. It will use the default
	// groups assigned for the current machine.
	NDIlib_find_create_t NDI_find_create_desc; /* Use defaults */
	NDIlib_find_instance_t pNDI_find = NDIlib_find_create_v2(&NDI_find_create_desc);
	if (!pNDI_find) return 0;

	// Run for one minute
	const auto start = std::chrono::high_resolution_clock::now();
	while (!exit_loop && std::chrono::high_resolution_clock::now() - start < std::chrono::minutes(1))
	{	// Wait up till 5 seconds to check for new sources to be added or removed
		if (!NDIlib_find_wait_for_sources(pNDI_find, 5000))
		{	// No new sources added !
			printf("No change to the sources found.\n");
		}
		else
		{	// Get the updated list of sources
			uint32_t no_sources = 0;
			const NDIlib_source_t* p_sources = NDIlib_find_get_current_sources(pNDI_find, &no_sources);
				
			// Display all the sources.
			printf("Network sources (%u found).\n", no_sources);
			for (uint32_t i = 0; i < no_sources; i++)
				printf("%u. %s\n", i + 1, p_sources[i].p_ndi_name);
		}
	}

	// Destroy the NDI finder
	NDIlib_find_destroy(pNDI_find);

	// Success. We are done
	return 0;
}
