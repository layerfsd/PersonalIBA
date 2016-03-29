#pragma once

#define G2XPropertyClass         // 说明声明的类具有属性

#define G2XPropertyReadOnly(name,type) \
	private:\
	type name;\
	public: const type Get##name()\
	{\
	return name;\
	}
#define G2XProperty(name,type) \
	private:\
	type name;\
	public: const type Get##name()\
	{\
	return name;\
	}\
	public: void Set##name(const type& newVar )\
	{\
	name = newVar;\
	}\
	public: type& Set##name()\
	{\
	return name;\
	}
#define G2XPropertyInit(name,value) \
	name = value;
#define G2XPropertyInit2(P1,P2,V1,V2) \
	G2XPropertyInit(P1,V1);\
	G2XPropertyInit(P2,V2);
#define G2XPropertyInit3(P1,P2,P3,V1,V2,V3) \
	G2XPropertyInit(P1,V1);\
	G2XPropertyInit2(P2,P3,V2,V3);
#define GPropertyInit4(P1,P2,P3,P4,V1,V2,V3,V4) \
	G2XPropertyInit(P1,V1);\
	G2XPropertyInit3(P2,P3,P4,V2,V3,V4);

#define G2X_GET(name) \
	Get##name()
#define G2X_SET(name) \
	Set##name()