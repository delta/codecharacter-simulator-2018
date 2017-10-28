#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "state/player_state.h"

using namespace drivers;

// Takes one arg, the name of shm
// Returns 0 if shm has expected values, 1 otherwise
int main(int argc, char *argv[]) {
	if (argc == 1)
		return 0;

	SharedMemoryPlayer shm((std::string(argv[1])));
	SharedBuffer *buf = shm.GetBuffer();

	if (buf->is_player_running && buf->instruction_counter == 1)
		return 0;
	else
		return 1;
}
