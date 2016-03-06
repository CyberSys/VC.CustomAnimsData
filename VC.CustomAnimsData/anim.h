#pragma once
#pragma pack(push, 1)
//code from Maxo's Vehicle Loader

struct RwV2D
{
	float x, y;
};

struct RwV3d
{
	float x, y, z;
};

class CVector
{
public:
	float	x, y, z;

	CVector()
	{}

	CVector(float fX, float fY, float fZ = 0.0f)
		: x(fX), y(fY), z(fZ)
	{}

	CVector(const RwV3d& rwVec)
		: x(rwVec.x), y(rwVec.y), z(rwVec.z)
	{}

	CVector&		operator+=(const CVector& vec)
	{
		x += vec.x; y += vec.y; z += vec.z;
		return *this;
	}

	inline float	Magnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	friend inline float DotProduct(const CVector& vec1, const CVector& vec2)
	{
		return vec1.x * vec2.x + vec1.x * vec2.y + vec1.z * vec2.z;
	}

	friend inline CVector CrossProduct(const CVector& vec1, const CVector& vec2)
	{
		return CVector(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x);
	}

	friend inline CVector operator*(const CVector& in, float fMul)
	{
		return CVector(in.x * fMul, in.y * fMul, in.z * fMul);
	}
	friend inline CVector operator+(const CVector& vec1, const CVector& vec2)
	{
		return CVector(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
	}
	friend inline CVector operator-(const CVector& vec1, const CVector& vec2)
	{
		return CVector(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
	}
};

struct RwMatrix {
	RwV3d right; // 0-12
				 //rotation vector 1
	uint32_t dwFlags; // 12-16
					  //matrix flags
	RwV3d at; // 16-28
			  //rotation vector 2
	uint32_t dwPad01; // 28-32
					  //padding
	RwV3d up; // 32-44
			  //rotation vector 3
	uint32_t dwPad02; // 44-48
					  //padding
	RwV3d vPos; // 48-60
				//position vector
	uint32_t dwPad03; // 60-64
					  //padding
};

class CQuaternion {
public:
	CQuaternion();
	CQuaternion(float fX, float fY, float fZ, float fW);
	CQuaternion(const RwMatrix* pMat);
	CQuaternion(CVector vector);
	~CQuaternion();

	CQuaternion& operator += (const CQuaternion& a);
	CQuaternion& operator -= (const CQuaternion& a);

	CQuaternion& operator *= (const CQuaternion& a);
	CQuaternion operator * (const CQuaternion& a);

	CQuaternion& operator *= (float fMul);
	CQuaternion operator * (float fMul);

	CQuaternion operator + (const CQuaternion& a);
	CQuaternion operator - (const CQuaternion& a);

	CQuaternion operator - (void);

	void Set(float fX, float fY, float fZ, float fW);
	void Multiply(float fMul);

	CQuaternion inv(void);
	CQuaternion norm(void);

	void Invert(void);
	void FullInvert(void);
	void TransformPoint(CVector* v);
	void TransformInvPoint(CVector* v);

	void RevMul(CQuaternion* pQuat);
	float DotProduct(CQuaternion* a, CQuaternion* b);
	void Slerp(CQuaternion* a, CQuaternion* b, float fPosition);
	void Normalize(void);

	void FromEuler(float fX, float fY, float fZ);
	void FromEuler(CVector* pVector);
	void ToEuler(CVector* pVector);

	void FromMatrix(const RwMatrix* pMat);
	void ToMatrix(RwMatrix* pMat);

	float X, Y, Z, W;
};

class CQuaternionVC {
public:
	void CQuaternionVC::Slerp(CQuaternionVC* pSrc, CQuaternionVC* pDest, float fHalfAngle, float fInvHalfAngle, float fProgress); //4DFBE0h

	CQuaternion inner;
};

class CAnimBlendAssociation;
class CAnimBlendAssocGroup;
class CAnimBlendClumpData;
class CAnimBlendNode;

#define ANIMASSOC_ANIMPLAYING			0x01
#define ANIMASSOC_ANIMREPEAT			0x02
#define ANIMASSOC_DELETEAFTERBLENDOUT	0x04
#define ANIMASSOC_DELETEAFTERFINISH		0x08
#define ANIMASSOC_PARTIALANIM			0x10
#define ANIMASSOC_MOVEMENTANIM			0x20

struct ANIMSEQUENCEFRAME;
struct ANIMSEQUENCEFRAMEEXT;

#define FRAMEDATA(p,i) ((AnimNodeFrame*)p+i)
#define FRAMEDATAEXT(p,i) (AnimNodeFrame*)((AnimNodeFrameWithPos*)p+i)
#define FRAMEDATAASEXT(p) ((AnimNodeFrameWithPos*)p)

#define AFRAMEID_SPINE1 1
#define AFRAMEID_HEAD 2
#define AFRAMEID_LUPPERARM 3
#define AFRAMEID_RUPPERARM 4
#define AFRAMEID_LHAND 5
#define AFRAMEID_RHAND 6
#define AFRAMEID_LTHIGH 7
#define AFRAMEID_RTHIGH 8
#define AFRAMEID_LFOOT 9
#define AFRAMEID_RFOOT 10
#define AFRAMEID_RCALF 11
#define AFRAMEID_LCALF 12
#define AFRAMEID_LFOREARM 13
#define AFRAMEID_RFOREARM 14
#define AFRAMEID_BIP01LCLAVICLE 15
#define AFRAMEID_BIP01RCLAVICLE 16
#define AFRAMEID_NECK 17

struct AnimNodeFrame {
	CQuaternionVC stQuat; // 0-16
	//quaternion of the frame
	float fFrameTime; // 16-20
	//frame position in animation
};

struct AnimNodeFrameWithPos {
	AnimNodeFrame b; // 0-20
	//basic data of the frame
	CVector vecTranslation; // 20-32
	//translation vector (position)
};

class CAnimBlendHierarchy;

struct AnimBlendAssocLink {
	AnimBlendAssocLink* pNext; // 0-4
	//next association
	AnimBlendAssocLink* pPrev; // 4-8
	//previous association
};

struct AnimBlendFrameData {
	uint8_t uFlags; // 0-1
	//frame flags
	char _pad[3]; // 1-4
	CVector vecFramePos; // 4-16
	//position of the frame
	uintptr_t* pFrame; // 16-20
	//frame that this anim frame represents
	int nFrameNodeId; // 20-24
	//node ID of this frame
};

struct AnimBlendNodeList {
	int bHasNonMovementAnims; // 0-4
	//whether the nodes are only from movement anims
	CAnimBlendNode* pNodes[12]; // 4-44
	//animation blend nodes
};

class AnimationBlock {
public:

	AnimationBlock(void) {
		szBlockName[0] = '\0';
		bIsLoaded = false;
	}

	char szBlockName[20]; // 0-20
	//name of animation block (IFP)
	bool bIsLoaded; // 20-21
	//is block loaded
	char _pad[1]; // 21-22
	short nNumRefs; // 22-24
	//number of references to this block
	int nFirstHierarchyId; // 24-28
	//index of the first hierarchy in this block
	int nNumAnimations; // 28-32
	//number of hierarchies in this block (IFP)
};

struct StaticAnimInfoList {
	int nAnimationId; // 0-4
	//animation ID
	uint16_t wAnimationFlags; // 4-6
	//flags of this animation
	char _pad[2]; // 6-8
};

struct StaticAnimationGroup {
	char* pszGroupName; // 0-4
	//name of the animation group
	char* pszBlockName; // 4-8
	//name of the animation block (animation file)
	int nInitModelIndex; // 8-12
	//model index (ped) that is used to prepare this animation group
	int nGroupAnimCount; // 12-16
	//number of animations in this group
	const char** pszAnimationNames; // 16-20
	//list of name pointers for animations in this group
	StaticAnimInfoList* pAnimationInfos; // 20-24
	//list of info for animations in this group
};

struct AnimFileSectionHead {
	char szSectionName[4]; // 0-4
	//name of the section (does not have to be null-terminated)
	uint32_t uSectionSize; // 4-8
	//size of the section
};

struct AnimFileInfoSection {
	AnimFileSectionHead stHeader; // 0-8
	//section header
	int nSubCount; // 8-12
	//count of subsections
	char szName[20]; // 12-32
	//name of the info section
};

struct AnimFileAnimSection {
	char szPartName[28]; // 0-28
	//sequence or bone name
	int nNumFrames; // 28-32
	//number of frames
	int nUnknownA; // 32-36
	//unknown
	int nUnknownB; // 36-40
	//unknown
	int nFrameIndex; // 40-44
	//frame index
};
#pragma pack(pop)
