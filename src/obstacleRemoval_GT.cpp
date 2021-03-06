#include "obstacleRemoval.h"

int ObstacleRemovalModel::solveConflict_yielding_heterogeneous(arrayNi &agentsInConflict) {
	if (agentsInConflict.size() == 1)
		return agentsInConflict[0];

	arrayNi evacuees, volunteers;
	for (const auto &i : agentsInConflict) {
		if (mAgentManager.mPool[i].mInChargeOf == STATE_NULL)
			evacuees.push_back(i);
		else
			volunteers.push_back(i);
	}
	if (evacuees.size() == 0 || volunteers.size() == 0)
		return solveConflict_yielding_homogeneous(agentsInConflict);

	arrayNf realPayoff_e(evacuees.size(), 0.f), realPayoff_v(volunteers.size(), 0.f);
	arrayNf virtualPayoff_e(evacuees.size(), 0.f), virtualPayoff_v(volunteers.size(), 0.f);
	for (size_t i = 0; i < evacuees.size(); i++) {
		for (size_t j = 0; j < volunteers.size(); j++) {
			if (mAgentManager.mPool[evacuees[i]].mStrategy[0] && mAgentManager.mPool[volunteers[j]].mStrategy[0]) {
				realPayoff_e[i] += 1.f + mMu;
				realPayoff_v[j] += 1.f - mOc;
				virtualPayoff_e[i] += 1.f;
				virtualPayoff_v[j] += 1.f;
			}
			else if (mAgentManager.mPool[evacuees[i]].mStrategy[0] && !mAgentManager.mPool[volunteers[j]].mStrategy[0]) {
				realPayoff_e[i] += mMu;
				realPayoff_v[j] += 1.f;
				virtualPayoff_v[j] += 1.f - mOc;
			}
			else if (!mAgentManager.mPool[evacuees[i]].mStrategy[0] && mAgentManager.mPool[volunteers[j]].mStrategy[0]) {
				realPayoff_e[i] += 1.f;
				realPayoff_v[j] += -mOc;
				virtualPayoff_e[i] += 1.f + mMu;
			}
			else {
				virtualPayoff_e[i] += mMu;
				virtualPayoff_v[j] += -mOc;
			}
		}
		realPayoff_e[i] /= volunteers.size();
		virtualPayoff_e[i] /= volunteers.size();
	}
	std::for_each(realPayoff_v.begin(), realPayoff_v.end(), [&](float &i) { i /= evacuees.size(); });
	std::for_each(virtualPayoff_v.begin(), virtualPayoff_v.end(), [&](float &i) { i /= evacuees.size(); });

	adjustAgentStates(evacuees, realPayoff_e, virtualPayoff_e, GAME_YIELDING_HETERO);
	adjustAgentStates(volunteers, realPayoff_v, virtualPayoff_v, GAME_YIELDING_HETERO);

	std::sort(evacuees.begin(), evacuees.end(),
		[&](int i, int j) { return mAgentManager.mPool[i].mStrategy[0] < mAgentManager.mPool[j].mStrategy[0]; });
	std::sort(volunteers.begin(), volunteers.end(),
		[&](int i, int j) { return mAgentManager.mPool[i].mStrategy[0] < mAgentManager.mPool[j].mStrategy[0]; });

	int numEvacueesNotYield = std::count_if(evacuees.begin(), evacuees.end(),
		[&](int i) { return !mAgentManager.mPool[i].mStrategy[0]; });
	int numVolunteersNotYield = std::count_if(volunteers.begin(), volunteers.end(),
		[&](int i) { return !mAgentManager.mPool[i].mStrategy[0]; });

	if (numEvacueesNotYield == 0)
		return numVolunteersNotYield == 0
			? agentsInConflict[(int)(mDistribution(mRNG_GT) * agentsInConflict.size())]
			: volunteers[(int)(mDistribution(mRNG_GT) * numVolunteersNotYield)];
	if (numVolunteersNotYield == 0)
		return evacuees[(int)(mDistribution(mRNG_GT) * numEvacueesNotYield)];
	return STATE_NULL;
}

