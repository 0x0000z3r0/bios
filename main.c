#include <stdio.h>
#include <sys/io.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PCI_CFG_ADR 0xCF8
#define PCI_CFG_DAT 0xCFC
#define PCI_CFG_ADR_EN	31
#define PCI_CFG_ADR_RSV	24
#define	PCI_CFG_ADR_BUS	16
#define PCI_CFG_ADR_DEV	11
#define	PCI_CFG_ADR_FUN	8
#define PCI_CFG_ADR_OFF 0
#define PCI_CFG_ADR_EN_MSK 	0x80000000
#define	PCI_CFG_ADR_RSV_MSK	0x7F000000
#define	PCI_CFG_ADR_BUS_MSK	0x00FF0000
#define PCI_CFG_ADR_DEV_MSK	0x0000F800
#define	PCI_CFG_ADR_FUN_MSK	0x00000700
#define	PCI_CFG_ADR_OFF_MSK	0x000000FF

#define PCI_ADR_LPC_BUS	0x00
#define PCI_ADR_LPC_DEV	0x1F
#define	PCI_ADR_LPC_FUN	0x00

#define LPC_RCBA_OFF	0xF0
#define	LPC_RCBA_BASE	14
#define	LPC_RCBA_RSV	1
#define LPC_RCBA_EN	0
#define	LPC_RCBA_BASE_MSK	0xFFFFC000
#define	LPC_RCBA_RSV_MSK	0x00003FFE
#define LPC_RCBA_EN_MSK		0x00000001

#define RCRB_SPIBAR	0x3800
#define RCRB_GCS	0x3410
#define RCRB_GCS_RSV0		13
#define RCRB_GCS_FLRCSSEL	12
#define RCRB_GCS_BBS		10
#define RCRB_GCS_SERM		9
#define RCRB_GCS_RSV1		7
#define RCRB_GCS_FME		6
#define RCRB_GCS_NR		5
#define	RCRB_GCS_AME		4
#define RCRB_GCS_SPS		3
#define RCRB_GCS_RPR		2
#define RCRB_GCS_RSV2		1
#define RCRB_GCS_BILD		0
#define RCRB_GCS_RSV0_MSK	0xFFFFE000
#define RCRB_GCS_FLRCSSEL_MSK	0x00001000
#define RCRB_GCS_BBS_MSK	0x00000C00
#define RCRB_GCS_SERM_MSK	0x00000200
#define RCRB_GCS_RSV1_MSK	0x00000180
#define RCRB_GCS_FME_MSK	0x00000040
#define RCRB_GCS_NR_MSK		0x00000020
#define RCRB_GCS_AME_MSK	0x00000010
#define RCRB_GCS_SPS_MSK	0x00000008
#define RCRB_GCS_RPR_MSK	0x00000004
#define RCRB_GCS_RSV2_MSK	0x00000002
#define RCRB_GCS_BILD_MSK	0x00000001

#define LPC_BIOS_CNTL_OFF 0xDC
#define LPC_BIOS_CNTL_RSV	5
#define LPC_BIOS_CNTL_TSS	4
#define LPC_BIOS_CNTL_SRC	2
#define LPC_BIOS_CNTL_BLE	1
#define LPC_BIOS_CNTL_BIOSWE	0
#define LPC_BIOS_CNTL_RSV_MSK		0xE0
#define LPC_BIOS_CNTL_TSS_MSK		0x10
#define LPC_BIOS_CNTL_SRC_MSK		0x0C
#define LPC_BIOS_CNTL_BLE_MSK		0x02
#define LPC_BIOS_CNTL_BIOSWE_MSK	0x01

#define SPI_BFPR_OFF	0x00
#define SPI_BFPR_RSV0	29
#define SPI_BFPR_PRL	16
#define SPI_BFPR_RSV1	13
#define SPI_BFPR_PRB	0
#define SPI_BFPR_RSV0_MSK	0xE0000000
#define SPI_BFPR_PRL_MSK	0x1DFF0000
#define SPI_BFPR_RSV1_MSK	0x0000E000
#define SPI_BFPR_PRB_MSK	0x00001FFF

