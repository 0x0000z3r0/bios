// http://ftp.kolibrios.org/users/art_zh/doc/public/A50_BDG.pdf
#ifndef _FCH_A50M_H_
#define _FCH_A50M_H_

#include <stdint.h>

union rom_prot {
	struct {
		uint32_t write:1;
		uint32_t read:1;
		uint32_t offset:9;
		uint32_t base:21;
	};
	uint32_t val;
};

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

#define SPI_READ_MODE (1 << 30) | (1 << 29) | (1 << 28)

struct spi_rcr {
	union {
		struct {
			uint32_t op_code:8;
			uint32_t tx_byte_cnt:4;
			uint32_t rx_byte_cnt:4;
			uint32_t exec_op_code:1;
			uint32_t reserved_0:1;
			uint32_t fast_rd_en:1;
			uint32_t spi_arb_en:1;
			uint32_t fifo_ptr_clr:1;
			uint32_t fifo_ptr_inc:1;
			uint32_t acc_mac_rom_en:1;
			uint32_t host_acc_rom_en:1;
			uint32_t arb_wait_cnt:3;
			uint32_t spi_bridge_dis:1;
			uint32_t spi_clk_gate:1;
			uint32_t reserved_1:2;
			uint32_t spi_busy:1;
		} cntrl0;
		uint32_t cntrl0_val;
	};
	union {
		struct {
			uint8_t restr_cmd0;
			uint8_t restr_cmd1;
			uint8_t restr_cmd2;
			uint8_t restr_cmd3;
		} restr_cmd;
		uint32_t restr_cmd_val;
	};
	union {
		struct {
			uint8_t restr_cmd4;
			uint8_t restr_cmd_wo_addr0;
			uint8_t restr_cmd_wo_addr1;
			uint8_t restr_cmd_wo_addr2;
		} restr_cmd2;
		uint32_t restr_cmd2_val;
	};
	union {
		struct {
			uint32_t spi_params:8;
			uint32_t fifo_ptr:3;
			uint32_t track_mac_lck_en:1;
			uint32_t norm_speed:2;
			uint32_t reserved_0:2;
			uint32_t restr_cmd_wo_addr1:8;
			uint32_t restr_cmd_wo_addr2:8;
		} cntrl1;
		uint32_t cntrl1_val; 
	};
	union {
		struct {
			uint8_t mac_lck_cmd0;
			uint8_t mac_lck_cmd1;
			uint8_t mac_ulck_cmd0;
			uint8_t mac_ulck_cmd1;
		} cmd_val0;
		uint32_t cmd_val0_val;
	};
	union {
		struct {
			uint8_t wren;
			uint8_t wrdi;
			uint8_t rdid;
			uint8_t rdsr;
		} cmd_val1;
		uint32_t cmd_val1_val;
	};
	union {
		struct {
			uint8_t read;
			uint8_t fread;
			uint8_t pagewr;
			uint8_t bytewr;
		} cmd_val2;
		uint32_t cmd_val2_val;
	};
	uint8_t reserved_0;
	union {
		struct {
			uint8_t alt_spi_cs_en:2;
			uint8_t reserved_0:6;
		} spi_cs;
		uint8_t spi_cs_val;
	};
} __attribute__((packed));

#endif
