#ifndef TEST_STATE_MOCKS_STATE_H
#define TEST_STATE_MOCKS_STATE_H

#include "gmock/gmock.h"
#include "state/actor/soldier.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_state.h"
#include "state/map/interfaces/i_map.h"
#include <vector>

using namespace std;
using namespace state;
using namespace physics;

class StateMock : public IState {
  public:
	MOCK_METHOD0(GetAllSoldiers, const vector<vector<Soldier *>>());
	MOCK_METHOD0(GetAllTowers, const vector<vector<Tower *>>());
	MOCK_METHOD0(GetMoney, vector<int64_t>());
	MOCK_METHOD0(GetMap, IMap *());
	MOCK_METHOD3(MoveSoldier, void(PlayerId, int64_t, Vector));
	MOCK_METHOD3(AttackActor, void(PlayerId, int64_t, int64_t));
	MOCK_METHOD3(BuildTower, void(PlayerId, int64_t, Vector));
	MOCK_METHOD2(UpgradeTower, void(PlayerId, int64_t));
	MOCK_METHOD2(SuicideTower, void(PlayerId, int64_t));
};

#endif
