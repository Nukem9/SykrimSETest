#include "../../rendering/common.h"
#include "../../../common.h"
#include "BSShaderUtil.h"

using namespace DirectX;

XMMATRIX BSShaderUtil::GetXMFromNi(const NiTransform& Transform)
{
	return GetXMFromNiPosAdjust(Transform, *(const NiPoint3 *)&GetThreadedGlobals()->__zz2[28]);
}

XMMATRIX BSShaderUtil::GetXMFromNiPosAdjust(const NiTransform& Transform, const NiPoint3& PosAdjust)
{
	XMMATRIX temp;

	const NiMatrix3& m = Transform.m_Rotate;
	const float scale = Transform.m_fScale;

	temp.r[0] = XMVectorScale(XMVectorSet(
		m.m_pEntry[0][0],
		m.m_pEntry[1][0],
		m.m_pEntry[2][0],
		0.0f), scale);

	temp.r[1] = XMVectorScale(XMVectorSet(
		m.m_pEntry[0][1],
		m.m_pEntry[1][1],
		m.m_pEntry[2][1],
		0.0f), scale);

	temp.r[2] = XMVectorScale(XMVectorSet(
		m.m_pEntry[0][2],
		m.m_pEntry[1][2],
		m.m_pEntry[2][2],
		0.0f), scale);

	temp.r[3] = XMVectorSet(
		Transform.m_Translate.x - PosAdjust.x,
		Transform.m_Translate.y - PosAdjust.y,
		Transform.m_Translate.z - PosAdjust.z,
		1.0f);

	return temp;
}