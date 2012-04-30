#pragma once

class MBitmap;
struct CCMatchItemDesc;

MBitmap* GetItemIconBitmap(CCMatchItemDesc* pItemDesc);
const char* GetItemIconBitmap_Potion(CCMatchItemDesc* pDesc);
const char* GetItemIconBitmap_Trap(CCMatchItemDesc* pDesc);
MBitmap* GetItemThumbnailBitmap(CCMatchItemDesc* pDesc);