#define	SPI_HSFSTS_OFF	0x04
#define SPI_HSFSTS_FLOCKDN	15
#define SPI_HSFSTS_FDV		14
#define SPI_HSFSTS_FDOPSS	13
#define SPI_HSFSTS_RSV		6
#define SPI_HSFSTS_SCIP		5
#define SPI_HSFSTS_BERASE	3
#define SPI_HSFSTS_AEL		2
#define SPI_HSFSTS_FCERR	1
#define SPI_HSFSTS_FDONE	0
#define SPI_HSFSTS_FLOCKDN_MSK	0x8000
#define SPI_HSFSTS_FDV_MSK	0x4000
#define SPI_HSFSTS_FDOPSS_MSK	0x2000
#define SPI_HSFSTS_RSV_MSK	0x1FC0
#define SPI_HSFSTS_SCIP_MSK	0x0020
#define SPI_HSFSTS_BERASE_MSK	0x0018
#define SPI_HSFSTS_AEL_MSK	0x0004
#define SPI_HSFSTS_FCERR_MSK	0x0002
#define SPI_HSFSTS_FDONE_MSK	0x0001

#define	SPI_HSFCTL_OFF	0x06
#define	SPI_FADDR_OFF	0x08
#define	SPI_RSV0_OFF	0x0C	
#define	SPI_FDATA0_OFF	0xF0
#define	SPI_FDATAN_OFF	0x14

#define	SPI_FRACC_OFF	0x50
#define SPI_FRACC_BMWAG	24
#define SPI_FRACC_BMRAG 16
#define SPI_FRACC_BRWA	8
#define SPI_FRACC_BRRA	0
#define SPI_FRACC_BMWAG_MSK	0xFF000000
#define SPI_FRACC_BMRAG_MSK	0x00FF0000
#define SPI_FRACC_BRWA_MSK	0x0000FF00
#define SPI_FRACC_BRRA_MSK	0x000000FF

#define	SPI_FREG0_OFF	0x54
#define	SPI_FREG1_OFF	0x58
#define	SPI_FREG2_OFF	0x5C
#define	SPI_FREG3_OFF	0x60
#define	SPI_FREG4_OFF	0x64
#define	SPI_RSV1_OFF	0x68 // ՀԹ։ ICH10֊ում գրած ա 0x67: Բայց բոլորը 4 բայթ են։ Երևի էլի դոկի սխալ ա։
#define	SPI_FPR0_OFF	0x74
#define	SPI_FPR1_OFF	0x78
#define	SPI_FPR2_OFF	0x7C
#define	SPI_FPR3_OFF	0x80
#define SPI_FPR4_OFF	0x84
#define SPI_RSV2_OFF	0x88
#define SPI_SSFSTS_OFF	0x90
#define SPI_SSFCTL_OFF	0x91
#define SPI_PREOP_OFF	0x94
#define SPI_OPTYPE_OFF	0x96
#define SPI_OPMENU_OFF	0x98
#define SPI_BBAR_OFF	0xA0
// ՀԹ։ բաց տեղ, արժի նայել ինչի դոկում բան գրած չի
#define SPI_FDOC_OFF	0xB0
#define SPI_FDOD_OFF	0xB4
#define SPI_RSV3_OFF	0xB8
#define SPI_AFC_OFF	0xC0 // ՀԹ։ էլի դոկը ասում ա պահած ա իրանց համար RSV3-ում, բայց սա նկարագրած ա:
#define SPI_LVSCC_OFF	0xC4
#define SPI_UVSCC_OFF	0xC8
#define SPI_FPB_OFF	0xD0

uint32_t
pci_adr(uint32_t bus, uint32_t dev, uint32_t fun, uint32_t off)
{
	uint32_t res;
	res = 0;

	res |= (1 << PCI_CFG_ADR_EN);
	res |= (bus << PCI_CFG_ADR_BUS);
	res |= (dev << PCI_CFG_ADR_DEV);
	res |= (fun << PCI_CFG_ADR_FUN);
	res |= (off << PCI_CFG_ADR_OFF);

	return res;
}

void
log_pci_adr(uint32_t adr)
{
	uint32_t en;
	uint32_t rsv;
	uint32_t bus;
	uint32_t dev;
	uint32_t fun;
	uint32_t off;

	en  = (adr & PCI_CFG_ADR_EN_MSK) >> PCI_CFG_ADR_EN;
	rsv = (adr & PCI_CFG_ADR_RSV_MSK) >> PCI_CFG_ADR_RSV;
	bus = (adr & PCI_CFG_ADR_BUS_MSK) >> PCI_CFG_ADR_BUS;
	dev = (adr & PCI_CFG_ADR_DEV_MSK) >> PCI_CFG_ADR_DEV;
	fun = (adr & PCI_CFG_ADR_FUN_MSK) >> PCI_CFG_ADR_FUN;
	off = (adr & PCI_CFG_ADR_OFF_MSK) >> PCI_CFG_ADR_OFF;

	printf("PCI ADR: 0x%08X\n"
		"-> EN:  0x%08X\n"
		"-> RSV: 0x%08X\n"
		"-> BUS: 0x%08X\n"
		"-> DEV: 0x%08X\n"
		"-> FUN: 0x%08X\n"
		"-> OFF: 0x%08X\n",
		adr, en, rsv, bus, dev, fun, off);
}

