#pragma once
struct animation;
class CResolver
{
private:
	float GetLeftYaw(IBasePlayer*);
	float GetRightYaw(IBasePlayer*);
	void CResolver::DetectSide(INetChannel* netdata, IBasePlayer* entity);
	bool TargetSide(IBasePlayer*);
	bool TargetJitter(IBasePlayer*, bool);
	bool DoesHaveJitter(IBasePlayer*, int*);
	int FreestandSide[64];
public:
	float GetAngle(IBasePlayer*);
	float GetForwardYaw(IBasePlayer*);
	float GetBackwardYaw(IBasePlayer*);
	void Do(IBasePlayer*);
	void StoreAntifreestand();
};
extern CResolver* resolver;
extern std::string ResolverMode[65];