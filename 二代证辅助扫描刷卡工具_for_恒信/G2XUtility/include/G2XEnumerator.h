#pragma once
//Enumerator.h
//计数者（货物托管员的职责）
//在游泳池、大型超市门口都有物品托管员，他们负责把客户的物品放入柜子的某个格子，
// 每个格子都有一个编号（托管员给客户的编号牌对应这个编号）。每进来一位客户，托管
// 员都要选择一个空的格子给客户。当客户拿着牌子取回物品时，这个格子就腾出了。当然
// 托管员不能把已经被使用的格子分配给新来的客户，必须要寻找一个空白的格子。
// 
// 程序也要实现托管员的功能，所以定义 CEnumerator类.
// 超市的货物架的格子数是固定的，CEnumerator的格子数可以增加和减少.
// 当 某个对象需要 存放 "物品"时，CEnumerator给 这个对象一个牌子（整型编号）,
// 当这个对象 取回"物品"时，CEnumerator根据这个对象拿过来的牌子，腾出空间
//
//2009-10-30
//Author:guoxuxing

namespace G2XUtility
{

template<class ShelfType>    //ShelfType货物架的类型
class CG2XEnumerator
{
public:
	CG2XEnumerator()
	{
		m_pShelf = 0;   //默认没有物架
		m_pUsedState = 0;
		m_pAreaFlag = 0;
		m_nItemCount = 0;  //没有格子
		m_nAreaCount = 0;
		AREA_SIZE = 0;
	}
	//初始货物架格子总数
	CG2XEnumerator(unsigned int nAreasCount, unsigned int nAreaSize)
	{
		m_pShelf = 0;   //默认没有物架
		m_pUsedState = 0;
		m_pAreaFlag = 0;
		m_nItemCount = 0;  //没有格子
		m_nAreaCount = 0;
		AREA_SIZE = 0;
		Buy( nAreasCount, nAreaSize );  //购买
	}
	~CG2XEnumerator()
	{
		if( m_pShelf != NULL )
		{
			delete []m_pShelf;
		}
		if( m_pUsedState != NULL )
		{
			delete []m_pUsedState;
		}
		if( m_pAreaFlag != NULL )
		{
			delete []m_pAreaFlag;
		}
	}
	//老板通过这个方法购买物架
	void Buy(unsigned int nAreasCount, unsigned int nAreaSize)
	{
		if( nAreasCount == 0)
		{
			throw "初始货物架的区域数必须要大于0";
			return;
		}
		if( nAreaSize == 0)
		{
			throw "货物架的区域大小不能为0";
			return;
		}
		m_nItemCount = nAreasCount * nAreaSize;
		m_nAreaCount = nAreasCount;
		AREA_SIZE = nAreaSize;

		if( m_pShelf )
		{
			delete []m_pShelf;
			m_pShelf = NULL;
		}
		m_pShelf = new ShelfType[ m_nItemCount ];
		if( m_pShelf == NULL )
		{
			m_nItemCount = 0;
			return;
		}
		if( m_pUsedState )
		{
			delete []m_pUsedState;
			m_pUsedState = NULL;
		}
		m_pUsedState = new unsigned char[ m_nItemCount ];
		int i = 0;
		for(i = 0 ; i < m_nItemCount ; i ++)
		{
			m_pUsedState[i] = nobody;
		}

		//分配格子区
		if( m_pAreaFlag )
		{
			delete []m_pAreaFlag;
			m_pAreaFlag  = NULL;
		}
		m_pAreaFlag = new unsigned int[ m_nAreaCount ];
		for(i = 0 ; i < m_nAreaCount ; i ++)
		{
			m_pAreaFlag[i] = 0;
		}
	}
	unsigned int GetItemCount()
	{
		return m_nItemCount;
	}
	//腾出格子
	void Vacate(unsigned int nID)
	{
		if( nID >= m_nItemCount )
		{
			return;
		}
		m_pUsedState[nID] = nobody;
	}
	//腾出所有格子
	void VacateAll()
	{
		for(int i = 0 ; i < m_nItemCount ; i ++)
		{
			m_pUsedState[nID] = nobody;
		}
	}
	//得到剩余的牌子数
	unsigned int GetSurplusSignCount()
	{
		unsigned int nCount = 0;
		for(int i = 0 ; i < m_nAreaCount ;i ++)
		{
			if( m_pAreaFlag[i] != AREA_SIZE )
			{	//该区有剩余的格子
				int iStartPos = i*AREA_SIZE;
				int iEndPos = iStartPos + AREA_SIZE;
				for(int j = iStartPos; j < iEndPos ; j ++ )
				{
					if( m_pUsedState[j] == nobody )
						nCount ++;
				}
			}
		}
		return nCount;
	}
	//得到一个牌子ID,如果没有可使用的格子，返回-1
	unsigned int GetSign(ShelfType goods)
	{
		//找到区号
		int iArea = -1;
		int i = 0;
		for(i = 0 ; i < m_nAreaCount ; i ++ )
		{
			if( m_pAreaFlag[i] != AREA_SIZE)
			{
				iArea = i;
				break;
			}
		}
		if( iArea == -1)
		{
			return -1;
		}
		int iStartPos = iArea*AREA_SIZE;
		int iEndPos = iStartPos + AREA_SIZE? AREA_SIZE;
		//
		int iID = -1;
		int nCount = 0;
		for(i = iStartPos ; i < iEndPos ; i ++)
		{
			if( m_pUsedState[i] == nobody)
			{
				m_pUsedState[i] = someone;
				m_pShelf[i] = goods;
				m_pAreaFlag[ iArea ] += 1;
				return i;
			}
		}
		return -1;
	}
	//归还牌子
	ShelfType* GiveBack(unsigned char nID)
	{
		
		ShelfType* goods = LookUpGoods(nID);
		if (goods)
		{
			int iArea = nID / AREA_SIZE;
			m_pUsedState[nID] = nobody;
			m_pAreaFlag[ iArea ] -= 1;
		}
	}
	//查看物品
	ShelfType* LookUpGoods(unsigned int nID)
	{
		if( m_pUsedState[nID] == nobody )
		{	//哪里偷来的牌子
			return NULL;   
		}
		return &m_pShelf[nID];
	}
protected:
	enum
	{
		nobody = 0,   //无人
		someone       //有人
	};
	unsigned int m_nItemCount;     //物架格子的总数 = AREA_SIZE * m_nAreaCount
	unsigned int AREA_SIZE;
	unsigned int m_nAreaCount;  //架子区的个数，每个区的格式数为

	ShelfType* m_pShelf;   //物架
	unsigned char* m_pUsedState;    //格子的使用状态

	
	unsigned int* m_pAreaFlag;  //架子区的占满标识
};



}