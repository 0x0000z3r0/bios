#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#define off_of(cont, field) (size_t)(&(((cont*)0)->field))
#define cnt_of(arr) (sizeof (arr) / sizeof (arr[0]))

// ICH7-M datasheet: 
// https://www.intel.com/content/dam/doc/datasheet/i-o-controller-hub-7-datasheet.pdf

union pci_cfg_addr {
	struct {
		uint8_t off;
		uint8_t func:3;
		uint8_t dev:5;
		uint8_t bus;
		uint8_t rsv:7;
		uint8_t en:1;
	};
	uint32_t val;
};

struct pci_cfg_hdr {
	uint16_t vid;
	uint16_t did;
	union {
		struct {
			uint8_t iose:1;
			uint8_t mse:1;
			uint8_t bme:1;
			uint8_t sce:1;
			uint8_t mwie:1;
			uint8_t vps:1;
			uint8_t pere:1;
			uint8_t wcc:1;
			uint8_t serr:1;
			uint8_t fbe:1;
			uint8_t rsv:6;

		} cmd;
		uint16_t cmd_val;
	};
	union {
		struct {
			uint8_t rsv:3;
			uint8_t is:1;
			uint8_t clist:1;
			uint8_t c66mhz:1;
			uint8_t fbc:1;
			uint8_t dped:1;
			uint8_t dsel:2;
			uint8_t sta:1;
			uint8_t rta:1;
			uint8_t rma:1;
			uint8_t sse:1;
			uint8_t dpe:1;
		} sts;
		uint16_t sts_val;
	};
	uint8_t  rid; 
	uint8_t  pi; 
	uint8_t  scc; 
	uint8_t  bcc; 
	uint8_t  udf0;
	union {
		struct {
			uint8_t rsv:2;
			uint8_t mlc:6;
		} plt;
		uint8_t plt_val;
	};
	union {
		struct {
			uint8_t typ:7;
			uint8_t mfd:1;
		} typ;
		uint8_t typ_val;
	};
	uint8_t  udf1[29];
	uint32_t ss; 
	uint8_t  udf2[4];
	uint8_t  capp; 
	uint8_t  udf3[11];
	uint32_t pmbase;
	uint8_t  acpi_cntl;
	uint8_t  udf4[3];
	uint32_t gpiobase;
	uint8_t  gc;
	uint8_t  udf5[19];
	uint8_t  pirq_rout[4];
	uint8_t  sirq_cntl;
	uint8_t  udf6[3];
	uint8_t  pirq_rout2[4];
	uint8_t  udf7[20];
	uint8_t  lpc_io_dec;
	uint8_t  udf8;
	uint16_t lpc_en;
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;
	uint8_t  udf9[12];
	uint8_t  pow_mgm[48];
	uint32_t fwh_sel1;
	uint32_t fwh_sel2;
	uint16_t fwh_dec_en1;
	uint8_t  udf10[2];
	union {
		struct {
			uint8_t bioswe:1;
			uint8_t ble:1;
			uint8_t src:2;
			uint8_t tss:1;
			uint8_t rsv:3;
		} bios_cntl;
		uint8_t bios_cntl_val;
	};
	uint8_t  udf11[3];
	uint16_t fdcap;
	uint8_t  fdlen;
	uint8_t  fdver;
	uint8_t  fdvct[8];
	uint8_t  udf12[4];
	union {
		struct {
			uint32_t en:1;
			uint32_t rsv:13;
			uint32_t ba:18;
		} rcba;
		uint32_t rcba_val;
	};
} __attribute__((packed));

struct spi_map {
	uint16_t spis;
	uint16_t spic;
	uint32_t spia;
	uint8_t  spid0[8];
	uint8_t  spid1[8];
	uint8_t  spid2[8];
	uint8_t  spid3[8];
	uint8_t  spid4[8];
	uint8_t  spid5[8];
	uint8_t  spid6[8];
	uint8_t  spid7[8];
	uint32_t bbar;
	uint16_t preop;
	uint16_t optype;
	uint64_t opmenu;
	uint32_t pbr0;
	uint32_t pbr1;
	uint32_t pbr2;
	uint8_t  rsv[4];
} __attribute__((packed));

