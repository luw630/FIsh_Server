// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FISHDEAD_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// FISHDEAD_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
	* 初始化炮弹分值 initGunPoints
	*
	* 输入参数：
	*     setvalue - 设定的初值
	*
	* 输出参数：
	*     totalGunPoints - 设定后的参数值
	*
	*/
	void initGunPoints(unsigned int setvalue, unsigned int &totalGunPoints);

	/******************
	* 初始化击中鱼分数值 initFishPoints
	*
	* 输入参数：
	*     setvalue - 设定的初值
	*
	* 输出参数：
	*     totalFishPoints - 设定后的参数值
	*
	*/
	void initFishPoints(unsigned int setvalue, unsigned int &totalFishPoints);

	/******************
	* 判断鱼是否死亡 isFishDead
	*
	* 输入参数：
	*     gunPoints - 击中鱼炮弹分值
	*     fishRadio - 被击中的鱼的倍率
	*     maxFishRadio - 设定的鱼的最大倍率
	*     minFishRadio - 设定的鱼的最小倍率
	*     placeType - 场地类型
	*     radio - 抽水比例
	*     step - 难度等级
	* 输入输出参数：
	*     totalGunPoints - 总击中鱼炮弹分累计，程序内部可更改
	*     totalFishPoints - 总击中鱼得分累计，程序内部可更改
	*
	* 返回值：1 - 鱼死亡
	*         0 - 鱼未死亡
	*/
	int isFishDead(int gunPoints,                 // - 击中鱼炮弹分值
		int fishRadio,                 // - 被击中的鱼的倍率
		int maxFishRadio,              // - 设定的鱼的最大倍率
		int minFishRadio,              // - 设定的鱼的最小倍率
		unsigned int &totalGunPoints,  // - 总击中鱼炮弹分累计
		unsigned int &totalFishPoints, // - 总击中鱼得分累计
		int placeType,                 // - 场地类型
		int radio,                     // - 抽水比例
		int step                       // - 难度等级)
	);

private:
	FAlg *m_alg;
};