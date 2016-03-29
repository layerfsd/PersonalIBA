#include "stdafx.h"
#include ".\ibadal.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"


BOOL CIBADAL::GetCommodityUint(CString strUintCode, CString & strCNUintName)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("Select distinct commodityunit.* "));

	strSQL.Append(_T("From commodityunit "));

	strSQL.AppendFormat(_T(" where commodityunit.unitCode = '%s' "), strUintCode);

	strSQL.Append(_T(" order by commodityunit.unitCode "));
	//写日志
	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommodityUint:%s"), strSQL);


	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		//只取第一个
		if (Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("unitName"), strCNUintName);

			bRet = TRUE;
		}
		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetSortPriceCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("Select distinct Commodity.*, CommodityGroup.groupName, CommodityGroup.parentGroupId, "));

	strSQL.Append(_T(" StoreTotal.remainQuantity, RetailPrice.price, RetailPrice.point "));
	
	strSQL.Append(_T("From Commodity "));

	strSQL.Append(_T(" left outer join StoreTotal  on StoreTotal.commodityCode=Commodity.commodityCode "));

	strSQL.Append(_T(" left outer join RetailPrice  on Commodity.CommodityCode=RetailPrice.CommodityCode  "));

	strSQL.Append(_T(" left outer join CommodityGroup  on Commodity.GroupID=CommodityGroup.GroupID "));

	strSQL.AppendFormat(_T(" where StoreTotal.storeId = %d "), CIBAConfig::GetInstance()->GetStoreID());

	if (!CommodityQueryCondition.m_strCommodityId.IsEmpty())//商品ID空
	{
		strSQL.AppendFormat(_T(" and commodity.barcode like '%%%s'"), CommodityQueryCondition.m_strCommodityId);
	}

	if (CommodityQueryCondition.m_bHasPoint)
	{
		strSQL.Append(_T(" and retailprice.point > 0"));
	}

	strSQL.Append(_T(" group  by CommodityGroup.GroupName, RetailPrice.Price "));
	//写日志
	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetSortPriceCommodity:%s"), strSQL);

	CommodityArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CCommodity Commodity;

			CString strTmp;

			INT nTmp = 0;

			Rs.GetFieldValue(_T("commodityCode"), strTmp);
			Commodity.SetCommodityCode(strTmp);
			Rs.GetFieldValue(_T("commodityName"), strTmp);
			Commodity.SetCommodityName(strTmp);
			Rs.GetFieldValue(_T("shortName"), strTmp);
			Commodity.SetShortName(strTmp);
			Rs.GetFieldValue(_T("groupId"), strTmp);
			Commodity.SetGroupId(strTmp);
			Rs.GetFieldValue(_T("groupName"), strTmp);
			Commodity.SetGroupName(strTmp);
			Rs.GetFieldValue(_T("barCode"), strTmp);
			Commodity.SetBarCode(strTmp);
			Rs.GetFieldValue(_T("statusFlag"), strTmp);
			Commodity.SetStatusFlag(strTmp);
			Rs.GetFieldValue(_T("coreFlag"), strTmp);
			Commodity.SetCoreFlag(strTmp);
			Rs.GetFieldValue(_T("deliveryFlag"), strTmp);
			Commodity.SetDeliveryFlag(strTmp);
			Rs.GetFieldValue(_T("storeFlag"), strTmp);
			Commodity.SetStoreFlag(strTmp);
			Rs.GetFieldValue(_T("purchaseFlag"), strTmp);
			Commodity.SetPurchaseFlag(strTmp);
			Rs.GetFieldValue(_T("storeUnit"), strTmp);
			Commodity.SetStoreUnit(strTmp);
			Rs.GetFieldValue(_T("retailUnit"), strTmp);
			Commodity.SetRetailUnit(strTmp);
			Rs.GetFieldValue(_T("unitMultiple"), nTmp);
			Commodity.SetUnitMultiple(nTmp);
			Rs.GetFieldValue(_T("contractPrice"), nTmp);
			Commodity.SetContractPrice(nTmp);
			Rs.GetFieldValue(_T("price"), nTmp);
			Commodity.SetAdvicePrice(nTmp);
			Rs.GetFieldValue(_T("unitCost"), nTmp);
			Commodity.SetUnitCost(nTmp);
			Rs.GetFieldValue(_T("producerCode"), strTmp);
			Commodity.SetProducerCode(strTmp);
			Rs.GetFieldValue(_T("productCode"), strTmp);
			Commodity.SetProductCode(strTmp);
			Rs.GetFieldValue(_T("description"), strTmp);
			Commodity.SetDescription(strTmp);
			Rs.GetFieldValue(_T("image"), strTmp);
			Commodity.SetImage(strTmp);
			Rs.GetFieldValue(_T("accountType"), nTmp);
			Commodity.SetAccountType(nTmp);
			Rs.GetFieldValue(_T("remainQuantity"), nTmp);
			Commodity.SetRemainQuantity(nTmp);
			Rs.GetFieldValue(_T("point"), nTmp);
			Commodity.SetPoint(nTmp);

			CommodityArray.Add(Commodity);

			bRet = TRUE;

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetPriceCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("SELECT distinct commodity.*, commoditygroup.groupName, commoditygroup.parentGroupId, "));

	strSQL.Append(_T("storetotal.remainQuantity, retailprice.price, retailprice.point FROM commodity, commoditygroup, storetotal, retailprice "));

	strSQL.Append(_T("where commodity.groupId = commoditygroup.groupId and commodity.commodityCode = storetotal.commodityCode"));

	strSQL.Append(_T(" and commodity.commodityCode = retailprice.commodityCode"));

	if (!CommodityQueryCondition.m_strCommodityId.IsEmpty())//商品ID空
	{
		strSQL.AppendFormat(_T(" and commodity.barCode like '%%%s'"), CommodityQueryCondition.m_strCommodityId);
	}

	if (CommodityQueryCondition.m_bHasPoint)
	{
		strSQL.Append(_T(" and retailprice.point > 0"));
	}

	strSQL.AppendFormat(_T(" and storetotal.storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(" order by retailprice.price"));
	//写日志
	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetPriceCommodity:%s"), strSQL);

	CommodityArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CCommodity Commodity;

			CString strTmp;

			INT nTmp = 0;

			Rs.GetFieldValue(_T("commodityCode"), strTmp);
			Commodity.SetCommodityCode(strTmp);
			Rs.GetFieldValue(_T("commodityName"), strTmp);
			Commodity.SetCommodityName(strTmp);
			Rs.GetFieldValue(_T("shortName"), strTmp);
			Commodity.SetShortName(strTmp);
			Rs.GetFieldValue(_T("groupId"), strTmp);
			Commodity.SetGroupId(strTmp);
			Rs.GetFieldValue(_T("groupName"), strTmp);
			Commodity.SetGroupName(strTmp);
			Rs.GetFieldValue(_T("barCode"), strTmp);
			Commodity.SetBarCode(strTmp);
			Rs.GetFieldValue(_T("statusFlag"), strTmp);
			Commodity.SetStatusFlag(strTmp);
			Rs.GetFieldValue(_T("coreFlag"), strTmp);
			Commodity.SetCoreFlag(strTmp);
			Rs.GetFieldValue(_T("deliveryFlag"), strTmp);
			Commodity.SetDeliveryFlag(strTmp);
			Rs.GetFieldValue(_T("storeFlag"), strTmp);
			Commodity.SetStoreFlag(strTmp);
			Rs.GetFieldValue(_T("purchaseFlag"), strTmp);
			Commodity.SetPurchaseFlag(strTmp);
			Rs.GetFieldValue(_T("storeUnit"), strTmp);
			Commodity.SetStoreUnit(strTmp);
			Rs.GetFieldValue(_T("retailUnit"), strTmp);
			Commodity.SetRetailUnit(strTmp);
			Rs.GetFieldValue(_T("unitMultiple"), nTmp);
			Commodity.SetUnitMultiple(nTmp);
			Rs.GetFieldValue(_T("contractPrice"), nTmp);
			Commodity.SetContractPrice(nTmp);
			Rs.GetFieldValue(_T("price"), nTmp);
			Commodity.SetAdvicePrice(nTmp);
			Rs.GetFieldValue(_T("unitCost"), nTmp);
			Commodity.SetUnitCost(nTmp);
			Rs.GetFieldValue(_T("producerCode"), strTmp);
			Commodity.SetProducerCode(strTmp);
			Rs.GetFieldValue(_T("productCode"), strTmp);
			Commodity.SetProductCode(strTmp);
			Rs.GetFieldValue(_T("description"), strTmp);
			Commodity.SetDescription(strTmp);
			Rs.GetFieldValue(_T("image"), strTmp);
			Commodity.SetImage(strTmp);
			Rs.GetFieldValue(_T("accountType"), nTmp);
			Commodity.SetAccountType(nTmp);
			Rs.GetFieldValue(_T("remainQuantity"), nTmp);
			Commodity.SetRemainQuantity(nTmp);
			Rs.GetFieldValue(_T("point"), nTmp);
			Commodity.SetPoint(nTmp);

			CommodityArray.Add(Commodity);

			bRet = TRUE;

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetStoreCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("SELECT distinct commodity.*, commoditygroup.groupName, commoditygroup.parentGroupId, "));
	
	//strSQL.Append(_T("storetotal.remainQuantity, storetotal.storeInQuantity, storetotal.storeInAmount, ")); 

	strSQL.Append(_T("storetotal.remainQuantity, retailprice.price, retailprice.point FROM commodity, commoditygroup, storetotal, retailprice "));
	
	strSQL.Append(_T("where commodity.groupId = commoditygroup.groupId and commodity.commodityCode = storetotal.commodityCode"));

	strSQL.Append(_T(" and commodity.commodityCode = retailprice.commodityCode"));

	if (!CommodityQueryCondition.m_strCommodityId.IsEmpty())//商品ID空
	{
		strSQL.AppendFormat(_T(" and commodity.barCode like '%%%s'"), CommodityQueryCondition.m_strCommodityId);
	}

	if (CommodityQueryCondition.m_bHasPoint)
	{
		strSQL.Append(_T(" and retailprice.point > 0"));
	}

	strSQL.AppendFormat(_T(" and storetotal.storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(" order by groupId"));
	//写日志
	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetStoreCommodity:%s"), strSQL);

	CommodityArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CCommodity Commodity;

			CString strTmp;

			INT nTmp = 0;

			Rs.GetFieldValue(_T("commodityCode"), strTmp);
			Commodity.SetCommodityCode(strTmp);
			Rs.GetFieldValue(_T("commodityName"), strTmp);
			Commodity.SetCommodityName(strTmp);
			Rs.GetFieldValue(_T("shortName"), strTmp);
			Commodity.SetShortName(strTmp);
			Rs.GetFieldValue(_T("groupId"), strTmp);
			Commodity.SetGroupId(strTmp);
			Rs.GetFieldValue(_T("groupName"), strTmp);
			Commodity.SetGroupName(strTmp);
			Rs.GetFieldValue(_T("barCode"), strTmp);
			Commodity.SetBarCode(strTmp);
			Rs.GetFieldValue(_T("statusFlag"), strTmp);
			Commodity.SetStatusFlag(strTmp);
			Rs.GetFieldValue(_T("coreFlag"), strTmp);
			Commodity.SetCoreFlag(strTmp);
			Rs.GetFieldValue(_T("deliveryFlag"), strTmp);
			Commodity.SetDeliveryFlag(strTmp);
			Rs.GetFieldValue(_T("storeFlag"), strTmp);
			Commodity.SetStoreFlag(strTmp);
			Rs.GetFieldValue(_T("purchaseFlag"), strTmp);
			Commodity.SetPurchaseFlag(strTmp);
			Rs.GetFieldValue(_T("storeUnit"), strTmp);
			Commodity.SetStoreUnit(strTmp);
			Rs.GetFieldValue(_T("retailUnit"), strTmp);
			Commodity.SetRetailUnit(strTmp);
			Rs.GetFieldValue(_T("unitMultiple"), nTmp);
			Commodity.SetUnitMultiple(nTmp);
			Rs.GetFieldValue(_T("contractPrice"), nTmp);
			Commodity.SetContractPrice(nTmp);
			Rs.GetFieldValue(_T("price"), nTmp);
			Commodity.SetAdvicePrice(nTmp);
			Rs.GetFieldValue(_T("unitCost"), nTmp);
			Commodity.SetUnitCost(nTmp);
			Rs.GetFieldValue(_T("producerCode"), strTmp);
			Commodity.SetProducerCode(strTmp);
			Rs.GetFieldValue(_T("productCode"), strTmp);
			Commodity.SetProductCode(strTmp);
			Rs.GetFieldValue(_T("description"), strTmp);
			Commodity.SetDescription(strTmp);
			Rs.GetFieldValue(_T("image"), strTmp);
			Commodity.SetImage(strTmp);
			Rs.GetFieldValue(_T("accountType"), nTmp);
			Commodity.SetAccountType(nTmp);
			Rs.GetFieldValue(_T("remainQuantity"), nTmp);
			Commodity.SetRemainQuantity(nTmp);
			Rs.GetFieldValue(_T("point"), nTmp);
			Commodity.SetPoint(nTmp);

			//Rs.GetFieldValue(_T("storeInQuantity"), nTmp);
			//Commodity.SetStoreInQuantity(nTmp);
			//Rs.GetFieldValue(_T("storeInAmount"), nTmp);
			//Commodity.SetStoreInAmount(nTmp);

			CommodityArray.Add(Commodity);

			bRet = TRUE;

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetCommodity(CCommodityArray& CommodityArray, CCommodityQueryCondition & CommodityQueryCondition)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("SELECT distinct commodity.*, commoditygroup.groupName, commoditygroup.parentGroupId, "));

	//strSQL.Append(_T("storetotal.remainQuantity, storetotal.storeInQuantity, storetotal.storeInAmount, ")); 
	
	strSQL.Append(_T("storetotal.remainQuantity, retailprice.price, retailprice.point FROM commodity ")); 

	strSQL.AppendFormat(_T("left outer join storetotal on commodity.commodityCode = storetotal.commodityCode and storetotal.storeId = %d"), 
		CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(", commoditygroup, retailprice where commodity.groupId = commoditygroup.groupId "));

	strSQL.Append(_T(" and commodity.commodityCode = retailprice.commodityCode"));

	if (!CommodityQueryCondition.m_strCommodityId.IsEmpty())//商品ID空
	{
		strSQL.AppendFormat(_T(" and commodity.barCode like '%%%s'"), CommodityQueryCondition.m_strCommodityId);
	}

	if (CommodityQueryCondition.m_bHasPoint)
	{
		strSQL.Append(_T(" and retailprice.point > 0"));
	}

	strSQL.Append(_T(" order by groupId"));

	//写日志
	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommodity:%s"), strSQL);

	CommodityArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	CIBALog::GetInstance()->WriteFormat(_T("执行SQL  Step1"));
	if (!GetDBConnInfo(pDb)) return bRet;
	CIBALog::GetInstance()->WriteFormat(_T("执行SQL  Step2"));

	CADORecordset Rs(pDb->GetConn());
	CIBALog::GetInstance()->WriteFormat(_T("执行SQL  Step3"));

	if (Rs.Open(strSQL))
	{
		CIBALog::GetInstance()->WriteFormat(_T("执行SQL成功"));

		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CCommodity Commodity;

			CString strTmp;

			INT nTmp = 0;

			Rs.GetFieldValue(_T("commodityCode"), strTmp);
			Commodity.SetCommodityCode(strTmp);
			Rs.GetFieldValue(_T("commodityName"), strTmp);
			Commodity.SetCommodityName(strTmp);
			Rs.GetFieldValue(_T("shortName"), strTmp);
			Commodity.SetShortName(strTmp);
			Rs.GetFieldValue(_T("groupId"), strTmp);
			Commodity.SetGroupId(strTmp);
			Rs.GetFieldValue(_T("groupName"), strTmp);
			Commodity.SetGroupName(strTmp);
			Rs.GetFieldValue(_T("barCode"), strTmp);
			Commodity.SetBarCode(strTmp);
			Rs.GetFieldValue(_T("statusFlag"), strTmp);
			Commodity.SetStatusFlag(strTmp);
			Rs.GetFieldValue(_T("coreFlag"), strTmp);
			Commodity.SetCoreFlag(strTmp);
			Rs.GetFieldValue(_T("deliveryFlag"), strTmp);
			Commodity.SetDeliveryFlag(strTmp);
			Rs.GetFieldValue(_T("storeFlag"), strTmp);
			Commodity.SetStoreFlag(strTmp);
			Rs.GetFieldValue(_T("purchaseFlag"), strTmp);
			Commodity.SetPurchaseFlag(strTmp);
			Rs.GetFieldValue(_T("storeUnit"), strTmp);
			Commodity.SetStoreUnit(strTmp);
			Rs.GetFieldValue(_T("retailUnit"), strTmp);
			Commodity.SetRetailUnit(strTmp);
			Rs.GetFieldValue(_T("unitMultiple"), nTmp);
			Commodity.SetUnitMultiple(nTmp);
			Rs.GetFieldValue(_T("contractPrice"), nTmp);
			Commodity.SetContractPrice(nTmp);
			Rs.GetFieldValue(_T("price"), nTmp);
			Commodity.SetAdvicePrice(nTmp);
			Rs.GetFieldValue(_T("unitCost"), nTmp);
			Commodity.SetUnitCost(nTmp);
			Rs.GetFieldValue(_T("producerCode"), strTmp);
			Commodity.SetProducerCode(strTmp);
			Rs.GetFieldValue(_T("productCode"), strTmp);
			Commodity.SetProductCode(strTmp);
			Rs.GetFieldValue(_T("description"), strTmp);
			Commodity.SetDescription(strTmp);
			Rs.GetFieldValue(_T("image"), strTmp);
			Commodity.SetImage(strTmp);
			Rs.GetFieldValue(_T("accountType"), nTmp);
			Commodity.SetAccountType(nTmp);
			Rs.GetFieldValue(_T("remainQuantity"), nTmp);
			Commodity.SetRemainQuantity(nTmp);
			//Rs.GetFieldValue(_T("storeInQuantity"), nTmp);
			//Commodity.SetStoreInQuantity(nTmp);
			//Rs.GetFieldValue(_T("storeInAmount"), nTmp);
			//Commodity.SetStoreInAmount(nTmp);
			Rs.GetFieldValue(_T("point"), nTmp);
			Commodity.SetPoint(nTmp);

			CommodityArray.Add(Commodity);

			bRet = TRUE;

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::AddRetailVoucher(CRetailVoucher& RetailVoucher)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL.Append(_T("select * from RetailVoucher where 1=0"));

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) && Rs.AddNew())
	{
		Rs.SetFieldValue(_T("voucherNo"), RetailVoucher.GetVoucherNo());
		Rs.SetFieldValue(_T("oldVoucherNo"), RetailVoucher.GetOldVoucherNo());
		Rs.SetFieldValue(_T("netBarId"), RetailVoucher.GetNetBarId());
		Rs.SetFieldValue(_T("storeId"), RetailVoucher.GetStoreId());
		Rs.SetFieldValue(_T("refNo"), RetailVoucher.GetRefNo());
		Rs.SetFieldValue(_T("serialNo"), RetailVoucher.GetSerialNo());
		Rs.SetFieldValue(_T("payType"), RetailVoucher.GetPayType());
		Rs.SetFieldValue(_T("retailType"), RetailVoucher.GetRetailType());
		Rs.SetFieldValue(_T("memberId"), RetailVoucher.GetMemberId());
		Rs.SetFieldValue(_T("week"), RetailVoucher.GetWeek());
		Rs.SetFieldValue(_T("classId"), RetailVoucher.GetClassId());
		Rs.SetFieldValue(_T("userId"), RetailVoucher.GetUserId());
		Rs.SetFieldValue(_T("voucherTime"), RetailVoucher.GetVoucherTime());
		Rs.SetFieldValue(_T("description"), RetailVoucher.GetDescription());
		Rs.SetFieldValue(_T("cashRegisterID"), RetailVoucher.GetCashRegisterID());
		Rs.SetFieldValue(_T("point"), RetailVoucher.GetPoint());
		Rs.SetFieldValue(_T("accountYear"), RetailVoucher.GetAccountYear());
		Rs.SetFieldValue(_T("accountMonth"), RetailVoucher.GetAccountMonth());
		Rs.SetFieldValue(_T("voucherStatus"), RetailVoucher.GetVoucherStatus());

		bRet = Rs.Update();

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::AddRetailDetail(CRetailDetail& RetailDetail)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL.Append(_T("select * from RetailDetail where 1=0"));

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) && Rs.AddNew())
	{
		Rs.SetFieldValue(_T("voucherNo"), RetailDetail.GetVoucherNo());
		Rs.SetFieldValue(_T("commodityCode"), RetailDetail.GetCommodityCode());
		Rs.SetFieldValue(_T("quantity"), RetailDetail.GetQuantity());
		Rs.SetFieldValue(_T("price"), RetailDetail.GetPrice());
		Rs.SetFieldValue(_T("amount"), RetailDetail.GetAmount());
		Rs.SetFieldValue(_T("cost"), RetailDetail.GetCost());
		Rs.SetFieldValue(_T("Point"), RetailDetail.GetPoint());
		Rs.SetFieldValue(_T("returnQuantity"), RetailDetail.GetReturnQuantity());
		
		bRet = Rs.Update();

		Rs.Close();
		
		//更新库存
		strSQL.Format( _T("select * from storetotal where commodityCode ='%s' and storeId = %d")
			, RetailDetail.GetCommodityCode(), CIBAConfig::GetInstance()->GetStoreID());
		
		if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
		{
			INT nTmp = 0;
			Rs.GetFieldValue(_T("remainQuantity"), nTmp);
			nTmp -= RetailDetail.GetQuantity();

			Rs.Edit();
			Rs.SetFieldValue(_T("remainQuantity"), nTmp);

			Rs.Update();
			Rs.Close();
		}
	}

	pDb->Release();

	return bRet;
}


