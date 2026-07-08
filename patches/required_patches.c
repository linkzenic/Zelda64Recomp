#include "patches.h"
#include "misc_funcs.h"
#include "transform_ids.h"
#include "loadfragment.h"
#include "libc/math.h"
#include "input.h"

void Main_ClearMemory(void* begin, void* end);
void Main_InitMemory(void);
void Main_InitScreen(void);
void Fault_AddHungupAndCrash(const char* file, s32 line);

extern u16 sNumDmaEntries;


RECOMP_DECLARE_EVENT(recomp_on_init());

#if defined(ZELDA_ANDROID_BUILTIN_PMM)
void zelda_builtin_pmm_after_main_init(void);
#endif

// @recomp Patched to load the code segment in the recomp runtime.
RECOMP_PATCH void Main_Init(void) {
    DmaRequest dmaReq;
    OSMesgQueue mq;
    OSMesg msg[1];
    size_t prevSize;
    s32 syncBootDma;

    // @recomp Register base actor extensions.
    recomp_printf("[MainInitDiag] begin\n");
    recomp_printf("[MainInitDiag] register_base_actor_extensions begin\n");
    register_base_actor_extensions();
    recomp_printf("[MainInitDiag] register_base_actor_extensions end\n");

    // @recomp_event recomp_on_init(): Allow mods to initialize themselves once.
    recomp_printf("[MainInitDiag] recomp_on_init begin\n");
    recomp_on_init();
    recomp_printf("[MainInitDiag] recomp_on_init end\n");

    syncBootDma = recomp_android_should_use_sync_boot_dma();

    if (!syncBootDma) {
        recomp_printf("[MainInitDiag] osCreateMesgQueue begin\n");
        osCreateMesgQueue(&mq, msg, ARRAY_COUNT(msg));
        recomp_printf("[MainInitDiag] osCreateMesgQueue end\n");
    }

    prevSize = gDmaMgrDmaBuffSize;
    gDmaMgrDmaBuffSize = 0;

    // @recomp Load the code segment in the recomp runtime.
    recomp_printf("[MainInitDiag] recomp_load_overlays begin\n");
    recomp_load_overlays(SEGMENT_ROM_START(code), SEGMENT_START(code), SEGMENT_ROM_END(code) - SEGMENT_ROM_START(code));
    recomp_printf("[MainInitDiag] recomp_load_overlays end\n");

    if (syncBootDma) {
        // @recomp Samsung devices have shown problems with this boot-time DMA
        // request going through the DMA manager queue. Keep that workaround
        // scoped to Samsung so normal Android devices retain the original path.
        recomp_printf("[MainInitDiag] DmaMgr_DmaRomToRam begin\n");
        DmaMgr_DmaRomToRam(SEGMENT_ROM_START(code), SEGMENT_START(code),
                           SEGMENT_ROM_END(code) - SEGMENT_ROM_START(code));
        recomp_printf("[MainInitDiag] DmaMgr_DmaRomToRam end\n");
    } else {
        recomp_printf("[MainInitDiag] DmaMgr_SendRequestImpl begin\n");
        DmaMgr_SendRequestImpl(&dmaReq, SEGMENT_START(code), SEGMENT_ROM_START(code),
                               SEGMENT_ROM_END(code) - SEGMENT_ROM_START(code), 0, &mq, NULL);
        recomp_printf("[MainInitDiag] DmaMgr_SendRequestImpl end\n");
    }
    recomp_printf("[MainInitDiag] Main_InitScreen begin\n");
    Main_InitScreen();
    recomp_printf("[MainInitDiag] Main_InitScreen end\n");
    recomp_printf("[MainInitDiag] Main_InitMemory begin\n");
    Main_InitMemory();
    recomp_printf("[MainInitDiag] Main_InitMemory end\n");
    if (!syncBootDma) {
        recomp_printf("[MainInitDiag] osRecvMesg begin\n");
        osRecvMesg(&mq, NULL, OS_MESG_BLOCK);
        recomp_printf("[MainInitDiag] osRecvMesg end\n");
    }

    gDmaMgrDmaBuffSize = prevSize;

    recomp_printf("[MainInitDiag] Main_ClearMemory begin\n");
    Main_ClearMemory(SEGMENT_BSS_START(code), SEGMENT_BSS_END(code));
    recomp_printf("[MainInitDiag] Main_ClearMemory end\n");
    
    // @recomp Patch a float that's used to render the clock into the correct value.
    // This is done this way instead of patching the function to avoid conflicts with mods that need to patch the function.
    // The original code is `Matrix_RotateZF(-(timeInSeconds * 0.0175f) / 10.0f, MTXMODE_APPLY);`, where 0.0175f is being used
    // to convert degrees to radians. However, the correct value is PI/180 which is approximately 0.0174533f, and the difference is enough
    // to cause the clock to overshoot when reaching an hour mark.
    recomp_printf("[MainInitDiag] clock float patch begin\n");
    *(f32*)0x801DDBBC = ((f32)M_PI) / 180.0f;
    recomp_printf("[MainInitDiag] clock float patch end\n");
#if defined(ZELDA_ANDROID_BUILTIN_PMM)
    if (!recomp_android_is_n64_mode()) {
        zelda_builtin_pmm_after_main_init();
    }
#endif
    recomp_printf("[MainInitDiag] end\n");
}