void
log_lpc_bios_cntl(uint8_t bios_cntl)
{
	uint8_t rsv;
	uint8_t tss;
	uint8_t src;
	uint8_t ble;
	uint8_t bioswe;

	rsv	= bios_cntl & LPC_BIOS_CNTL_RSV_MSK;
	tss	= bios_cntl & LPC_BIOS_CNTL_TSS_MSK;
	src	= bios_cntl & LPC_BIOS_CNTL_SRC_MSK;
	ble	= bios_cntl & LPC_BIOS_CNTL_BLE_MSK;
	bioswe	= bios_cntl & LPC_BIOS_CNTL_BIOSWE_MSK;

	printf("LPC BIOS_CNTL: 0x%02X\n"
		"-> RSV:    0x%02X\n"
		"-> TSS:    0x%02X\n"
		"-> SRC:    0x%02X\n"
		"-> BLE:    0x%02X\n"
		"-> BIOSWE: 0x%02X\n",
		bios_cntl, rsv, tss, src, ble, bioswe);
}

void
log_lpc_rcba(uint32_t rcba)
{
	uint32_t base;
	uint32_t rsv;
	uint32_t en;

	base = rcba & LPC_RCBA_BASE_MSK;
	rsv  = rcba & LPC_RCBA_RSV_MSK;
	en   = rcba & LPC_RCBA_EN_MSK;

	printf("LPC RBCA: 0x%08X\n"
		"-> BASE: 0x%08X\n"
		"-> RSV:  0x%08X\n"
		"-> EN:   0x%08X\n",
		rcba, base, rsv, en);
}

void
log_spi_bfpr(uint32_t bfpr)
{
	uint32_t rsv0;
	uint32_t prl;
	uint32_t rsv1;
	uint32_t prb;

	rsv0 = bfpr & SPI_BFPR_RSV0_MSK;
	prl  = bfpr & SPI_BFPR_PRL_MSK;
	rsv1 = bfpr & SPI_BFPR_RSV1_MSK;
	prb  = bfpr & SPI_BFPR_PRB_MSK;

	printf("SPI BFPR: 0x%08X\n"
		"-> RSV0: 0x%08X\n"
		"-> PRL:  0x%08X\n"
		"-> RSV1: 0x%08X\n"
		"-> PRB:  0x%08X\n",
		bfpr, rsv0, prl, rsv1, prb);
}

void
log_spi_hsfsts(uint16_t hsfsts)
{
	uint16_t flockdn;
	uint16_t fdv;
	uint16_t fdopss;
	uint16_t rsv;
	uint16_t scip;
	uint16_t berase;
	uint16_t ael;
	uint16_t fcerr;
	uint16_t fdone;

	flockdn = hsfsts & SPI_HSFSTS_FLOCKDN_MSK;
	fdv     = hsfsts & SPI_HSFSTS_FDV_MSK;
	fdopss  = hsfsts & SPI_HSFSTS_FDOPSS_MSK;
	rsv     = hsfsts & SPI_HSFSTS_RSV_MSK;
	scip    = hsfsts & SPI_HSFSTS_SCIP_MSK;
	berase  = hsfsts & SPI_HSFSTS_BERASE_MSK;
	ael     = hsfsts & SPI_HSFSTS_AEL_MSK;
	fcerr   = hsfsts & SPI_HSFSTS_FCERR_MSK;
	fdone   = hsfsts & SPI_HSFSTS_FDONE_MSK;

	printf("SPI HSFSTS: 0x%04X\n"
		"-> FLOCKDN: 0x%04X\n"
		"-> FDV:     0x%04X\n"
		"-> FDOPSS:  0x%04X\n"
		"-> RSV:     0x%04X\n"
		"-> SCIP:    0x%04X\n"
		"-> BERASE:  0x%04X\n"
		"-> AEL:     0x%04X\n"
		"-> FCERR:   0x%04X\n"
		"-> FDONE:   0x%04X\n",
		hsfsts, flockdn, fdv, fdopss, rsv, scip, berase, ael, fcerr, fdone);
}

