#pragma once
#include "stdafx.h"

enum class EEngineShowFlags : uint64
{
	SF_Primitives,
	SF_BillboardText,
	SF_None
};

extern EEngineShowFlags ShowPrimitives;
extern EEngineShowFlags ShowBillboard;