void Overlay_Relocate(void* allocatedRamAddr, OverlayRelocationSection* ovlRelocs, uintptr_t vramStart);

static void AndroidDiag_LoadOverlayToRam(uintptr_t vromStart, void* dst, size_t size) {
    DmaEntry* entry = DmaMgr_FindDmaEntry(vromStart);
    s32 yaz0Status;

    recomp_measure_latency(97, 0x70, (u32)vromStart, (u32)(uintptr_t)dst, (u32)size);

    if (entry == NULL) {
        recomp_measure_latency(97, 0x71, (u32)vromStart, (u32)(uintptr_t)dst, (u32)size);
        recomp_load_overlays(vromStart, dst, size);
        return;
    }

    recomp_measure_latency(97, 0x72, (u32)entry->vromStart, (u32)entry->vromEnd, (u32)entry->romStart);
    recomp_measure_latency(97, 0x73, (u32)entry->romStart, (u32)entry->romEnd, (u32)size);

    if (entry->romEnd == 0) {
        recomp_load_overlays((entry->romStart + vromStart) - entry->vromStart, dst, size);
        recomp_measure_latency(97, 0x74, (u32)((entry->romStart + vromStart) - entry->vromStart),
                               (u32)(uintptr_t)dst, (u32)size);
        return;
    }

    if ((vromStart != entry->vromStart) || (size != (entry->vromEnd - entry->vromStart))) {
        recomp_measure_latency(97, 0x75, (u32)vromStart, (u32)entry->vromStart, (u32)size);
        recomp_load_overlays(vromStart, dst, size);
        return;
    }

    yaz0Status = recomp_android_load_yaz0(entry->romStart, entry->romEnd - entry->romStart, dst, size);
    recomp_measure_latency(97, 0x76, (u32)yaz0Status, (u32)entry->romStart, (u32)(uintptr_t)dst);

    if (yaz0Status != 0) {
        recomp_load_overlays(vromStart, dst, size);
        recomp_measure_latency(97, 0x77, (u32)yaz0Status, (u32)vromStart, (u32)size);
    }
}

static void AndroidDiag_LogDmaFault(const char* reason, uintptr_t vrom, void* ram, size_t size, s32 index,
                                    DmaEntry* entry) {
    if (!recomp_android_should_use_sync_boot_dma()) {
        return;
    }

    if (entry != NULL) {
        recomp_printf("[AndroidLoadDiag] DmaMgr fault %s vrom=%08llX ram=%08llX size=%08X index=%d "
                      "entry_vrom=%08llX-%08llX entry_rom=%08llX-%08llX\n",
                      reason, (u64)vrom, (u64)(uintptr_t)ram, (u32)size, index,
                      (u64)entry->vromStart, (u64)entry->vromEnd, (u64)entry->romStart, (u64)entry->romEnd);
    } else {
        recomp_printf("[AndroidLoadDiag] DmaMgr fault %s vrom=%08llX ram=%08llX size=%08X index=%d entry=NULL "
                      "numEntries=%u\n",
                      reason, (u64)vrom, (u64)(uintptr_t)ram, (u32)size, index, (u32)sNumDmaEntries);
    }
}

