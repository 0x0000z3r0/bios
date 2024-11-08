#ifndef _R2_H_
#define _R2_H_

struct gdt32 {
	uint16_t size;
	uint32_t offset;
};

struct seg_dsc {
	uint16_t limit0;		// Segment Limit
	uint16_t base0;			// Segment Base Address
	uint8_t  base1;			// Segment Base Address
	uint8_t  type_s_dpl_p;		// Type:4, S:1, DPL:2, P:1
	uint8_t	 limit1_avl_l_db_g; 	// Segment Limit:4, AVL:2, L:1, D/B:1, G:1
	uint8_t	 base2;			// Segment Base Address
};

enum CR0_FLAGS {
	CR0_PE = (1 << 0),	// Protection Enable (32bit)
	CR0_MP = (1 << 1),	// Monitor Processor
	CR0_EM = (1 << 2),	// Emulation
	CR0_TS = (1 << 3),	// Task Switched
	CR0_ET = (1 << 4),	// Extension Type
	CR0_NE = (1 << 5),	// Numeric Error
	CR0_WP = (1 << 16),	// Write Protected
	CR0_AM = (1 << 18),	// Alignment Mask
	CR0_NW = (1 << 29),	// Not Write-through
	CR0_CD = (1 << 30),	// Cache Disable
	CR0_PG = (1 << 31),	// Paging
};

#endif