int ObstacleRemovalModel::solveConflict_yielding_homogeneous(arrayNi &agentsInConflict) {
	std::sort(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i, int j) { return mAgentManager.mPool[i].mStrategy[1] < mAgentManager.mPool[j].mStrategy[1]; });
	int numAgentsNotYield = std::count_if(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i) { return !mAgentManager.mPool[i].mStrategy[1]; });

	arrayNf realPayoff(agentsInConflict.size(), 0.f);
	arrayNf virtualPayoff(agentsInConflict.size(), 0.f);
	switch (numAgentsNotYield) {
	case 0:
		std::fill(realPayoff.begin(), realPayoff.end(), 1.f / agentsInConflict.size());
		std::fill(virtualPayoff.begin(), virtualPayoff.end(), 1.f - mCc);
		break;
	case 1:
		realPayoff[0] = 1.f - mCc;
		virtualPayoff[0] = 1.f / agentsInConflict.size();
		std::fill(virtualPayoff.begin() + 1, virtualPayoff.end(), -mCc);
		break;
	default:
		std::fill(realPayoff.begin(), realPayoff.begin() + numAgentsNotYield, -mCc);
		std::fill(virtualPayoff.begin() + numAgentsNotYield, virtualPayoff.end(), -mCc);
	}

	adjustAgentStates(agentsInConflict, realPayoff, virtualPayoff, GAME_YIELDING_HOMO);

	std::sort(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i, int j) { return mAgentManager.mPool[i].mStrategy[1] < mAgentManager.mPool[j].mStrategy[1]; });
	numAgentsNotYield = std::count_if(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i) { return !mAgentManager.mPool[i].mStrategy[1]; });

	switch (numAgentsNotYield) {
	case 0:
		return agentsInConflict[(int)(mDistribution(mRNG_GT) * agentsInConflict.size())];
	case 1:
		return agentsInConflict[0];
	default:
		return STATE_NULL;
	}
}

void ObstacleRemovalModel::solveConflict_volunteering(arrayNi &agentsInConflict) {
	std::sort(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i, int j) { return mAgentManager.mPool[i].mStrategy[2] > mAgentManager.mPool[j].mStrategy[2]; });
	int numAgentsRemove = std::count_if(agentsInConflict.begin(), agentsInConflict.end(),
		[&](int i) { return mAgentManager.mPool[i].mStrategy[2]; });

	arrayNf realPayoff(agentsInConflict.size(), 0.f);
	arrayNf virtualPayoff(agentsInConflict.size(), 0.f);
	switch (numAgentsRemove) {
	case 0:
		std::fill(virtualPayoff.begin(), virtualPayoff.end(), 1.f - mRc);
		break;
	case 1:
		realPayoff[0] = 1.f - mRc;
		virtualPayoff[0] = 0.f;
		std::fill(realPayoff.begin() + 1, realPayoff.end(), 1.f);
		std::fill(virtualPayoff.begin() + 1, virtualPayoff.end(), 1.f - mRc / 2);
		break;
	default:
		std::fill(realPayoff.begin(), realPayoff.begin() + numAgentsRemove, 1.f - mRc / numAgentsRemove);
		std::fill(realPayoff.begin() + numAgentsRemove, realPayoff.end(), 1.f);
		std::fill(virtualPayoff.begin(), virtualPayoff.begin() + numAgentsRemove, 1.f);
		std::fill(virtualPayoff.begin() + numAgentsRemove, virtualPayoff.end(), 1.f - mRc / (numAgentsRemove + 1));
	}

	adjustAgentStates(agentsInConflict, realPayoff, virtualPayoff, GAME_VOLUNTEERING);
}

void ObstacleRemovalModel::adjustAgentStates(const arrayNi &agentsInConflict, const arrayNf &curRealPayoff, const arrayNf &curVirtualPayoff, int type) {
	arrayNb tmpStrategy(agentsInConflict.size());
	for (size_t i = 0; i < agentsInConflict.size(); i++) {
		Agent &agent = mAgentManager.mPool[agentsInConflict[i]];
		agent.mPayoff[type][agent.mStrategy[type]] += curRealPayoff[i];
		agent.mPayoff[type][!agent.mStrategy[type]] += curVirtualPayoff[i];
		agent.mNumGames[type]++;
		tmpStrategy[i] = agent.mStrategy[type];

		// follow another player's strategy
		if (mDistribution(mRNG_GT) < mHerdingCoefficient) {
			if (agentsInConflict.size() > 1) {
				size_t j = (size_t)(mDistribution(mRNG_GT) * (agentsInConflict.size() - 1));
				if (j >= i)
					j++;
				if (mDistribution(mRNG_GT) < calcTransProb(curRealPayoff[j], curRealPayoff[i]))
					tmpStrategy[i] = mAgentManager.mPool[agentsInConflict[j]].mStrategy[type];
			}
		}
		// use self-judgement
		else {
			float Ex = agent.mPayoff[type][agent.mStrategy[type]] / agent.mNumGames[type];
			float Ey = agent.mPayoff[type][!agent.mStrategy[type]] / agent.mNumGames[type];
			if (mDistribution(mRNG_GT) < calcTransProb(Ey, Ex))
				tmpStrategy[i] = !agent.mStrategy[type];
		}
	}

	for (size_t i = 0; i < agentsInConflict.size(); i++)
		mAgentManager.mPool[agentsInConflict[i]].mStrategy[type] = tmpStrategy[i];
}