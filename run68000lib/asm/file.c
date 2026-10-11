// from <stdint.h>
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long size_t;

// from <string.h>
int
memcmp(const void* str1, const void* str2, size_t count)
{
    register const unsigned char* s1 = (const unsigned char*)str1;
    register const unsigned char* s2 = (const unsigned char*)str2;

    while (count-- > 0)
    {
        if (*s1++ != *s2++)
            return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}


#define SECTOR_SIZE 512

// ------------------------------------------------------------
//  read_sector(lba, buffer) doit être fournie par ton émulateur
// ------------------------------------------------------------
extern void read_sector(uint32_t lba, uint8_t* buffer);

// Structure BPB minimale FAT12
typedef struct {
    uint16_t bytesPerSector;     // offset 11
    uint8_t  sectorsPerCluster;  // offset 13
    uint16_t reservedSectors;    // offset 14
    uint8_t  numFATs;            // offset 16
    uint16_t rootEntryCount;     // offset 17
    uint16_t sectorsPerFAT;      // offset 22
} BPB;

// ------------------------------------------------------------
//  Lire le BPB depuis le secteur 0
// ------------------------------------------------------------
void read_bpb(BPB* bpb) {
    uint8_t buf[SECTOR_SIZE];
    read_sector(0, buf);

    bpb->bytesPerSector = buf[11] | (buf[12] << 8);
    bpb->sectorsPerCluster = buf[13];
    bpb->reservedSectors = buf[14] | (buf[15] << 8);
    bpb->numFATs = buf[16];
    bpb->rootEntryCount = buf[17] | (buf[18] << 8);
    bpb->sectorsPerFAT = buf[22] | (buf[23] << 8);
}

// ------------------------------------------------------------
//  Convertir un nom "TEST.TXT" en format FAT 8.3
// ------------------------------------------------------------
void format_filename(const char* name, char out[11]) {
    int i = 0;
    // Nom
    while (*name && *name != '.' && i < 8) {
        out[i++] = (*name >= 'a' && *name <= 'z') ? *name - 32 : *name;
        name++;
    }
    while (i < 8) out[i++] = ' ';

    // Extension
    if (*name == '.') name++;
    while (*name && i < 11) {
        out[i++] = (*name >= 'a' && *name <= 'z') ? *name - 32 : *name;
        name++;
    }
    while (i < 11) out[i++] = ' ';
}

// ------------------------------------------------------------
//  Chercher un fichier dans le root directory
//  Retourne le first cluster ou 0 si non trouvé
// ------------------------------------------------------------
uint16_t find_file_in_root(const char* filename) {
    BPB bpb;
    read_bpb(&bpb);

    uint32_t rootDirStart =
        bpb.reservedSectors +
        bpb.numFATs * bpb.sectorsPerFAT;

    uint32_t rootDirSectors =
        (bpb.rootEntryCount * 32 + (bpb.bytesPerSector - 1))
        / bpb.bytesPerSector;

    char fatname[11];
    format_filename(filename, fatname);

    uint8_t sector[SECTOR_SIZE];

    for (uint32_t s = 0; s < rootDirSectors; s++) {
        read_sector(rootDirStart + s, sector);

        for (int i = 0; i < SECTOR_SIZE; i += 32) {
            uint8_t* entry = &sector[i];

            // 0x00 = fin du répertoire
            if (entry[0] == 0x00)
                return 0;

            // 0xE5 = entrée supprimée
            if (entry[0] == 0xE5)
                continue;

            // Comparer le nom
            if (memcmp(entry, fatname, 11) == 0) {
                // First cluster = offset 26
                uint16_t cl = entry[26] | (entry[27] << 8);
                return cl;
            }
        }
    }

    return 0; // non trouvé
}