void
log_spi_fracc(uint32_t fracc)
{
	uint32_t bmwag;
	uint32_t bmrag;
	uint32_t brwa;
	uint32_t brra;

	bmwag = fracc & SPI_FRACC_BMWAG_MSK;
	bmrag = fracc & SPI_FRACC_BMRAG_MSK;
	brwa  = fracc & SPI_FRACC_BRWA_MSK;
	brra  = fracc & SPI_FRACC_BRRA_MSK;

	printf("SPI FRACC: 0x%08X\n"
		"-> BMWAG: 0x%08X\n"
		"-> BMRAG: 0x%08X\n"
		"-> BRWA:  0x%08X\n"
		"-> BRRA:  0x%08X\n",
		fracc, bmwag, bmrag, brwa, brra);
}

void
log_rcrb_gcs(uint32_t gcs)
{
	uint32_t rsv0;
	uint32_t flrcssel;
	uint32_t bbs;
	uint32_t serm;
	uint32_t rsv1;
	uint32_t fme;
	uint32_t nr;
	uint32_t ame;
	uint32_t sps;
	uint32_t rpr;
	uint32_t rsv2;
	uint32_t bild;

	rsv0     = gcs & RCRB_GCS_RSV0_MSK;
	flrcssel = gcs & RCRB_GCS_FLRCSSEL_MSK;
	bbs      = gcs & RCRB_GCS_BBS_MSK;
	serm     = gcs & RCRB_GCS_SERM_MSK;
	rsv1     = gcs & RCRB_GCS_RSV1_MSK;
	fme      = gcs & RCRB_GCS_FME_MSK;
	nr       = gcs & RCRB_GCS_NR_MSK;
	ame      = gcs & RCRB_GCS_AME_MSK;
	sps      = gcs & RCRB_GCS_SPS_MSK;
	rpr      = gcs & RCRB_GCS_RPR_MSK;
	rsv2     = gcs & RCRB_GCS_RSV2_MSK;
	bild     = gcs & RCRB_GCS_BILD_MSK;
	
	printf("GCS: 0x%08X\n"
		"-> RSV0:     0x%08X\n"
		"-> FLRCSSEL: 0x%08X\n"
		"-> BBS:      0x%08X\n"
		"-> SERM:     0x%08X\n"
		"-> RSV1:     0x%08X\n"
		"-> FME:      0x%08X\n"
		"-> NR:       0x%08X\n"
		"-> AME:      0x%08X\n"
		"-> SPS:      0x%08X\n"
		"-> RPR:      0x%08X\n"
		"-> RSV2:     0x%08X\n"
		"-> BILD:     0x%08X\n",
		gcs, rsv0, flrcssel, bbs, serm, rsv1,
		fme, nr, ame, sps, rpr, rsv2, bild);
}

