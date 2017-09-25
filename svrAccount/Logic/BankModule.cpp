#include "BankModule.h"


NS_WF_BEGIN

BankModule::BankModule()
{
	m_HavePwd = false;
	m_Pwd = "";
	m_Money = 0;
}
BankModule::~BankModule()
{

}
void BankModule::loadData(const ::rpc::t_BankDatas& data)
{
	m_HavePwd = data.havepwd();
	m_Pwd = data.pwd();
	m_Money = data.money();
}
NS_WF_END