static uint32_t
pci_cfg_get(union pci_cfg_addr cfg_addr)
{
	cfg_addr.en  = 1;
	cfg_addr.rsv = 0;

	// should be aligned
	cfg_addr.off = cfg_addr.off & 0xFC;

	printf("pci configuration address: %08x\n", cfg_addr.val);

	static const uint16_t pci_cfg_port_addr = 0xCF8;
	outl(cfg_addr.val, pci_cfg_port_addr);

	static const uint16_t pci_cfg_port_data = 0xCFC;

	uint32_t cfg_data;
	cfg_data = inl(pci_cfg_port_data);

	printf("pci configuration data: %08x\n", cfg_data);

	return cfg_data;
}

int
main(void)
{
	int res;
	res = iopl(3);
	if (res == -1) {
		printf("failed to lower IPOL flag, err: %s\n", strerror(errno));
		return 1;
	}

	static const uint8_t pci_lpc_dev = 0x1F;

	uint32_t data;

	union pci_cfg_addr addr;
	addr.bus  = 0;
	addr.dev  = pci_lpc_dev;
	addr.func = 0;
	addr.off  = 0;
	
	printf("trying to find the pci device: %02x\n", pci_lpc_dev);

	data = pci_cfg_get(addr);

	struct pci_cfg_hdr hdr;
	hdr.vid = data & 0xFFFF;
	hdr.did = data >> 16;

	if (hdr.vid == 0xFFFF) {
		printf("invalid vendor id\n");
		return 1;
	}

	printf("vendor id: %04x, device id: %04x\n", hdr.vid, hdr.did);

	addr.off = off_of(struct pci_cfg_hdr, bios_cntl);
	printf("trying to get the bios control register, offset: %02x\n", addr.off);

	hdr.bios_cntl_val = pci_cfg_get(addr);
	printf("bioc_cntl values, top swap status: %u, spi read config: %u, bios block enable: %u, bios write enable: %u\n", hdr.bios_cntl.tss, hdr.bios_cntl.src, hdr.bios_cntl.ble, hdr.bios_cntl.bioswe);

	addr.off = off_of(struct pci_cfg_hdr, rcba);
	printf("trying to get the root complex base address, offset: %02x\n", addr.off);

	hdr.rcba_val = pci_cfg_get(addr);
	printf("rcba values, enable: %u, base address: %08x\n", hdr.rcba.en, hdr.rcba.ba);

	if (!hdr.rcba.en) {
		printf("rcba is disabled\n");
		return 1;
	}

	int mem;
	mem = open("/dev/mem", O_RDWR);
	if (res == -1) {
		printf("failed to open physical memory, err: %s\n", strerror(errno));
		return 1;
	}

	size_t size;
	size = 0x4000;

	off_t off;
	off = (off_t)hdr.rcba.ba << 14;
	printf("trying to map physical memory, size: %zu, offset: %08lx\n", size, off);

	void *rcrb;
	rcrb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem, off);
	if (rcrb == MAP_FAILED) {
		printf("failed to map physical membory, err: %s\n", strerror(errno));
		return 1;
	}

	printf("root complex register block starts at %p\n", rcrb);

	uint32_t gcs_reg;
	gcs_reg = *(uint32_t*)(rcrb + 0x3410);

	printf("general control and status register, value: %08x, boot bios straps: %02x\n", gcs_reg, (gcs_reg & (0x3 << 11)) >> 11);

	void *spibar;
	spibar = rcrb + 0x3020;

	printf("spi base address starts at %p\n", spibar);

	for (size_t i = 0; i < sizeof (struct spi_map); ++i) {
		printf("%02x", *((unsigned char*)spibar + i));
		if (i % 8 == 0)
			printf("\n");
		else if (i % 4 == 0)
			printf(" ");
	}

	struct spi_map *map;
	map = (struct spi_map*)spibar;

	munmap(rcrb, size);
	close(mem);
	return 0;
}
