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


enum CR4_FLAGS {
	CR4_VME        = (1 << 0),	// Virtual 8086 Mode Extensions
 	CR4_PVI        = (1 << 1),	// Protected-mode Virtual Interrupts
	CR4_TSD        = (1 << 2),	// Time Stamp Disable
	CR4_DE         = (1 << 3),	// Debugging Extensions
	CR4_PSE        = (1 << 4),	// Page Size Extension
	CR4_PAE        = (1 << 5),	// Physical Address Extension
	CR4_MCE        = (1 << 6),	// Machine Check Exception
	CR4_PGE        = (1 << 7),	// Page Global Enabled
	CR4_PCE        = (1 << 8),	// Performance-Monitoring Counter enable
	CR4_OSFXSR     = (1 << 9),	// Operating system support for FXSAVE and FXRSTOR instruction
	CR4_OSXMMEXCPT = (1 << 10),	// Operating System Support for Unmasked SIMD Floating-Point Exceptions
	CR4_UMIP       = (1 << 11),	// User-Mode Instruction Prevention
	CR4_LA57       = (1 << 12),	// 57-bit linear addresses
	CR4_VMXE       = (1 << 13),	// Virtual Machine Extensions Enable
	CR4_SMXE       = (1 << 14),	// Safer Mode Extensions Enable
	CR4_FSGSBASE   = (1 << 16),	// Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE
	CR4_PCIDE      = (1 << 17),	// PCID Enable
	CR4_OSXSAVE    = (1 << 18),	// XSAVE and Processor Extended States Enable
	CR4_SMEP       = (1 << 20),	// Supervisor Mode Execution Protection Enable
	CR4_SMAP       = (1 << 21),	// Supervisor Mode Access Prevention Enable
	CR4_PKE        = (1 << 22),	// Protection Key Enable
	CR4_CET        = (1 << 23),	// Control-flow Enforcement Technology
	CR4_PKS        = (1 << 24),	// Enable Protection Keys for Supervisor-Mode Pages
};

#endif
