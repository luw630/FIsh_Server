// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FISHDEAD_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FISHDEAD_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FISHDEAD_EXPORTS
#define FISHDEAD_API __declspec(dllexport)
#else
#define FISHDEAD_API __declspec(dllimport)
#endif

class FAlg;

class FISHDEAD_API FishDead {

public:
	FishDead();
	~FishDead();

public:
	/******************
	* ��ʼ���ڵ���ֵ initGunPoints
	*
	* ���������
	*     setvalue - �趨�ĳ�ֵ
	*
	* ���������
	*     totalGunPoints - �趨��Ĳ���ֵ
	*
	*/
	void initGunPoints(unsigned int setvalue, unsigned int &totalGunPoints);

	/******************
	* ��ʼ�����������ֵ initFishPoints
	*
	* ���������
	*     setvalue - �趨�ĳ�ֵ
	*
	* ���������
	*     totalFishPoints - �趨��Ĳ���ֵ
	*
	*/
	void initFishPoints(unsigned int setvalue, unsigned int &totalFishPoints);

	/******************
	* �ж����Ƿ����� isFishDead
	*
	* ���������
	*     gunPoints - �������ڵ���ֵ
	*     fishRadio - �����е���ı���
	*     maxFishRadio - �趨����������
	*     minFishRadio - �趨�������С����
	*     placeType - ��������
	*     radio - ��ˮ����
	*     step - �Ѷȵȼ�
	* �������������
	*     totalGunPoints - �ܻ������ڵ����ۼƣ������ڲ��ɸ���
	*     totalFishPoints - �ܻ�����÷��ۼƣ������ڲ��ɸ���
	*
	* ����ֵ��1 - ������
	*         0 - ��δ����
	*/
	int isFishDead(int gunPoints,                 // - �������ڵ���ֵ
		int fishRadio,                 // - �����е���ı���
		int maxFishRadio,              // - �趨����������
		int minFishRadio,              // - �趨�������С����
		unsigned int &totalGunPoints,  // - �ܻ������ڵ����ۼ�
		unsigned int &totalFishPoints, // - �ܻ�����÷��ۼ�
		int placeType,                 // - ��������
		int radio,                     // - ��ˮ����
		int step                       // - �Ѷȵȼ�)
	);

private:
	FAlg *m_alg;
};