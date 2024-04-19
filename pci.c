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
} __attribute__((packed));

// Great, another manual error. The description says its 16bit but summary says its 32bit
union spi_addr {
	struct {
		uint32_t rsv0:1;
		uint32_t sre:1;
		uint32_t rsv1:1;
		uint32_t rsv2:2;
		uint32_t sba:27;
	};
	uint32_t val;
};

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

	static const uint8_t pci_lpc_dev = 0x14;
	static const uint8_t pci_lpc_fun = 3;

	uint32_t data;

	union pci_cfg_addr addr;
	addr.bus  = 0;
	addr.dev  = pci_lpc_dev;
	addr.func = pci_lpc_fun;
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

	addr.off = 0xA0;
	printf("trying to get the spi base address, offset: %02x\n", addr.off);

	union spi_addr spi_addr;
	spi_addr.val = pci_cfg_get(addr);
	printf("spi base address, enabled: %u, address: %08x\n", spi_addr.sre, spi_addr.sba);

	int mem;
	mem = open("/dev/mem", O_RDWR);
	if (res == -1) {
		printf("failed to open physical memory, err: %s\n", strerror(errno));
		return 1;
	}

	size_t size;
	size = 0x4000;

	off_t off;
	off = (off_t)spi_addr.sba << 5;
	printf("trying to map physical memory, size: %zu, offset: %08lx\n", size, off);

	void *sba;
	sba = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem, off);
	if (sba == MAP_FAILED) {
		printf("failed to map physical membory, err: %s\n", strerror(errno));
		return 1;
	}

	printf("root complex register block starts at %p\n", sba);

	for (size_t i = 1; i <= 32; ++i) {
		printf("%02x", *((unsigned char*)sba + i - 1));
		if (i % 8 == 0)
			printf("\n");
		else if (i % 4 == 0)
			printf(" ");
	}

	munmap(sba, size);
	close(mem);
	return 0;
}
