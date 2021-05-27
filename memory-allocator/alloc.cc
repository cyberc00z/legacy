

typedef unsigned int u32;
typedef unsigned short u16;

// thifunction is main hero of story
void kalloc_init(u32 memStart, u32 memSize) ;

void *kalloc(u32 size);
void kfree(void* ptr);


struct SlabEntry {
   SlabEntry* next;
};

struct Slab {
   Slab* m_nextSlab;
   SlabEntry* m_freeList;
   u32 m_slabStart;
   u16 m_size;

   void Init(u32 slabStart, u16 size);
   bool Alloc(u32 size, u32& newLoc);
   bool Free(u32 location);
};

void Slab::Init(u32 slabStart, u16 size){
   m_nextSlab = nullptr;
   m_slabStart = slabStart;
   m_size = size;

   map_page(slabStart);
   memset((void*)slabStart, 0, 0x1000);

   u32 numEntries = (0x1000 / size) -1;
   m_freelist = (SlabEntry*)slabStart;
   SlabEntry* current = m_freeList;
   for (u32 i=1; i<numEntries;i++){
        current->next = (SlabEntry*)(slabStart + (i * size));
        current = current->next;
   }
}

bool Slab::Alloc(u32 size, u32& newLoc){
   if (m_size != size || m_freelist == nullptr){
        return false;
   }
   newLoc = (u32)m_freeList;
   m_freeList = m_freelist->next;
   return true;

}

bool Slab::free(u32 location){
   if (location < m_slabStart || location >= m_slabStart + 0x1000){
          return false;
   }
   SlabEntry* newEntry = (SlabEntry*)location;
   newEntry->next = m_freeList;
   m_freeList  = newEntry;
   return true;
}


Slab* g_slabList;
Slab* g_slabMetaData;

static Slab* alloc_slab_meta(u32 slabStart){
   Slab slabMetadata;
   slabMetadata.Init(slabStart, sizeof(Slab), true) ;
   u32 slabLoc;
   bool didAlloc = slabMetadata.Alloc(sizeof(Slab), slabLoc);
   assert(didAlloc && slabStart == slabLoc);

   Slab* newSlabMeta = (Slab*)slabLoc;
   *newSlabMeta = slabMetadata;
   return newSlabMeta;

}

u32 g_memStart;


void* kalloc(u32 size){
   u32 newLoc;
   Slab* slab = g_slabList;
   for (; slab; slab= slab->m_nextSlab){
       if (slab->Alloc(size, newLoc)){
           return (void*)newLoc;
       }
   }
   u32 slabLoc;
   bool didAlloc = g_slabMetaData->Alloc(sizeof(Slab), slabLoc) ;
   if (!didAlloc){
      g_slabMetaData = alloc_slab_meta(g_memStart);
      g_memStart += 0x1000;
      g_slabMetaData->Alloc(sizeof(Slab), slabLoc);
   }
   Slab* newSlab = (Slab*)slabLoc;
   newSlab->Init(g_memStart, size, false);
   g_memStart += 0x1000;
   newSlab->m_nextSlab = g_slabList;
   g_slabList = newSlab;

   newSlab->Alloc(size, newLoc);
   return (void*)newLoc;


}
void kfree(void* ptr){
   if (!ptr){
       return;
   }
   u32 loc = (u32)ptr;
   for (Slab* slab = g_slabList; slab; slab= slab->m_nextSlab){
       if (slab->Free(loc)){
           return;
       }
   }

}
void kalloc_init(u32 memStart, u32 memSize){

    g_memStart = memStart;
    g_slabList = nullptr;
    g_slabMetaData = alloc_slab_meta(g_memStart);
    g_memStart += 0x1000;

}

char g_memory[1024*1024];

#include <stdio.h>
#include <string>
// main function

int main(int argc, char** argv){
    kalloc_init((u32)&g_memory[0], sizeof(g_memory));
    int* bits[64];
    for (int i=0; i<64; ++i){
         bits[i] = (int*)kalloc(sizeof(int));
         *bits[i] = i;
    }
    kfree(bits[10]);
    bits[10] = (int*)kalloc(sizeof(int));
    for (int i=0; i<64; ++i){
        if (bits[i]){
           kfree(bits[i]);
        }
    }
    struct Test{
        int a, b, c, d;
    };
    Test* bits2[20];
    for (int i=0; i<20; ++i){
       bits2[i] = (Test*)kalloc(sizeof(Test));
       *bits2[i] = {4,3,2,1};
    }
    kfree(bits2[8]);
    char* str = (char*)kalloc(20);
    strcpy(str, "4kb allocated!");
    return 0;
}