int
main(void)
{
	printf("START\n");

	int res;
	res = iopl(3);
	if (res != 0) {
		perror("IOPL");
		goto _EXIT;
	}	

	// BIOS_CNTL
	uint8_t lpc_bios_cntl;
	{
		uint32_t lpc_adr;
		lpc_adr = pci_adr(PCI_ADR_LPC_BUS, PCI_ADR_LPC_DEV, PCI_ADR_LPC_FUN, LPC_BIOS_CNTL_OFF);

		printf("LPC Interface Bridge Address, OFFSET BIOS_CNTL\n");
		log_pci_adr(lpc_adr);
		
		outl(lpc_adr, PCI_CFG_ADR);

		uint32_t dat;
		dat = inl(PCI_CFG_DAT);

		lpc_bios_cntl = dat & 0xFF;
		log_lpc_bios_cntl(lpc_bios_cntl);

	}
	// RCBA
	uint32_t lpc_rcba;
	{
		uint32_t lpc_adr;
		lpc_adr = pci_adr(PCI_ADR_LPC_BUS, PCI_ADR_LPC_DEV, PCI_ADR_LPC_FUN, LPC_RCBA_OFF);
		printf("LPC Interface Bridge Address, OFFSET RCBA\n");
		log_pci_adr(lpc_adr);
		outl(lpc_adr, PCI_CFG_ADR);

		lpc_rcba = inl(PCI_CFG_DAT);
		log_lpc_rcba(lpc_rcba);
	}

	if (!(lpc_rcba & LPC_RCBA_EN_MSK)) {
		printf("LPC RCBA EN is disabled, trying to set\n");

		uint32_t lpc_adr;
		lpc_adr = pci_adr(PCI_ADR_LPC_BUS, PCI_ADR_LPC_DEV, PCI_ADR_LPC_FUN, LPC_RCBA_OFF);
		outl(lpc_adr, PCI_CFG_ADR);
		lpc_rcba |= (1 << LPC_RCBA_EN);
		printf("RCBA COPY in CPU\n");
		log_lpc_rcba(lpc_rcba);
		outl(lpc_rcba, PCI_CFG_DAT);

		outl(lpc_adr, PCI_CFG_ADR);
		lpc_rcba = inl(PCI_CFG_DAT);

		log_lpc_rcba(lpc_rcba);
		if (!(lpc_rcba & LPC_RCBA_EN_MSK)) {
			printf("FAILED to update the LPC RCBA register\n");
			goto _EXIT;
		}
	}

	int mem;
	mem = open("/dev/mem", O_RDWR);
	if (mem == -1) {
		perror("OPEN");
		goto _EXIT;
	}

	uint32_t rcba_base;
	rcba_base = lpc_rcba & LPC_RCBA_BASE_MSK;

	const size_t rcrb_size = 0x4000;

	void *rcrb;
	rcrb = mmap(NULL, rcrb_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem, rcba_base);
	if (rcrb == MAP_FAILED) {
		perror("MMAP");
		goto _CLOSE_MEM;
	}
	printf("RCRB, PHY: 0x%08X, VIRT: 0x%016lX\n", rcba_base, (uintptr_t)rcrb);

	uint32_t rcrb_gcs;
	// GCS
	{
		rcrb_gcs = *(uint32_t*)((uint8_t*)rcrb + RCRB_GCS);
		log_rcrb_gcs(rcrb_gcs);
	}

	uint32_t rcrb_gcs_bss;
	rcrb_gcs_bss = (rcrb_gcs & RCRB_GCS_BBS_MSK) >> RCRB_GCS_BBS;
	static const char *bios_place[] = { "SPI", "SPI", "PCI", "LPC" };
	printf("BIOS is in %s\n", bios_place[rcrb_gcs_bss]);

	if (rcrb_gcs_bss & 0b10) {
		printf("BIOS is not in the SPI flash, changing the interface\n");
		if (rcrb_gcs & RCRB_GCS_BILD) {
			printf("RCRB GCS BILD is enabled, can not modify the BBS\n");
			goto _MUNMAP_RCRB;
		}

		uint32_t rcrb_gcs_new;
		rcrb_gcs_new = rcrb_gcs & ~(1 << (RCRB_GCS_BBS + 1));
		log_rcrb_gcs(rcrb_gcs_new);
		*(uint32_t*)((uint8_t*)rcrb + RCRB_GCS) = rcrb_gcs_new;

		printf("RCRB GCS BBS was modified\n");
	}

	void *spirb;
	spirb = rcrb + RCRB_SPIBAR;
	printf("SPIBAR: 0x%08X, SPIRB: 0x%016lX\n", RCRB_SPIBAR, (uintptr_t)spirb);

	uint32_t spi_bfpr;
	// BFPR
	{
		spi_bfpr = *(uint32_t*)((uint8_t*)spirb + SPI_BFPR_OFF);
		log_spi_bfpr(spi_bfpr);
	}

	uint16_t spi_hsfsts;
	// HSFS
	{
		spi_hsfsts = *(uint16_t*)((uint8_t*)spirb + SPI_HSFSTS_OFF);
		log_spi_hsfsts(spi_hsfsts);
	}

	uint32_t spi_fracc;
	// FRAP
	{
		spi_fracc = *(uint32_t*)((uint8_t*)spirb + SPI_FRACC_OFF);
		log_spi_fracc(spi_fracc);
	}

	if (spi_hsfsts & SPI_HSFSTS_FDV_MSK) {
		printf("DESCRIPTOR MODE, using ICH10\n");
	} else {
		printf("DESCRIPTOR MODE, falling back to ICH7\n");
	}

	if (spi_hsfsts & SPI_HSFSTS_FLOCKDN_MSK) {
		printf("Registers are locked with FLOCKDN, only ME and Reboot can reset them\n");
	} else {
		printf("Not protected by SPI HSFSTS FLOCKDN\n");
		printf("FRACC modifications in progress\n");

		uint32_t spi_fracc_new;
		spi_fracc_new = 0xFFFFFFFF;

		*(uint32_t*)((uint8_t*)spirb + SPI_FRACC_OFF) = spi_fracc_new;

		printf("Modified the SPI FRACC\n");
	}


_MUNMAP_RCRB:
	munmap(rcrb, rcrb_size);
_CLOSE_MEM:
	close(mem);
_EXIT:
	printf("END\n");
	return 0;
}
