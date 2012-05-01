#pragma once

class CCBitmap;
struct CCMatchItemDesc;

CCBitmap* GetItemIconBitmap(CCMatchItemDesc* pItemDesc);
const char* GetItemIconBitmap_Potion(CCMatchItemDesc* pDesc);
const char* GetItemIconBitmap_Trap(CCMatchItemDesc* pDesc);
CCBitmap* GetItemThumbnailBitmap(CCMatchItemDesc* pDesc);
