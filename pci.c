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

#include "fcha50m.h"

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

	addr.off = 0x50;
	printf("trying to get rom protections, offset: %02x\n", addr.off);

	union rom_prot rom_prot0;
	rom_prot0.val = pci_cfg_get(addr);
	printf("rom protection 0, write protected: %u, read protected: %u, rom offset: %08x, rom base: %08x\n", 
			rom_prot0.write, rom_prot0.read, rom_prot0.offset, (uint32_t)rom_prot0.base << 11);

	addr.off = 0x54;
	printf("trying to get rom protections, offset: %02x\n", addr.off);

	union rom_prot rom_prot1;
	rom_prot1.val = pci_cfg_get(addr);
	printf("rom protection 1, write protected: %u, read protected: %u, rom offset: %08x, rom base: %08x\n", 
			rom_prot1.write, rom_prot1.read, rom_prot1.offset, (uint32_t)rom_prot1.base << 11);
	
	addr.off = 0x58;
	printf("trying to get rom protections, offset: %02x\n", addr.off);

	union rom_prot rom_prot2;
	rom_prot2.val = pci_cfg_get(addr);
	printf("rom protection 2, write protected: %u, read protected: %u, rom offset: %08x, rom base: %08x\n", 
			rom_prot2.write, rom_prot2.read, rom_prot2.offset, (uint32_t)rom_prot2.base << 11);

	addr.off = 0x5C;
	printf("trying to get rom protections, offset: %02x\n", addr.off);

	union rom_prot rom_prot3;
	rom_prot3.val = pci_cfg_get(addr);
	printf("rom protection 3, write protected: %u, read protected: %u, rom offset: %08x, rom base: %08x\n", 
			rom_prot3.write, rom_prot3.read, rom_prot3.offset, (uint32_t)rom_prot3.base << 11);

	int mem;
	mem = open("/dev/mem", O_RDWR);
	if (res == -1) {
		printf("failed to open physical memory, err: %s\n", strerror(errno));
		return 1;
	}

	size_t size;
	size = 0x1000;

	off_t off;
	off = (off_t)spi_addr.sba << 5;
	printf("trying to map physical memory, size: %zu, offset: %08lx\n", size, off);

	void *sba;
	sba = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem, off);
	if (sba == MAP_FAILED) {
		printf("failed to map physical membory, err: %s\n", strerror(errno));
		return 1;
	}

	printf("spi base virtual address starts at %p\n", sba);

	struct spi_rcr *spi_rcr;
	spi_rcr = (struct spi_rcr*)sba;

	printf("spi control register 0: %08x\n"
		"\toperation code:         %02x\n"
		"\tsend byte count:        %02x\n"
		"\treceive byte count:     %02x\n"
		"\texecute code:           %02x\n"
		"\treserved:               %02x\n"
		"\tfast read enable:       %02x\n"
		"\tspi arbitrary enable:   %02x\n"
		"\tfifo pointer clear:     %02x\n"
		"\tfifo pointer increment: %02x\n"
		"\taccess mac rom enable:  %02x\n"
		"\thost access rom enable: %02x\n"
		"\tarbitrary wait count:   %02x\n"
		"\tspi bridge disable:     %02x\n"
		"\tspi clock gate:         %02x\n"
		"\treserved:               %02x\n"
		"\tspi busy:               %02x\n"
		"spi restricted command register: %08x\n"
		"\trestricted command 0: %02x\n"
		"\trestricted command 1: %02x\n"
		"\trestricted command 2: %02x\n"
		"\trestricted command 3: %02x\n"
		"spi restricted command 2 register: %08x\n"
		"\trestricted command 4:                 %02x\n"
		"\trestricted command without address 0: %02x\n"
		"\trestricted command without address 1: %02x\n"
		"\trestricted command without address 2: %02x\n"
		"spi control register 1: %08x\n"
		"\tspi parameters:                       %02x\n"
		"\tfifo pointer:                         %02x\n"
		"\ttrack mac lock enable:                %02x\n"
		"\tclock speed:                          %02x\n"
		"\treserved:                             %02x\n"
		"\trestricted command without address 1: %02x\n"
		"\trestricted command without address 2: %02x\n"
		"spi command value 0: %08x\n"
		"\tmac lock command 0:   %02x\n"
		"\tmac lock command 1:   %02x\n"
		"\tmac unlock command 0: %02x\n"
		"\tmac unlock command 1: %02x\n"
		"spi command value 1: %08x\n"
		"\twrite enable:  %02x\n"
		"\twrite disable: %02x\n"
		"\tread id:       %02x\n"
		"\tread status:   %02x\n"
		"spi command value 2: %08x\n"
		"\tread byte:  %02x\n"
		"\tfast read:  %02x\n"
		"\tpage write: %02x\n"
		"\tbyte write: %02x\n"
		"reserved: %02x\n"
		"spi cs: %02x\n"
		"\talternate spi cs enable: %02x\n"
		"\treserved:                %02x\n",
		spi_rcr->cntrl0_val,
		spi_rcr->cntrl0.op_code,
		spi_rcr->cntrl0.tx_byte_cnt,
		spi_rcr->cntrl0.rx_byte_cnt,
		spi_rcr->cntrl0.exec_op_code,
		spi_rcr->cntrl0.reserved_0,
		spi_rcr->cntrl0.fast_rd_en,
		spi_rcr->cntrl0.spi_arb_en,
		spi_rcr->cntrl0.fifo_ptr_clr,
		spi_rcr->cntrl0.fifo_ptr_inc,
		spi_rcr->cntrl0.acc_mac_rom_en,
		spi_rcr->cntrl0.host_acc_rom_en,
		spi_rcr->cntrl0.arb_wait_cnt,
		spi_rcr->cntrl0.spi_bridge_dis,
		spi_rcr->cntrl0.spi_clk_gate,
		spi_rcr->cntrl0.reserved_1,
		spi_rcr->cntrl0.spi_busy,
		spi_rcr->restr_cmd_val,
		spi_rcr->restr_cmd.restr_cmd0,
		spi_rcr->restr_cmd.restr_cmd1,
		spi_rcr->restr_cmd.restr_cmd2,
		spi_rcr->restr_cmd.restr_cmd3,
		spi_rcr->restr_cmd2_val,
		spi_rcr->restr_cmd2.restr_cmd4,
		spi_rcr->restr_cmd2.restr_cmd_wo_addr0,
		spi_rcr->restr_cmd2.restr_cmd_wo_addr1,
		spi_rcr->restr_cmd2.restr_cmd_wo_addr2,
		spi_rcr->cntrl1_val,
		spi_rcr->cntrl1.spi_params,
		spi_rcr->cntrl1.fifo_ptr,
		spi_rcr->cntrl1.track_mac_lck_en,
		spi_rcr->cntrl1.norm_speed,
		spi_rcr->cntrl1.reserved_0,
		spi_rcr->cntrl1.restr_cmd_wo_addr1,
		spi_rcr->cntrl1.restr_cmd_wo_addr2,
		spi_rcr->cmd_val0_val,
		spi_rcr->cmd_val0.mac_lck_cmd0,
		spi_rcr->cmd_val0.mac_lck_cmd1,
		spi_rcr->cmd_val0.mac_ulck_cmd0,
		spi_rcr->cmd_val0.mac_ulck_cmd1,
		spi_rcr->cmd_val1_val,
		spi_rcr->cmd_val1.wren,
		spi_rcr->cmd_val1.wrdi,
		spi_rcr->cmd_val1.rdid,
		spi_rcr->cmd_val1.rdsr,
		spi_rcr->cmd_val2_val,
		spi_rcr->cmd_val2.read,
		spi_rcr->cmd_val2.fread,
		spi_rcr->cmd_val2.pagewr,
		spi_rcr->cmd_val2.bytewr,
		spi_rcr->reserved_0,
		spi_rcr->spi_cs_val,
		spi_rcr->spi_cs.alt_spi_cs_en,
		spi_rcr->spi_cs.reserved_0);

	munmap(sba, size);
	close(mem);
	return 0;
}