static s32 AndroidDiag_ShouldIgnoreImplausibleBootDma(uintptr_t vrom, size_t size) {
    if (!recomp_android_should_use_sync_boot_dma()) {
        return false;
    }

    // Samsung's sync boot path can receive one stale/corrupt DMA message during
    // startup. Real MM DMA requests are table-backed and much smaller than this.
    if ((size > 0x10000000) || (vrom >= 0x10000000)) {
        return true;
    }

    return false;
}

// Samsung devices can reach the game entrypoint now, but still fault inside the
// vanilla DMAMGR thread while Yaz0 data is being decompressed. Keep the normal
// path everywhere else, and route only Samsung compressed DMA through the
// host-side Android loader used by the other Samsung-safe asset paths.
RECOMP_PATCH void DmaMgr_ProcessMsg(DmaRequest* req) {
    uintptr_t vrom;
    void* ram;
    size_t size;
    uintptr_t romStart;
    size_t romSize;
    DmaEntry* dmaEntry;
    s32 index;
    s32 yaz0Status;
    s32 syncBootDma = recomp_android_should_use_sync_boot_dma();

    vrom = req->vromAddr;
    ram = req->dramAddr;
    size = req->size;

    index = DmaMgr_FindDmaIndex(vrom);

    if ((index >= 0) && (index < sNumDmaEntries)) {
        dmaEntry = &dmadata[index];
        if (dmaEntry->romEnd == 0) {
            if (dmaEntry->vromEnd < (vrom + size)) {
                AndroidDiag_LogDmaFault("raw-bounds", vrom, ram, size, index, dmaEntry);
                Fault_AddHungupAndCrash("../z_std_dma.c", 499);
            }
            if (syncBootDma) {
                recomp_measure_latency(97, 0x92, (u32)((dmaEntry->romStart + vrom) - dmaEntry->vromStart),
                                       (u32)(uintptr_t)ram, (u32)size);
                AndroidDiag_LoadOverlayToRam(vrom, ram, size);
                recomp_measure_latency(97, 0x93, (u32)vrom, (u32)(uintptr_t)ram, (u32)size);
                return;
            }
            DmaMgr_DmaRomToRam((dmaEntry->romStart + vrom) - dmaEntry->vromStart, (u8*)ram, size);
            return;
        }

        romSize = dmaEntry->romEnd - dmaEntry->romStart;
        romStart = dmaEntry->romStart;

        if (vrom != dmaEntry->vromStart) {
            AndroidDiag_LogDmaFault("compressed-vrom-mismatch", vrom, ram, size, index, dmaEntry);
            Fault_AddHungupAndCrash("../z_std_dma.c", 518);
        }

        if (size != (dmaEntry->vromEnd - dmaEntry->vromStart)) {
            AndroidDiag_LogDmaFault("compressed-size-mismatch", vrom, ram, size, index, dmaEntry);
            Fault_AddHungupAndCrash("../z_std_dma.c", 525);
        }

        if (syncBootDma) {
            recomp_measure_latency(97, 0x90, (u32)romStart, (u32)(uintptr_t)ram, (u32)romSize);
            yaz0Status = recomp_android_load_yaz0(romStart, romSize, ram, size);
            recomp_measure_latency(97, 0x91, (u32)yaz0Status, (u32)romStart, (u32)(uintptr_t)ram);
            if (yaz0Status != 0) {
                recomp_printf("[AndroidLoadDiag] DmaMgr_ProcessMsg host Yaz0 failed status=%d rom=%08llX "
                              "compressed=%08X expected=%08X\n",
                              yaz0Status, (u64)romStart, (u32)romSize, (u32)size);
                AndroidDiag_LogDmaFault("host-yaz0-failed", vrom, ram, size, index, dmaEntry);
                Fault_AddHungupAndCrash("../z_std_dma.c", 545);
            }
        } else {
            Yaz0_Decompress(romStart, ram, romSize);
        }
    } else {
        AndroidDiag_LogDmaFault("index-not-found", vrom, ram, size, index, NULL);
        if (AndroidDiag_ShouldIgnoreImplausibleBootDma(vrom, size)) {
            recomp_printf("[AndroidLoadDiag] ignored implausible Samsung DMA request vrom=%08llX ram=%08llX "
                          "size=%08X\n",
                          (u64)vrom, (u64)(uintptr_t)ram, (u32)size);
            return;
        }
        Fault_AddHungupAndCrash("../z_std_dma.c", 558);
    }
}