BOOL CIBADAL::GetRetailInfo(CRetailInfoArray & RetailInfoArray, CRetailQueryCondition & RetailQueryCondition)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("SELECT "));
	strSQL.Append(_T("retailvoucher.voucherNo,retailvoucher.oldVoucherNo,retailvoucher.storeId,retailvoucher.refNo,"));
	strSQL.Append(_T("retailvoucher.serialNo,retailvoucher.payType,retailvoucher.retailType,retailvoucher.memberId,"));
	strSQL.Append(_T("retailvoucher.classId,retailvoucher.voucherTime,retailvoucher.description,retailvoucher.userId,"));

	strSQL.Append(_T("retaildetail.commodityCode,retaildetail.quantity,retaildetail.price,retaildetail.amount,retaildetail.cost,retaildetail.Point,retaildetail.returnQuantity,"));
	strSQL.Append(_T("commodity.commodityName,commodity.groupId, commodity.barCode "));
	
	strSQL.Append(_T("FROM retailvoucher, retaildetail, commodity "));

	strSQL.Append(_T("WHERE "));
	strSQL.Append(_T("retailvoucher.voucherNo = retaildetail.voucherNo and retaildetail.commodityCode = commodity.commodityCode "));
	
	if (RetailQueryCondition.m_nStoreId > 0)
	{
		strSQL.AppendFormat(_T(" and retailvoucher.storeId = %d"), RetailQueryCondition.m_nStoreId);
	}
	
	if (RetailQueryCondition.m_nPayType != 0)
	{
		strSQL.AppendFormat(_T(" and retailvoucher.payType = %d"), RetailQueryCondition.m_nPayType);
	}

	if (RetailQueryCondition.m_nRetailType != 0)
	{
		strSQL.AppendFormat(_T(" and retailvoucher.retailType = %d"), RetailQueryCondition.m_nRetailType);
	}
	
	if (!RetailQueryCondition.m_strCashier.IsEmpty())
	{
		strSQL.AppendFormat(_T(" and retailvoucher.userId = '%s'"), RetailQueryCondition.m_strCashier);
	}
	
	if (!RetailQueryCondition.m_strCommodityId.IsEmpty())
	{
		strSQL.AppendFormat(_T(" and retaildetail.commodityCode like '%%%s'"), RetailQueryCondition.m_strCommodityId);
	}

	strSQL.AppendFormat(_T(" and retailvoucher.voucherTime between '%s' and '%s'"), 
		RetailQueryCondition.m_strStartTime, RetailQueryCondition.m_strEndTime);

	strSQL.Append(_T(" order by retailvoucher.retailType desc, retailvoucher.voucherNo desc"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetRetailInfo:%s"), strSQL);
	
	RetailInfoArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CRetailInfo RetailInfo;

			CString strTmp; INT nTmp = 0;

			Rs.GetFieldValue(_T("voucherNo"), strTmp);
			RetailInfo.RetailVoucher.SetVoucherNo(strTmp);
			RetailInfo.RetailDetail.SetVoucherNo(strTmp);

			Rs.GetFieldValue(_T("oldVoucherNo"), strTmp);
			RetailInfo.RetailVoucher.SetOldVoucherNo(strTmp);
			Rs.GetFieldValue(_T("storeId"), nTmp);
			RetailInfo.RetailVoucher.SetStoreId(nTmp);

			Rs.GetFieldValue(_T("refNo"), nTmp);
			RetailInfo.RetailVoucher.SetRefNo(nTmp);
			Rs.GetFieldValue(_T("serialNo"), nTmp);
			RetailInfo.RetailVoucher.SetSerialNo(nTmp);

			Rs.GetFieldValue(_T("payType"), nTmp);
			RetailInfo.RetailVoucher.SetPayType(nTmp);
			Rs.GetFieldValue(_T("retailType"), nTmp);
			RetailInfo.RetailVoucher.SetRetailType(nTmp);

			Rs.GetFieldValue(_T("memberId"), nTmp);
			RetailInfo.RetailVoucher.SetMemberId(nTmp);
			Rs.GetFieldValue(_T("classId"), nTmp);
			RetailInfo.RetailVoucher.SetClassId(nTmp);
			Rs.GetFieldValue(_T("voucherTime"), strTmp);
			RetailInfo.RetailVoucher.SetVoucherTime(strTmp);

			Rs.GetFieldValue(_T("userId"), strTmp);
			RetailInfo.RetailVoucher.SetUserId(strTmp);
			Rs.GetFieldValue(_T("description"), strTmp);
			RetailInfo.RetailVoucher.SetDescription(strTmp);

			Rs.GetFieldValue(_T("commodityCode"), strTmp);
			RetailInfo.RetailDetail.SetCommodityCode(strTmp);

			Rs.GetFieldValue(_T("quantity"), nTmp);
			RetailInfo.RetailDetail.SetQuantity(nTmp);
			Rs.GetFieldValue(_T("price"), nTmp);
			RetailInfo.RetailDetail.SetPrice(nTmp);
			Rs.GetFieldValue(_T("amount"), nTmp);
			RetailInfo.RetailDetail.SetAmount(nTmp);

			Rs.GetFieldValue(_T("cost"), nTmp);
			RetailInfo.RetailDetail.SetCost(nTmp);
			Rs.GetFieldValue(_T("Point"), nTmp);
			RetailInfo.RetailDetail.SetPoint(nTmp);
			Rs.GetFieldValue(_T("returnQuantity"), nTmp);
			RetailInfo.RetailDetail.SetReturnQuantity(nTmp);

			Rs.GetFieldValue(_T("commodityName"), strTmp);
			RetailInfo.Commodity.SetCommodityName(strTmp);
			Rs.GetFieldValue(_T("groupId"), strTmp);
			RetailInfo.Commodity.SetGroupId(strTmp);

			RetailInfoArray.Add(RetailInfo);

			bRet = TRUE;

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}


BOOL CIBADAL::ZeroAllCommodityRemainQuantity()
{
	CString strSQL(_T("update storetotal set remainQuantity = 0"));

	strSQL.AppendFormat(_T(" where storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	return ExecuteSQL(strSQL);
}

BOOL CIBADAL::UpdateCommodityRemainQuantity(CString strCommodityCode, UINT nRemainQuantity)
{
	CString strSQL;

	strSQL.Format(_T("update storetotal set remainQuantity = %d where commodityCode = '%s'"), nRemainQuantity, strCommodityCode);

	strSQL.AppendFormat(_T(" and storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	return ExecuteSQL(strSQL);
}

BOOL CIBADAL::ZeroAllCommodityStoreInQuantity()
{
	CString strSQL(_T("update storetotal set storeInQuantity = 0, storeInAmount = 0"));

	strSQL.AppendFormat(_T(" where storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	return ExecuteSQL(strSQL);
}

BOOL CIBADAL::UpdateCommodityStoreInQuantity(CString strCommodityCode, UINT nStoreInQuantity, UINT nStoreInAmount)
{
	CString strSQL;

	strSQL.Format(_T("update storetotal set remainQuantity = remainQuantity + %d, storeInQuantity = storeInQuantity + %d, \
					 storeInAmount = storeInAmount + %d where commodityCode = '%s'"), 

		nStoreInQuantity, nStoreInQuantity, nStoreInAmount, strCommodityCode);

	strSQL.AppendFormat(_T(" and storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	return ExecuteSQL(strSQL);
}

BOOL CIBADAL::GetCommoditySoldInfo(CString strCommodityCode, CCommoditySoldInfo& CommoditySoldInfo)
{
	BOOL bRet = FALSE;

	CString strSQL;

	//统计销售

	strSQL = _T("SELECT retailvoucher.payType, sum(quantity) as quantity, sum(amount) as amount");

	strSQL.Append(_T(" FROM retaildetail, retailvoucher "));

	strSQL.Append(_T("where retaildetail.voucherNo = retailvoucher.voucherNo "));

	strSQL.AppendFormat(_T("and retailvoucher.userId = '%s' "), theApp.GetCurCashier()->GetName());

	strSQL.AppendFormat(_T("and retailvoucher.voucherTime > '%s' "), theApp.GetCurCashier()->GetOnDutyTime());

	strSQL.AppendFormat(_T("and commodityCode = '%s'"), strCommodityCode);

	strSQL.AppendFormat(_T(" and storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(" and retailvoucher.retailType = 1 group by retailvoucher.payType"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommoditySoldInfo(1):%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
	{
		Rs.MoveFirst();

		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CString strTmp;

			INT nPayType = 0;

			Rs.GetFieldValue(_T("payType"), nPayType);

			switch (nPayType)
			{
			case 1: //现金销售
				Rs.GetFieldValue(_T("quantity"), strTmp); //SUM返回的类型是浮点的，所以不能直接用整形获取其值
				CommoditySoldInfo.SetSoldCount1(_ttoi(strTmp));
				Rs.GetFieldValue(_T("amount"), strTmp);
				CommoditySoldInfo.SetSoldAmout1(_ttoi(strTmp));
				break;

			case 2: //账户销售
				Rs.GetFieldValue(_T("quantity"), strTmp);
				CommoditySoldInfo.SetSoldCount2(_ttoi(strTmp));
				Rs.GetFieldValue(_T("amount"), strTmp);
				CommoditySoldInfo.SetSoldAmout2(_ttoi(strTmp));
				break;
			}	

			Rs.MoveNext();
		}

		Rs.Close();

		bRet = TRUE;
	}


	//******************************************************************************

	//统计退货

	strSQL = _T("SELECT retailvoucher.payType, sum(quantity) as quantity, sum(amount) as amount");

	strSQL.Append(_T(" FROM retaildetail, retailvoucher "));

	strSQL.Append(_T("where retaildetail.voucherNo = retailvoucher.voucherNo "));

	strSQL.AppendFormat(_T("and retailvoucher.userId = '%s' "), theApp.GetCurCashier()->GetName());

	strSQL.AppendFormat(_T("and retailvoucher.voucherTime > '%s' "), theApp.GetCurCashier()->GetOnDutyTime());

	strSQL.AppendFormat(_T("and commodityCode = '%s'"), strCommodityCode);

	strSQL.AppendFormat(_T(" and storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(" and retailvoucher.retailType = 2 group by retailvoucher.payType"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommoditySoldInfo(2):%s"), strSQL);

	if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
	{
		Rs.MoveFirst();

		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CString strTmp;

			INT nPayType = 0;

			Rs.GetFieldValue(_T("payType"), nPayType);

			switch (nPayType)
			{
			case 1: //现金销售
				Rs.GetFieldValue(_T("quantity"), strTmp); //SUM返回的类型是浮点的，所以不能直接用整形获取其值
				CommoditySoldInfo.SetReturnCount1(_ttoi(strTmp));
				Rs.GetFieldValue(_T("amount"), strTmp);
				CommoditySoldInfo.SetReturnAmout1(_ttoi(strTmp));
				break;

			case 2: //账户退货
				Rs.GetFieldValue(_T("quantity"), strTmp);
				CommoditySoldInfo.SetReturnCount2(_ttoi(strTmp));
				Rs.GetFieldValue(_T("amount"), strTmp);
				CommoditySoldInfo.SetReturnAmout2(_ttoi(strTmp));
				break;
			}	

			Rs.MoveNext();
		}

		Rs.Close();

		bRet = TRUE;
	}

	//******************************************************************************

	//统计赠送

	strSQL = _T("SELECT sum(quantity) as quantity, sum(retaildetail.point) as point");

	strSQL.Append(_T(" FROM retaildetail, retailvoucher "));

	strSQL.Append(_T("where retaildetail.voucherNo = retailvoucher.voucherNo "));

	strSQL.AppendFormat(_T("and retailvoucher.userId = '%s' "), theApp.GetCurCashier()->GetName());

	strSQL.AppendFormat(_T("and retailvoucher.voucherTime > '%s' "), theApp.GetCurCashier()->GetOnDutyTime());

	strSQL.AppendFormat(_T("and commodityCode = '%s'"), strCommodityCode);

	strSQL.AppendFormat(_T(" and storeId = %d"), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T(" and retailvoucher.retailType = 3"));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommoditySoldInfo(3):%s"), strSQL);

	if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
	{
		CString strTmp;

		Rs.GetFieldValue(_T("quantity"), strTmp);
		CommoditySoldInfo.SetPresentCount(_ttoi(strTmp));
		Rs.GetFieldValue(_T("point"), strTmp);
		CommoditySoldInfo.SetExchangePoint(_ttoi(strTmp));

		Rs.Close();

		bRet = TRUE;
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetCommoditySoldInfo(CCommoditySoldInfoArray& CommoditySoldInfoArray)
{
	BOOL bRet = FALSE;

	CString strSQL;

	//统计销售

	strSQL = _T("SELECT retailvoucher.payType, retailvoucher.retailType, commodityCode, ");

	strSQL.Append(_T(" sum(quantity) as quantity, sum(amount) as amount, sum(retaildetail.point) as point "));

	strSQL.Append(_T(" FROM retaildetail, retailvoucher "));

	strSQL.Append(_T("where retaildetail.voucherNo = retailvoucher.voucherNo "));

	strSQL.AppendFormat(_T("and retailvoucher.userId = '%s' "), theApp.GetCurCashier()->GetName());

	strSQL.AppendFormat(_T("and retailvoucher.voucherTime > '%s' "), theApp.GetCurCashier()->GetOnDutyTime());

	strSQL.AppendFormat(_T(" and storeId = %d "), CIBAConfig::GetInstance()->GetStoreID());

	strSQL.Append(_T("group by retailvoucher.payType,commodityCode "));

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetCommoditySoldInfo:%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			Rs.MoveFirst();

			for (UINT i = 0; i < Rs.GetRecordCount(); i++)
			{
				CString strTmp;
				INT nTmp = 0;
				CCommoditySoldInfo CommoditySoldInfo;

				Rs.GetFieldValue(_T("payType"), nTmp);
				CommoditySoldInfo.SetPayType(nTmp);

				Rs.GetFieldValue(_T("retailType"), nTmp);
				CommoditySoldInfo.SetRetailType(nTmp);

				Rs.GetFieldValue(_T("commodityCode"), strTmp);
				CommoditySoldInfo.SetCommoditycode(strTmp);

				switch(CommoditySoldInfo.GetRetailType())
				{
				case 1://零售
					{
						switch (CommoditySoldInfo.GetPayType())
						{
						case 1: //现金销售
							Rs.GetFieldValue(_T("quantity"), strTmp); //SUM返回的类型是浮点的，所以不能直接用整形获取其值
							CommoditySoldInfo.SetSoldCount1(_ttoi(strTmp));
							Rs.GetFieldValue(_T("amount"), strTmp);
							CommoditySoldInfo.SetSoldAmout1(_ttoi(strTmp));
							break;

						case 2: //账户销售
							Rs.GetFieldValue(_T("quantity"), strTmp);
							CommoditySoldInfo.SetSoldCount2(_ttoi(strTmp));
							Rs.GetFieldValue(_T("amount"), strTmp);
							CommoditySoldInfo.SetSoldAmout2(_ttoi(strTmp));
							break;
						}	
						break;
					}
				case 2://退款
					{
						switch (CommoditySoldInfo.GetPayType())
						{
						case 1: //现金销售
							Rs.GetFieldValue(_T("quantity"), strTmp); //SUM返回的类型是浮点的，所以不能直接用整形获取其值
							CommoditySoldInfo.SetReturnCount1(_ttoi(strTmp));
							Rs.GetFieldValue(_T("amount"), strTmp);
							CommoditySoldInfo.SetReturnAmout1(_ttoi(strTmp));
							break;

						case 2: //账户销售
							Rs.GetFieldValue(_T("quantity"), strTmp);
							CommoditySoldInfo.SetReturnCount2(_ttoi(strTmp));
							Rs.GetFieldValue(_T("amount"), strTmp);
							CommoditySoldInfo.SetReturnAmout2(_ttoi(strTmp));
							break;
						}	
						break;
					}
				case 3://赠送
					{
						Rs.GetFieldValue(_T("quantity"), strTmp);
						CommoditySoldInfo.SetPresentCount(_ttoi(strTmp));
						Rs.GetFieldValue(_T("point"), strTmp);
						CommoditySoldInfo.SetExchangePoint(_ttoi(strTmp));
						break;
					}
				}

				Rs.MoveNext();

				CommoditySoldInfoArray.Add(CommoditySoldInfo);
			}

		}

		Rs.Close();

		bRet = TRUE;
	}

	return bRet;
}