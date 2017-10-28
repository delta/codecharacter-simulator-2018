#include "player/player_code_export.h"
#include "player_wrapper/interfaces/i_player_code.h"
#include "state/player_state.h"

namespace player {

/**
 * Class where player defines AI code
 */
class PLAYER_CODE_EXPORT PlayerCode0 : public IPlayerCode {
  private:
	state::PlayerState *player_state;

  public:
	PlayerCode0(state::PlayerState *player_state);

	/**
	 * Player AI update function (main logic of the AI)
	 */
	void Update() override;
};
}
