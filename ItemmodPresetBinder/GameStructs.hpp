#pragma once
#include "bakkesmod/wrappers/includes.h"


class FString
{
	const wchar_t*		ArrayData;
	int32_t				ArrayCount;
	int32_t				ArrayMax;

public:
	inline UnrealStringWrapper to_wrapper() const
	{
		return reinterpret_cast<uintptr_t>(this);
	}
};


template<typename T>
class TArray
{
    T* Data = nullptr;
    int32_t Size = 0;
    int32_t Capacity = 0;

public:
    T& operator[](int32_t index)
    {
        return Data[index];
    }

    const T& operator[](int32_t index) const
    {
        return Data[index];
    }

    int32_t size() const
    {
        return Size;
    }

    bool empty() const
    {
        return Size == 0;
    }
};


// structs
struct FGFxLoadoutSet
{
	FString				Name;
	int32_t				Index;
	uint32_t			bEquipped : 1;
};

struct UGFxData_LoadoutSets_TA
{
	uint8_t                     padding[0x98];
	TArray<FGFxLoadoutSet>      LoadoutSetsArray;
};

struct ULoadoutSet_TA
{
    uint8_t                     padding[0x60];
    FString                     LoadoutSetName;
};

struct UProfileLoadoutSave_TA
{
    uint8_t                     padding[0xC8];
    TArray<ULoadoutSet_TA*>     LoadoutSets;
    ULoadoutSet_TA*             EquippedLoadoutSet;
};


// Function TAGame.GFxData_LoadoutSets_TA.EquipPreset
struct EquipPreset_params
{
	int32_t				Index;
};

// Function TAGame.GFxData_LoadoutSets_TA.DeletePreset
struct DeletePreset_params
{
	int32_t				Index;
};

// Function TAGame.ProfileLoadoutSave_TA.GetLoadoutSet
struct GetLoadoutSet_params
{
    int32_t             SetIndex;
    ULoadoutSet_TA*     ReturnValue;
};