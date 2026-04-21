#pragma once
#include "../types.h"
#include <cstring> //memcpy

template <typename IDT, typename T, size_t MAX_NUM>
struct IDSlots {
	static_assert(std::is_same_v<IDT, uint32_t>, "IDT must be uint32_t");
	static_assert(std::is_trivially_copyable_v<T>);
	static_assert(MAX_NUM > 0 && (MAX_NUM & (MAX_NUM - 1)) == 0, "MAX_NUM must be a power of 2");

	T items[MAX_NUM];
	IDT openSlots[MAX_NUM];	// Circular buffer
	size_t openSlotsWriter, openSlotsReader;
	size_t numOpen;
	bool isOpen[MAX_NUM];
	IDT gens[MAX_NUM];
	IDT nextSlot;

	void clear() {
		memset(isOpen, 0, sizeof(isOpen));
		memset(gens, 0, sizeof(gens));
		nextSlot = 0;
		openSlotsWriter = openSlotsReader = 0;
		numOpen = 0;
	}

	IDT create(const T *data) {
		if (nextSlot == MAX_NUM && numOpen == 0) return jaw::INVALID_ID;
		IDT newSlot;
		if (nextSlot < MAX_NUM) {
			newSlot = nextSlot++;
		}
		else {
			newSlot = openSlots[openSlotsReader];
			openSlotsReader = (openSlotsReader + 1) & (MAX_NUM-1);
			numOpen--;
			isOpen[newSlot] = false;
		}

		memcpy(items + newSlot, data, sizeof(T));
		return newSlot + gens[newSlot];
	}

	bool destroy(IDT id) {
		IDT slot = id & (MAX_NUM-1);
		if (slot >= nextSlot || isOpen[slot] || id != slot + gens[slot]) return false;

		openSlots[openSlotsWriter] = slot;
		openSlotsWriter = (openSlotsWriter + 1) & (MAX_NUM-1);
		numOpen++;
		isOpen[slot] = true;
		assert(gens[slot] + MAX_NUM > gens[slot] && "OVERFLOW");
		gens[slot] += MAX_NUM;
		return true;
	}

	T *idtoptr(IDT id) {
		IDT slot = id & (MAX_NUM-1);
		if (slot >= nextSlot || isOpen[slot] || id != slot + gens[slot]) return nullptr;
		
		return items + slot;
	}
};