// @recomp Patched to load the overlay in the recomp runtime.
RECOMP_PATCH size_t Overlay_Load(uintptr_t vromStart, uintptr_t vromEnd, void* ramStart, void* ramEnd, void* allocatedRamAddr) {
    uintptr_t vramStart = (uintptr_t)ramStart;
    uintptr_t vramEnd = (uintptr_t)ramEnd;
    s32 size = vromEnd - vromStart;
    uintptr_t end;
    OverlayRelocationSection* ovlRelocs;
    s32 syncBootDma = recomp_android_should_use_sync_boot_dma();
    
    // @recomp Load the overlay in the recomp runtime.
    recomp_printf("[OverlayLoadDiag] begin vrom=%08llX-%08llX ram=%08llX-%08llX dst=%08llX size=%08X sync=%d\n",
                  (u64)vromStart, (u64)vromEnd, (u64)vramStart, (u64)vramEnd, (u64)(uintptr_t)allocatedRamAddr,
                  size, syncBootDma);
    recomp_printf("[OverlayLoadDiag] recomp_load_overlays begin\n");
    recomp_load_overlays(vromStart, allocatedRamAddr, vromEnd - vromStart);
    recomp_printf("[OverlayLoadDiag] recomp_load_overlays end\n");

    if (gOverlayLogSeverity >= 3) {}
    if (gOverlayLogSeverity >= 3) {}

    end = (uintptr_t)allocatedRamAddr + size;
    if (syncBootDma) {
        // Samsung devices can fault when actor overlays go through the raw
        // DMA path. Load directly from the ROM mapping instead, preserving the
        // same VROM-to-ROM and Yaz0 handling used by the safer Android loaders.
        recomp_printf("[OverlayLoadDiag] samsung direct load begin\n");
        AndroidDiag_LoadOverlayToRam(vromStart, allocatedRamAddr, size);
        recomp_printf("[OverlayLoadDiag] samsung direct load end\n");
    } else {
        recomp_printf("[OverlayLoadDiag] DmaMgr_SendRequest0 begin\n");
        DmaMgr_SendRequest0(allocatedRamAddr, vromStart, size);
        recomp_printf("[OverlayLoadDiag] DmaMgr_SendRequest0 end\n");
    }

    ovlRelocs = (OverlayRelocationSection*)(end - ((s32*)end)[-1]);
    recomp_printf("[OverlayLoadDiag] reloc offset=%08X relocs=%08llX\n", ((s32*)end)[-1], (u64)(uintptr_t)ovlRelocs);

    if (gOverlayLogSeverity >= 3) {}
    if (gOverlayLogSeverity >= 3) {}

    recomp_printf("[OverlayLoadDiag] Overlay_Relocate begin\n");
    Overlay_Relocate(allocatedRamAddr, ovlRelocs, vramStart);
    recomp_printf("[OverlayLoadDiag] Overlay_Relocate end\n");

    if (ovlRelocs->bssSize != 0) {
        if (gOverlayLogSeverity >= 3) {}
        recomp_printf("[OverlayLoadDiag] bzero begin size=%08X\n", ovlRelocs->bssSize);
        bzero((void*)end, ovlRelocs->bssSize);
        recomp_printf("[OverlayLoadDiag] bzero end\n");
    }

    size = vramEnd - vramStart;

    recomp_printf("[OverlayLoadDiag] cache begin size=%08X\n", size);
    osWritebackDCache(allocatedRamAddr, size);
    osInvalICache(allocatedRamAddr, size);
    recomp_printf("[OverlayLoadDiag] cache end\n");

    if (gOverlayLogSeverity >= 3) {}

    recomp_printf("[OverlayLoadDiag] end size=%08X\n", size);
    return size;
}
