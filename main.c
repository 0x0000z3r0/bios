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

#define SPI_BFPR_OFF	0x00
#define	SPI_HSFSTS_OFF	0x04
#define	SPI_HSFCTL_OFF	0x06
#define	SPI_FADDR_OFF	0x08
#define	SPI_RSV0_OFF	0x0C	
#define	SPI_FDATA0_OFF	0xF0
#define	SPI_FDATAN_OFF	0x14
#define	SPI_FRACC_OFF	0x50
#define	SPI_FREG0_OFF	0x54
#define	SPI_FREG1_OFF	0x58
#define	SPI_FREG2_OFF	0x5C
#define	SPI_FREG3_OFF	0x60
#define	SPI_FREG4_OFF	0x64
#define	SPI_RSV1_OFF	0x68 // ՀՏ։ ICH10֊ում գրած ա 0x67: Բայց բոլորը 4 բայթ են։ Երևի էլի դոկի սխալ ա։
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
// ՀՏ։ բաց տեղ, արժի նայել ինչի դոկում բան գրած չի
#define SPI_FDOC_OFF	0xB0
#define SPI_FDOD_OFF	0xB4
#define SPI_RSV3_OFF	0xB8
#define SPI_AFC_OFF	0xC0 // ՀՏ։ էլի դոկը ասում ա պահած ա իրանց համար RSV3-ում, բայց սա նկարագրած ա:
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

	uint32_t lpc_adr;
	lpc_adr = pci_adr(PCI_ADR_LPC_BUS, PCI_ADR_LPC_DEV, PCI_ADR_LPC_FUN, LPC_RCBA_OFF);

	printf("LPC Interface Bridge Address\n");
	log_pci_adr(lpc_adr);
	
	outl(lpc_adr, PCI_CFG_ADR);

	uint32_t lpc_rcba;
	lpc_rcba = inl(PCI_CFG_DAT);
	log_lpc_rcba(lpc_rcba);

	if (!(lpc_rcba & LPC_RCBA_EN_MSK)) {
		printf("LPC RCBA EN is disabled, trying to set\n");

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

	const size_t spibar = 0x3800;

	void *spirb;
	spirb = rcrb + spibar;
	printf("SPIBAR: 0x%08lX, SPIRB: 0x%016lX\n", spibar, (uintptr_t)spirb);

	

_MUNMAP_RCRB:
	munmap(rcrb, rcrb_size);
_CLOSE_MEM:
	close(mem);
_EXIT:
	printf("END\n");
	return 0;
}
