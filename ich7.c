
// ICH7-M datasheet: 
// https://www.intel.com/content/dam/doc/datasheet/i-o-controller-hub-7-datasheet.pdf

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
