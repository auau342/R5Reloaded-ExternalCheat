#pragma once
#include "SimpleMath\SimpleMath.h"
#include "Memory\Memory.h"
using namespace DirectX::SimpleMath;

// "NotSDK"

// ‚Æ‚è‚ ‚¦‚¸Žg‚¦‚ê‚Î‚¢‚¢‚Ì‚Å
struct CPlayer
{
	Vector3 m_vecAbsVelocity;
	Vector3 m_localOrigin;
	char z1[0x18];
	int m_shieldHealth;
	int m_shieldHealthMax;
	char z2[0x268];
	int m_iHealth;
	char z3[0xC];
	int m_iTeamNum;
	char z4[0x34];
	Vector3 m_localAngle;
	char z5[0xDC];
	int m_iMaxHealth;
};

// offsets
namespace offset
{
	// Main
	constexpr auto dwEntityList = 0x1F33F78;
	constexpr auto dwLocalPlayer = 0x22E93F8;
	constexpr auto ViewMatrix = 0x1A93D0;
	constexpr auto ViewRender= 0xD4138F0;

	// Player
	constexpr auto m_vecAbsVelocity		= 0x140;	// Vector3
	constexpr auto m_localOrigin		= 0x14C;	// Vector3
	constexpr auto m_shieldHealth		= 0x170;	// int
	constexpr auto m_shieldHealthMax	= 0x174;	// int
	constexpr auto m_iHealth			= 0x3E0;	// int
	constexpr auto m_iTeamNum			= 0x3F0;	// int
	constexpr auto m_localAngle			= 0x428;	// Vector3
	constexpr auto m_iMaxHealth			= 0x510;	// int
	constexpr auto m_lifeState			= 0x730;	// int

	constexpr auto m_nSkin = 0xe48; // OK
	constexpr auto s_BoneMatrix = 0xEE0; // OK
	constexpr auto camera_origin = 0x1b68; // OK

	// Weapon
	constexpr auto m_latestPrimaryWeapons = 0x1704;
}

/*
	[ SpeedHack ]
	constexpr auto TimeScale = 0x1843340;
	m.Write<float>(BaseAddress + offset::TimeScale, float speed);
	
	1.f = x1
	2.f = x2
	3.f = x3...
*/

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent)
{
	uint64_t EntityList = GameBaseAddr + offset::dwEntityList;
	uint64_t BaseEntity = m.Read<uint64_t>(EntityList + (Ent << 5));
	if (!BaseEntity)
		return 0;
	return BaseEntity;
}

Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition)
{
	uint64_t pBoneArray = m.Read<uint64_t>(ent + offset::s_BoneMatrix);

	Vector3 EntityHead = Vector3();

	EntityHead.x = m.Read<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
	EntityHead.y = m.Read<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
	EntityHead.z = m.Read<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

	return EntityHead;
}

bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f)
		return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;

	return true;
}

Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed)
{
	Vector3 predict;

	float bulletTime = dis / BulletSpeed;
	Vector3 pd = m.Read<Vector3>(Entity + offset::m_vecAbsVelocity);
	predict.x = pd.x * bulletTime;
	predict.y = pd.y * bulletTime;
	predict.z = (130.f * 0.5f * (bulletTime * bulletTime)); // Ctrl + V‚µ‚½‚Ì‚Å‚Ç‚¤‚µ‚Ä‚±‚¤‚È‚é‚Ì‚©‚í‚©‚è‚Ü‚¹‚ñiŽ©”’j

	return predict;
}

Vector3 CalcAngle(const Vector3& src, const Vector3& dst)
{
	Vector3 angle = Vector3();
	Vector3 delta = Vector3((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / 3.1415926535);
	angle.y = atan(delta.y / delta.x) * (180.0f / 3.1415926535);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

void NormalizeAngles(Vector3& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}