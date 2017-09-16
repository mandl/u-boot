
#include <common.h>
#include <asm/arch/mt6516.h>
#include <asm/io.h>
#include <asm/arch/mt6516_nand.h>

#define NFI_DEFAULT_ACCESS_TIMING        (0x44333)

#if defined(CONFIG_CMD_NAND)

typedef struct
{
	u16 id;			//deviceid+menuid
	u8 addr_cycle;
	u8 iowidth;
	u16 totalsize;
	u16 blocksize;
	u16 pagesize;
	u32 timmingsetting;
	char devciename[14];
	u32 advancedmode;//
}flashdev_info,*pflashdev_info;

#include <nand.h>
#include <linux/mtd/nand.h>

//#define USE_AHB_MODE 1



#define NAND_SECTOR_SIZE            (512)


#define IO_WIDTH_8                  8
#define IO_WIDTH_16                 16

#define NFI_WAIT_STATE_DONE(state) do{;}while ( readl(NFI_STA_REG32) & state)
#define NFI_WAIT_TO_READY()  do{;}while (!( readl(NFI_STA_REG32) & STA_BUSY2READY))

static struct nand_ecclayout mt6516_nand_oob = {
	//.useecc = MTD_NANDECC_AUTOPLACE,
	.eccbytes = 32,
	.eccpos = {32, 33, 34, 35, 36, 37, 38, 39,
		       40, 41, 42, 43, 44, 45, 46, 47,
		       48, 49, 50, 51, 52, 53, 54, 55,
		       56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {1, 7}, {9, 7}, {17, 7}, {25, 7}, {0, 0}}
};

#ifdef CFG_NAND_LEGACY
void nand_init(void)
{

}
#else

static flashdev_info devinfo;
struct mt6516_CMD {
	uint32_t u4ColAddr;
	uint32_t u4RowAddr;
	uint32_t u4OOBRowAddr;
	uint8_t au1OOB[64];
	uint8_t* pDataBuf;
};

#define CHIPVER_ECO_1 (0x8a00)
#define CHIPVER_ECO_2 (0x8a01)

static struct mt6516_CMD g_kCMD;
static uint32_t g_u4ChipVer;
static uint32_t g_i4ErrNum;
static bool g_bInitDone;

static const flashdev_info g_FlashTable[]= {
	//micro
	{	0xAA2C, 5, 8, 256, 128, 2048, 0x01113, "MT29F2G08ABD", 0},
	{	0xB12C, 4, 16, 128, 128, 2048, 0x01113, "MT29F1G16ABC", 0},
	{	0xBA2C, 5, 16, 256, 128, 2048, 0x01113, "MT29F2G16ABD", 0},
	{	0xAC2C, 5, 8, 512, 128, 2048, 0x01113, "MT29F4G08ABC", 0},
	{	0xA12C, 4, 8, 128, 128, 2048, 0x01113, "MT29F1G08ABB", 0},

	//hynix
	{	0xD3AD, 5, 8, 1024, 256, 2048, 0x44333, "HY27UT088G2A", 0},
	{	0xA1AD, 4, 8, 128, 128, 2048, 0x01123, "H8BCSOPJOMCP", 0},
	//toshiba
	{	0xBC98, 5, 16, 512, 128, 2048, 0x01123, "TYBC0A111086K", 0},
	{	0x9098, 5, 16, 816, 128, 2048, 0x00113, "TY9C000000CMG", 0},
	{	0x9498, 5, 16, 375, 128, 2048, 0x00113, "TY9C000000CMG", 0},

	{	0xC198, 4, 16, 128, 128, 2048, 0x44333, "TC58NWGOS8C", 0},
	{	0xBA98, 5, 16, 256, 128, 2048, 0x02113, "TC58NYG1S8C", 0},
	//st-micro
	{	0xBA20, 5, 16, 256, 128, 2048, 0x01123, "ND02CGR4B2DI6", 0},

	// elpida
	{	0xBC20, 5, 16, 512, 128, 2048, 0x01123, "04GR4B2DDI6", 0},
	{	0x0000, 0, 0, 0, 0, 0, 0, "xxxxxxxxxxxxx", 0}
};
bool get_device_info(u16 id,flashdev_info *pdevinfo)
{
	uint32_t index;
	for(index=0;g_FlashTable[index].id!=0;index++)
	{
		if(id==g_FlashTable[index].id)
		{
			pdevinfo->id = g_FlashTable[index].id;
			pdevinfo->blocksize = g_FlashTable[index].blocksize;
			pdevinfo->addr_cycle = g_FlashTable[index].addr_cycle;
			pdevinfo->iowidth = g_FlashTable[index].iowidth;
			pdevinfo->timmingsetting = g_FlashTable[index].timmingsetting;
			pdevinfo->advancedmode = g_FlashTable[index].advancedmode;
			pdevinfo->pagesize = g_FlashTable[index].pagesize;
			pdevinfo->totalsize = g_FlashTable[index].totalsize;
			memcpy(pdevinfo->devciename,g_FlashTable[index].devciename,sizeof(pdevinfo->devciename));

			break;
		}
	}
	if(0==pdevinfo->id)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void dump_nfi();

void dump_nfi()
{
#ifdef DEBUG
	printf("NFI_ACCCON: 0x%x\n", readl(NFI_ACCCON_REG32));
	printf("NFI_PAGEFMT: 0x%x\n", readw(NFI_PAGEFMT_REG16));
	printf("NFI_CNFG: 0x%x\n", readw(NFI_CNFG_REG16));
	printf("NFI_CON: 0x%x\n", readw(NFI_CON_REG16));
	printf("NFI_STRDATA: 0x%x\n", readw(NFI_STRDATA_REG16));
	printf("NFI_ADDRCNTR: 0x%x\n", readw(NFI_ADDRCNTR_REG16));
	printf("NFI_FIFOSTA: 0x%x\n", readw(NFI_FIFOSTA_REG16));
	printf("NFI_ADDRNOB: 0x%x\n", readw(NFI_ADDRNOB_REG16));
	printf("NFI_FDM_0L: 0x%x\n", readl(NFI_FDM0L_REG32));
	printf("NFI_FDM_0M: 0x%x\n", readl(NFI_FDM0M_REG32));
	printf("NFI_IOCON: 0x%x\n", readw(NFI_IOCON_REG16));
	printf("NFI_BYTELEN: 0x%x\n", readw(NFI_BYTELEN_REG16));
	printf("NFI_COLADDR: 0x%x\n", readl(NFI_COLADDR_REG32));
	printf("NFI_ROWADDR: 0x%x\n", readl(NFI_ROWADDR_REG32));
	printf("ECC_ENCCNFG: 0x%x\n", readl(ECC_ENCCNFG_REG32));
	printf("ECC_ENCCON: 0x%x\n", readw(ECC_ENCCON_REG16));
	printf("ECC_DECCNFG: 0x%x\n", readl(ECC_DECCNFG_REG32));
	printf("ECC_DECCON: 0x%x\n", readw(ECC_DECCON_REG16));

#endif
}
//-------------------------------------------------------------------------------
static void ECC_Config(void)
{
	uint32_t u4ENCODESize;
	uint32_t u4DECODESize;

	writew(DEC_DE,ECC_DECCON_REG16);
	do {;}while (!readw(ECC_DECIDLE_REG16));

	writew(ENC_DE,ECC_ENCCON_REG16);
	do {;}while (!readl(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	writel(NFI_FDM0L_REG32,ECC_FDMADDR_REG32);

	u4ENCODESize = (NAND_SECTOR_SIZE + 8) << 3;
	u4DECODESize = ((NAND_SECTOR_SIZE + 8) << 3) + 4 * 13;

	/* configure ECC decoder && encoder*/

	writel(ECC_CNFG_ECC4|DEC_CNFG_NFI|DEC_CNFG_EMPTY_EN|
			(u4DECODESize<<DEC_CNFG_CODE_SHIFT),ECC_DECCNFG_REG32);

	writel(ECC_CNFG_ECC4|ENC_CNFG_NFI|
			(u4ENCODESize << ENC_CNFG_MSG_SHIFT),ECC_ENCCNFG_REG32);

#if USE_AHB_MODE
	setbits_le32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);

#else
	setbits_le32(ECC_DECCNFG_REG32, DEC_CNFG_EL);
#endif
}
//-------------------------------------------------------------------------------
static void ECC_Decode_Start(void)
{
	/* wait for device returning idle */
	while(!(readw(ECC_DECIDLE_REG16) & DEC_IDLE));
	writew(DEC_EN,ECC_DECCON_REG16);
}
//-------------------------------------------------------------------------------
static void ECC_Decode_End(void)
{
	/* wait for device returning idle */
	while(!(readw(ECC_DECIDLE_REG16) & DEC_IDLE));
	writew(DEC_DE,ECC_DECCON_REG16);
}
//-------------------------------------------------------------------------------
static void ECC_Encode_Start(void)
{
	/* wait for device returning idle */
	while(!(readl(ECC_ENCIDLE_REG32) & ENC_IDLE));
	writew(ENC_EN,ECC_ENCCON_REG16);
}
//-------------------------------------------------------------------------------
static void ECC_Encode_End(void)
{
	/* wait for device returning idle */
	while(!(readl(ECC_ENCIDLE_REG32) & ENC_IDLE));
	writew(ENC_DE,ECC_ENCCON_REG16);
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_check_bch_error(u8* pDataBuf, uint32_t u4SecIndex, uint32_t u4PageAddr)
{
	bool bRet = true;
	u16 u2SectorDoneMask = 1 << u4SecIndex;
	uint32_t i, u4ErrNum;
	uint32_t timeout = 0xFFFF;

	uint32_t u4ErrorNumDebug;

	debug("mt6516_nand_check_bch_error\n");

#if !USE_AHB_MODE
	uint32_t au4ErrBitLoc[6];
	uint32_t u4ErrByteLoc, u4BitOffset;
	uint32_t u4ErrBitLoc1th, u4ErrBitLoc2nd;
#endif

	//4 // Wait for Decode Done
	while (0 == (u2SectorDoneMask & readw(ECC_DECDONE_REG16))) {
		timeout--;
		if (0 == timeout) {
			debug("timeout");
			return false;
		}
	}

#if USE_AHB_MODE
	u4ErrorNumDebug = readl(ECC_DECENUM_REG32);
	if (0 != (u4ErrorNumDebug & 0xFFFF)) {
		for (i = 0; i <= u4SecIndex; ++i) {
			u4ErrNum = readl(ECC_DECENUM_REG32) >> (i << 2);
			u4ErrNum &= 0xF;
			if (0xF == u4ErrNum) {
				debug( "AHB_MODE : UnCorrectable at PageAddr=%d, Sector=%d\n", u4PageAddr, i);
				bRet = false; //4 //Uncorrectable
			} else {
				debug( "AHB_MODE : Correct %d at PageAddr=%d, Sector=%d\n", u4ErrNum, u4PageAddr, i);
			}
		}
	}
#else
	memset(au4ErrBitLoc, 0x0, sizeof(au4ErrBitLoc));
	u4ErrorNumDebug = readl(ECC_DECENUM_REG32);
	u4ErrNum = readl(ECC_DECENUM_REG32) >> (u4SecIndex << 2);
	u4ErrNum &= 0xF;
	if (u4ErrNum) {
		if (0xF == u4ErrNum) {
			debug( "UnCorrectable at PageAddr=%d\n", u4PageAddr);
			bRet = false;
		} else {
			for (i = 0; i < ((u4ErrNum+1)>>1); ++i) {
				au4ErrBitLoc[i] = readl(ECC_DECEL0_REG32 + i<< 2);
				u4ErrBitLoc1th = au4ErrBitLoc[i] & 0x1FFF;
				if (u4ErrBitLoc1th < 0x1000) {
					u4ErrByteLoc = u4ErrBitLoc1th/8;
					u4BitOffset = u4ErrBitLoc1th%8;
					pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc]^(1<<u4BitOffset);
				} else {
					debug( "UnCorrectable ErrLoc=%d\n", au4ErrBitLoc[i]);
				}

				u4ErrBitLoc2nd = (au4ErrBitLoc[i] >> 16) & 0x1FFF;
				if (0 != u4ErrBitLoc2nd) {
					if (u4ErrBitLoc2nd < 0x1000) {
						u4ErrByteLoc = u4ErrBitLoc2nd/8;
						u4BitOffset = u4ErrBitLoc2nd%8;
						pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc]^(1<<u4BitOffset);
					} else {
						debug( "UnCorrectable High ErrLoc=%d\n", au4ErrBitLoc[i]);
					}
				}
			}
			bRet = true;
		}
		debug("u4SecIndex %d\n",u4SecIndex);
		if (0 == (readw(ECC_DECFER_REG16) & (1 << (u4SecIndex))))
		{
			bRet = false;
		}
	}
#endif

	return bRet;
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_RFIFOValidSize(u16 u2Size)
{
	uint32_t timeout = 0xFFFF;
	while (FIFO_RD_REMAIN(readw(NFI_FIFOSTA_REG16)) < u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	if(u2Size==0)
	{
		while (FIFO_RD_REMAIN(readw(NFI_FIFOSTA_REG16)))
		{
			timeout--;
			if (0 == timeout) {
				printf("mt6516_nand_RFIFOValidSize failed: 0x%x\n", u2Size);
				return false;
			}
		}
	}
	return true;
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_WFIFOValidSize(u16 u2Size)
{
	uint32_t timeout = 0xFFFF;
	while (FIFO_WR_REMAIN(readw(NFI_FIFOSTA_REG16)) > u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	if(u2Size==0)
	{
		while (FIFO_WR_REMAIN(readw(NFI_FIFOSTA_REG16)))
		{
			timeout--;
			if (0 == timeout) {
				printf("mt6516_nand_WFIFOValidSize failed: 0x%x\n", u2Size);
				return false;
			}
		}
	}
	return true;
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_status_ready(uint32_t u4Status)
{
	uint32_t timeout = 0xFFFF;
	while ((readl(NFI_STA_REG32) & u4Status) != 0) {
		timeout--;
		if (0 == timeout) {
			debug("mt6516_nand_status_ready timeout\n");
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_reset(void)
{
	/* issue reset operation */
	debug("mt6516_nand_reset\n");
	writew(CON_FIFO_FLUSH | CON_NFI_RST, NFI_CON_REG16);

	return mt6516_nand_status_ready(STA_NFI_FSM_MASK|STA_NAND_BUSY) &&
	mt6516_nand_RFIFOValidSize(0) &&
	mt6516_nand_WFIFOValidSize(0);
}
//-------------------------------------------------------------------------------
static void mt6516_nand_set_mode(u16 u2OpMode)
{
	u16 u2Mode = readw(NFI_CNFG_REG16);
	u2Mode &= ~CNFG_OP_MODE_MASK;
	u2Mode |= u2OpMode;
	writew(u2Mode,NFI_CNFG_REG16);
}
//-------------------------------------------------------------------------------
static void mt6516_nand_set_autoformat(bool bEnable)
{
	if (bEnable) {
		setbits_le16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	} else {
		clrbits_le16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	}
}
//-------------------------------------------------------------------------------
static void mt6516_nand_configure_fdm(u16 u2FDMSize)
{
	clrbits_le16(NFI_PAGEFMT_REG16, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	setbits_le16(NFI_PAGEFMT_REG16, u2FDMSize << PAGEFMT_FDM_SHIFT);
	setbits_le16(NFI_PAGEFMT_REG16, u2FDMSize << PAGEFMT_FDM_ECC_SHIFT);
}
//-------------------------------------------------------------------------------
static void mt6516_nand_configure_lock(void)
{
	uint32_t u4WriteColNOB = 2;
	uint32_t u4WriteRowNOB = 3;
	uint32_t u4EraseColNOB = 0;
	uint32_t u4EraseRowNOB = 3;
	writew(
			(u4WriteColNOB << PROG_CADD_NOB_SHIFT) |
			(u4WriteRowNOB << PROG_RADD_NOB_SHIFT) |
			(u4EraseColNOB << ERASE_CADD_NOB_SHIFT) |
			(u4EraseRowNOB << ERASE_RADD_NOB_SHIFT),NFI_LOCKANOB_REG16);

	// Workaround method for ECO1 mt6516	
	if (CHIPVER_ECO_1 == g_u4ChipVer) {
		int i;
		for (i = 0; i < 16; ++i) {
			writel(0xFFFFFFFF,NFI_LOCK00ADD_REG32 + (i << 1));
			writel(0xFFFFFFFF,NFI_LOCK00FMT_REG32 + (i << 1));
		}
		//DRV_WriteReg16(NFI_LOCKANOB_REG16, 0x0);
		writel(0xFFFFFFFF,NFI_LOCKCON_REG32);
		writew(NFI_LOCK_ON,NFI_LOCK_REG16);
	}
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_set_command(u16 command)
{
	/* Write command to device */
	writew(command,NFI_CMD_REG16);
	return mt6516_nand_status_ready(STA_CMD_STATE);
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_set_address(uint32_t u4ColAddr, uint32_t u4RowAddr, u16 u2ColNOB, u16 u2RowNOB)
{
	/* fill cycle addr */
	writel(u4ColAddr,NFI_COLADDR_REG32);
	writel(u4RowAddr,NFI_ROWADDR_REG32);
	writew(u2ColNOB|(u2RowNOB << ADDR_ROW_NOB_SHIFT),NFI_ADDRNOB_REG16);
	return mt6516_nand_status_ready(STA_ADDR_STATE);
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_check_RW_count(u16 u2WriteSize)
{
	uint32_t timeout = 0xFFFF;
	u16 u2SecNum = u2WriteSize >> 9;
	while (ADDRCNTR_CNTR(readw(NFI_ADDRCNTR_REG16)) < u2SecNum) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	return true;
}

/********************************************************************************
 * u4RowAddr/u4ColAddr: read operation start address.
 * bFull: true for read full page or main data
 *        false for read oob only
 *        the difference is NFI will work in different mode.
 *********************************************************************************/
static bool mt6516_nand_ready_for_read(
		struct nand_chip *chip, uint32_t u4RowAddr, uint32_t u4ColAddr, bool bFull, u8 *buf)
{
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	debug("mt6516_nand_ready_for_read\n");
	bool bRet = false;
	u16 sec_num = 1 << (chip->page_shift - 9);
	uint32_t col_addr = u4ColAddr;
	uint32_t colnob=2, rawnob=devinfo.addr_cycle-2;
	if (chip->options & NAND_BUSWIDTH_16)
	col_addr /= 2;

	if (!mt6516_nand_reset()) {
		goto cleanup;
	}

	mt6516_nand_set_mode(CNFG_OP_READ);
	setbits_le16(NFI_CNFG_REG16, CNFG_READ_EN);
	writew(sec_num << CON_NFI_SEC_SHIFT,NFI_CON_REG16);

	if (bFull)
	{
#if USE_AHB_MODE
		setbits_le16(NFI_CNFG_REG16, CNFG_AHB);
		writel(buf,NFI_STRADDR_REG32);
#else
		clrbits_le16(NFI_CNFG_REG16, CNFG_AHB);
#endif
		setbits_le16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	}
	else
	{
		clrbits_le16(NFI_CNFG_REG16, CNFG_AHB);
		clrbits_le16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	}

	mt6516_nand_set_autoformat(bFull);
	if (bFull)
	ECC_Decode_Start();

	if (!mt6516_nand_set_command(NAND_CMD_READ0)) {
		goto cleanup;
	}

	//1 FIXED ME: For Any Kind of AddrCycle
	if (!mt6516_nand_set_address(col_addr, u4RowAddr, colnob, rawnob)) {
		goto cleanup;
	}

	if (!mt6516_nand_set_command(NAND_CMD_READSTART)) {
		goto cleanup;
	}

	if (!mt6516_nand_status_ready(STA_NAND_BUSY)) {
		goto cleanup;
	}

	bRet = true;

	cleanup:
	return bRet;
}

static bool mt6516_nand_ready_for_write(
		struct nand_chip *nand, uint32_t u4RowAddr, u8 *buf)
{
	bool bRet = false;
	u16 sec_num = 1 << (nand->page_shift - 9);
	uint32_t colnob=2, rawnob=devinfo.addr_cycle-2;
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	if (!mt6516_nand_reset()) {
		return false;
	}

	mt6516_nand_set_mode(CNFG_OP_PRGM);

	clrbits_le16(NFI_CNFG_REG16, CNFG_READ_EN);

	writew(sec_num << CON_NFI_SEC_SHIFT,NFI_CON_REG16);

#if USE_AHB_MODE
	setbits_le16(NFI_CNFG_REG16, CNFG_AHB);
	writel(buf,NFI_STRADDR_REG32);
#else
	clrbits_le16(NFI_CNFG_REG16, CNFG_AHB);
#endif

	setbits_le16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	mt6516_nand_set_autoformat(true);

	ECC_Encode_Start();

	if (!mt6516_nand_set_command(NAND_CMD_SEQIN)) {
		goto cleanup;
	}

	//1 FIXED ME: For Any Kind of AddrCycle
	if (!mt6516_nand_set_address(0, u4RowAddr, 2, 3)) {
		goto cleanup;
	}

	if (!mt6516_nand_status_ready(STA_NAND_BUSY)) {
		goto cleanup;
	}

	bRet = true;
	cleanup:

	return bRet;
}

#if USE_AHB_MODE

static bool mt6516_nand_dma_transfer_data(u8 *buf, uint32_t length)
{
	uint32_t timeout = 0xFFFF;

	clrbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	debug("mt6516_nand_dma_transfer_data\n");
	// DRV_WriteReg32(NFI_STRADDR_REG32, buf);
	setbits_le16(NFI_CON_REG16, CON_NFI_BRD);
	while (length > readw(NFI_BYTELEN_REG16)) {
		timeout--;
		if (0 == timeout) {
			debug("timeout\n");
			return false; //4  // AHB Mode Time Out!
		}
	}

	return true;
}

#else

static bool mt6516_nand_mcu_transfer_data(u8 *buf, uint32_t length)
{
	uint32_t timeout = 0xFFFF;
	uint32_t i;
	uint32_t *buf32 = (uint32_t *)buf;

	debug("mt6516_nand_mcu_transfer_data\n");

	if (length % 4)
	// set byte read
	setbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	else
	clrbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	writel(0,NFI_STRADDR_REG32);
	setbits_le16(NFI_CON_REG16, CON_NFI_BRD);

	if (length % 4)
	{
		// read byte by byte
		for (i = 0; (i < length)&&(timeout > 0);) {
			if (FIFO_RD_REMAIN(readw(NFI_FIFOSTA_REG16)) >= 4) {
				*buf++ = (u8)readl(NFI_DATAR_REG32);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				debug("timeout\n");
				return false; //4 // MCU  Mode Time Out!
			}
		}
	}
	else
	{
		// read word by word
		for (i = 0; (i < length >> 2)&&(timeout > 0);) {
			if (FIFO_RD_REMAIN(readw(NFI_FIFOSTA_REG16)) >= 4) {
				*buf32++ = readl(NFI_DATAR_REG32);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				debug("timeout\n");
				return false; //4 // MCU  Mode Time Out!
			}
		}
	}

	return true;
}

#endif

static bool mt6516_nand_read_page_data(u8* buf, uint32_t length)
{

#if USE_AHB_MODE
	return mt6516_nand_dma_transfer_data(buf, length);
#else
	return mt6516_nand_mcu_transfer_data(buf, length);
#endif
}

//-----------------------------------------------------------------------------
static bool mt6516_nand_write_page_data(u8* pDataBuf, uint32_t u4Size)
{
	uint32_t timeout = 0xFFFF;
#if USE_AHB_MODE	
	clrbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	// DRV_WriteReg32(NFI_STRADDR_REG32, pDataBuf);
	setbits_le16(NFI_CON_REG16, CON_NFI_BWR);
	while (u4Size > readw(NFI_BYTELEN_REG16)) {
		timeout--;
		if (0 == timeout) {
			debug("timeout\n");
			return false; //4  // AHB Mode Time Out!
		}
	}
#else
	uint32_t i;
	uint32_t* pBuf32;
	clrbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	writel(0,NFI_STRADDR_REG32);
	setbits_le16(NFI_CON_REG16, CON_NFI_BWR);
	pBuf32 = (uint32_t*)pDataBuf;
	for (i = 0; (i < (u4Size >> 2))&&(timeout > 0);) {
		if (FIFO_WR_REMAIN(readw(NFI_FIFOSTA_REG16)) <= 12) {
			writel(*pBuf32++,NFI_DATAW_REG32);
			i++;
		} else {
			timeout--;
		}
		if (0 == timeout) {
			debug("timeout\n");
			return false; //4 // MCU Mode Time Out!		
		}
	}
#endif

	return true;
}
//-----------------------------------------------------------------------------
static void mt6516_nand_read_fdm_data(uint8_t* pDataBuf, uint32_t u4SecNum)
{
	uint32_t i;
	uint32_t* pBuf32 = (uint32_t*)pDataBuf;
	debug("mt6516_nand_read_fdm_data SecNum 0x%x\n",u4SecNum);
	for (i = 0; i < u4SecNum; ++i) {
		*pBuf32++=readl(NFI_FDM0L_REG32 + (i<<3));
		*pBuf32++=readl(NFI_FDM0M_REG32 + (i<<3));
	}

}
//-----------------------------------------------------------------------------
static void mt6516_nand_write_fdm_data(u8* pDataBuf, uint32_t u4SecNum)
{
	uint32_t i;
	uint32_t* pBuf32 = (uint32_t*)pDataBuf;
	for (i = 0; i < u4SecNum; ++i)
	{
		writel(*pBuf32++,NFI_FDM0L_REG32 + (i<<3));
		writel(*pBuf32++,NFI_FDM0M_REG32 + (i<<3));
	}
}
//-----------------------------------------------------------------------------
static void mt6516_nand_stop_read(void)
{
	debug("mt6516_nand_stop_read");
	clrbits_le16(NFI_CON_REG16, CON_NFI_BRD);
	ECC_Decode_End();
}
//-----------------------------------------------------------------------------
static void mt6516_nand_stop_write(void)
{
	clrbits_le16(NFI_CON_REG16, CON_NFI_BWR);
	ECC_Encode_End();
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_exec_read_page(
		struct nand_chip *nand, uint32_t u4RowAddr, uint32_t u4PageSize, u8* pPageBuf, uint8_t* pFDMBuf)
{
	bool bRet = true;
	uint32_t u4SecNum = u4PageSize >> 9;
	debug("mt6516_nand_exec_read_page   %x\n",pFDMBuf);

	if (mt6516_nand_ready_for_read(nand, u4RowAddr, 0, true, pPageBuf)) {
		if (!mt6516_nand_read_page_data(pPageBuf, u4PageSize)) {
			bRet = false;
		}
		if (!mt6516_nand_status_ready(STA_NAND_BUSY)) {
			bRet = false;
		}
		mt6516_nand_read_fdm_data(pFDMBuf, u4SecNum);

		if (!mt6516_nand_check_bch_error(pPageBuf, u4SecNum - 1, u4RowAddr)) {
			g_i4ErrNum++;
		}
		mt6516_nand_stop_read();
	}

	return bRet;
}
//-------------------------------------------------------------------------------
static bool mt6516_nand_exec_write_page(
		struct nand_chip *nand, uint32_t u4RowAddr, uint32_t u4PageSize, u8* pPageBuf, u8* pFDMBuf)
{
	bool bRet = true;
	uint32_t u4SecNum = u4PageSize >> 9;
	if (mt6516_nand_ready_for_write(nand, u4RowAddr, pPageBuf)) {
		mt6516_nand_write_fdm_data(pFDMBuf, u4SecNum);
		if (!mt6516_nand_write_page_data(pPageBuf, u4PageSize)) {
			bRet = false;
		}
		if (!mt6516_nand_check_RW_count(u4PageSize)) {
			bRet = false;
		}
		mt6516_nand_stop_write();
		mt6516_nand_set_command(NAND_CMD_PAGEPROG);
		while(readl(NFI_STA_REG32) & STA_NAND_BUSY);
	}
	return bRet;
}

#define OOB_PER_SECTOR 8     // available oob per sector

/******************************************************************************
 * Read OOB data from flash
 * row: page number
 * length: read oob data length, should always be times of 8
 * buf: oob data buffer
 * ****************************************************************************/
bool mt6516_nand_read_oob(struct nand_chip *chip, uint32_t page_addr, uint32_t length, u8* buf)
{
	uint32_t sector = 0;
	uint32_t col_addr = 0;

	if (length > 32 || length % 8 || !buf)
	{
		printf("[%s] invalid parameter, length: %d, buf: %p\n",
				__FUNCTION__, length, buf);
		return false;
	}

	while (length > 0)
	{
		col_addr = NAND_SECTOR_SIZE + sector * (NAND_SECTOR_SIZE + 16);
		if (!mt6516_nand_ready_for_read(chip, page_addr, col_addr, false, buf))
		return false;
		if (!mt6516_nand_mcu_transfer_data(buf + 8 * sector, 8))
		return false;
		clrbits_le16(NFI_CON_REG16, CON_NFI_BRD);
		sector++;
		length -= 8;
	}
	return true;
}

bool nand_block_bad(struct nand_chip *nand, uint32_t page_addr)
{
	uint32_t page = 0;
	uint32_t page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift);
	//uint32_t u4PageSize  = 1 << nand->page_shift;

	unsigned char oob_buf[OOB_PER_SECTOR];// 8 byte is enough for bad block
	memset(oob_buf, 0, OOB_PER_SECTOR);

	page &= ~(page_per_block - 1);

	if (!mt6516_nand_read_oob(nand, page_addr, OOB_PER_SECTOR, oob_buf))
	{
		printf("mt6516_nand_read_oob return fail!\n");
	}

	if (oob_buf[0] != 0xff)
	{
		printf("BAD BLOCK detected at 0x%x, oob_buf[0] is %x\n", page_addr, oob_buf[0]);
		dump_nfi();
		return true;
	}

	return false;		// this is a good block
}


static void mt6516_nand_command_bp(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	struct nand_chip* nand = mtd_to_nand(mtd);

	debug("mt6516_nand_command_bp: 0x%x\n",command );
	switch (command)
	{
		case NAND_CMD_SEQIN:
		/* Reset g_kCMD */
		if (g_kCMD.u4RowAddr != page_addr) {
			memset(g_kCMD.au1OOB, 0xFF, sizeof(g_kCMD.au1OOB));
			g_kCMD.pDataBuf = NULL;
		}
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column;
		break;

		case NAND_CMD_PAGEPROG:
		if (g_kCMD.pDataBuf || (0xFF != g_kCMD.au1OOB[0])) {
			u8* pDataBuf = g_kCMD.pDataBuf ? g_kCMD.pDataBuf : nand->buffers->databuf;
			mt6516_nand_exec_write_page(nand, g_kCMD.u4RowAddr, mtd->writesize,
					pDataBuf, g_kCMD.au1OOB);
			g_kCMD.u4RowAddr = (uint32_t)-1;
			g_kCMD.u4OOBRowAddr = (uint32_t)-1;
		}
		break;

		case NAND_CMD_READOOB:
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column + mtd->writesize;
		g_i4ErrNum = 0;
		break;

		case NAND_CMD_READ0:
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column;
		g_i4ErrNum = 0;
		break;

		case NAND_CMD_ERASE1:
		mt6516_nand_reset();
		mt6516_nand_set_mode(CNFG_OP_ERASE);
		mt6516_nand_set_command(NAND_CMD_ERASE1);
		mt6516_nand_set_address(0,page_addr,0,3);
		break;

		case NAND_CMD_ERASE2:
		mt6516_nand_set_command(NAND_CMD_ERASE2);
		while(readl(NFI_STA_REG32) & STA_NAND_BUSY);
		break;

		case NAND_CMD_STATUS:
		clrbits_le16(NFI_CNFG_REG16, CNFG_BYTE_RW);
		mt6516_nand_reset();
		mt6516_nand_set_mode(CNFG_OP_SRD);
		mt6516_nand_set_command(NAND_CMD_STATUS);
		clrbits_le16(NFI_CON_REG16, CON_NFI_NOB_MASK);
		writew(CON_NFI_SRD|(1 << CON_NFI_NOB_SHIFT),NFI_CON_REG16);
		break;

		case NAND_CMD_RESET:
		mt6516_nand_reset();
		//mt6516_nand_exec_reset_device();
		break;

		case NAND_CMD_READID:
		setbits_le16(NFI_CNFG_REG16, CNFG_READ_EN|CNFG_BYTE_RW);
		mt6516_nand_reset();
		mt6516_nand_set_mode(CNFG_OP_SRD);
		mt6516_nand_set_command(NAND_CMD_READID);
		mt6516_nand_set_address(0,0,1,0);
		writew(CON_NFI_SRD,NFI_CON_REG16);
		while(readl(NFI_STA_REG32) & STA_DATAR_STATE);
		break;

		default:
		debug("[ERR] mt6516_nand_command_bp : unknow command %d\n", command);
		BUG();
		break;
	}
}

//-----------------------------------------------------------------------------
static void mt6516_nand_select_chip(struct mtd_info *mtd, int chip)
{
	uint32_t busw = 0;

	debug("mt6516_nand_select_chip: %d\n",chip);

	if (chip == -1 && false == g_bInitDone) {

		struct nand_chip *nand = mtd_to_nand(mtd);

		if (nand->page_shift == 11) { //2 //Large Page
			setbits_le16(NFI_PAGEFMT_REG16, PAGEFMT_SPARE_16 | PAGEFMT_2K);
			nand->cmdfunc = mt6516_nand_command_bp;
		}
		else if(nand->page_shift == 9) //2 // Small Page
		{
			setbits_le16(NFI_PAGEFMT_REG16, PAGEFMT_SPARE_16 | PAGEFMT_512);
			//nand->cmdfunc = mt6516_nand_command_sp;
		}

		busw = devinfo.iowidth;;

		if(IO_WIDTH_16 == busw)
		{
			debug("Setting the NFI PAGEFMT to enable %d bit I/O\n", busw);
			setbits_le16(NFI_PAGEFMT_REG16, PAGEFMT_DBYTE_EN);
		}
		else if(IO_WIDTH_8 == busw)
		{
			debug("Setting the NFI PAGEFMT to enable %d bit I/O\n", busw);
		}
		else
		{
			debug("Setting NFI_BUS_WIDTH (%d) is error, please check the NAND setting in UBOOT\n", busw);

		}
		/*
		 #if defined(CFG_MT6516_NAND_WIDTH_16)
		 printf("Setting the NFI PAGEFMT to enable 16 bit I/O\n");
		 setbits_le16(NFI_PAGEFMT_REG16, PAGEFMT_DBYTE_EN);
		 #endif
		 */
		g_bInitDone = true;
	}

	switch(chip)
	{
		case 0:
		case 1:
		writew(chip,NFI_CSEL_REG16);
		break;
	}
}
//-----------------------------------------------------------------------------
static u_char mt6516_nand_read_byte(struct mtd_info *mtd)
{

	u_char tmp;
	while(0 == FIFO_RD_REMAIN(readw(NFI_FIFOSTA_REG16)));
	tmp = readb(NFI_DATAR_REG32);
	debug("mt6516_nand_read_byte 0x%x\n",tmp);
	return tmp;

}
//-----------------------------------------------------------------------------
static void mt6516_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct nand_chip* nand = mtd_to_nand(mtd);
	struct mt6516_CMD* pkCMD = &g_kCMD;
	uint32_t u4ColAddr = pkCMD->u4ColAddr;
	uint32_t u4PageSize = mtd->writesize;

	debug("mt6516_nand_read_buf:  0x%x  len 0x%x\n",(uint32_t)buf,(uint32_t)len);
	debug("mu4ColAddr:  0x%x  u4PageSize 0x%x   0x%x \n",u4ColAddr,u4PageSize,(uint32_t)pkCMD);

	if (u4ColAddr < u4PageSize) {
		if ((u4ColAddr == 0) && (len >= u4PageSize)) {
			mt6516_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize,
					buf, pkCMD->au1OOB);
			if (len > u4PageSize) {
				uint32_t u4Size = min(len - u4PageSize, sizeof(pkCMD->au1OOB));
				memcpy(buf + u4PageSize, pkCMD->au1OOB, u4Size);
			}
		} else {
			mt6516_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize,
					nand->buffers->databuf, pkCMD->au1OOB);
			memcpy(buf, nand->buffers->databuf + u4ColAddr, len);
		}
		pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
	} else {
		uint32_t u4Offset = u4ColAddr - u4PageSize;
		uint32_t u4Size = min(len - u4PageSize - u4Offset, sizeof(pkCMD->au1OOB));
		if (pkCMD->u4OOBRowAddr != pkCMD->u4RowAddr) {
			mt6516_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize,
					nand->buffers->databuf, pkCMD->au1OOB);
			pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
		}
		memcpy(buf, pkCMD->au1OOB + u4Offset, u4Size);
	}
	pkCMD->u4ColAddr += len;
	debug("done\n");
}
//-----------------------------------------------------------------------------
static void mt6516_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	struct mt6516_CMD* pkCMD = &g_kCMD;
	uint32_t u4ColAddr = pkCMD->u4ColAddr;
	uint32_t u4PageSize = mtd->writesize;
	uint32_t i;
	debug("mt6516_nand_write_buf\n");
	if (u4ColAddr >= u4PageSize) {
		u8* pOOB = pkCMD->au1OOB;
		uint32_t u4Size = min(len, sizeof(pkCMD->au1OOB));
		for (i = 0; i < u4Size; i++) {
			pOOB[i] &= buf[i];
		}
	} else {
		pkCMD->pDataBuf = (u8*)buf;
	}
	pkCMD->u4ColAddr += len;
}
//-----------------------------------------------------------------------------
//static int mt6516_nand_verify_buf(struct mtd_info *mtd, const u_char *buf, int len)
//{
//	return 0;	/* FIXME. Always return success */
//}
//-----------------------------------------------------------------------------
static int mt6516_nand_dev_ready(struct mtd_info *mtd)
{
	debug("mt6516_nand_dev_ready\n");
	return !(readl(NFI_STA_REG32) & STA_NAND_BUSY);
}
//-----------------------------------------------------------------------------
//static void mt6516_nand_enable_hwecc(struct mtd_info *mtd, int mode)
//{
//	//1 No need to implement
//    return;
//}
//-----------------------------------------------------------------------------
static int mt6516_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	/* ECC is encoded and decoded automatically at reading and programning buf. */
	debug("mt6516_nand_calculate_ecc\n");
	memset(ecc_code, 0xFF, 32);
	return 0;
}
//-----------------------------------------------------------------------------
static int mt6516_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	return g_i4ErrNum;
}

static void mt6516_nand_hwctl(struct mtd_info *mtd, int mode)
{
}

//-----------------------------------------------------------------------------
static int mt6516_nand_init(struct nand_chip *this)
{
	debug("mt6516_nand_init\n");

	/* Power on NFI HW component. */
	writel(1 << PDN_PERI_NFI, APMCUSYS_PDN_CLR0);

	//writel(1 << PDN_PERI_DMA, APMCUSYS_PDN_CLR0);

	/* Dynamic Control */
	g_bInitDone = false;
	g_u4ChipVer = readl(CONFIG_BASE/*HW_VER*/);
	g_kCMD.u4OOBRowAddr = (uint32_t)-1;
	g_kCMD.u4ColAddr = (uint32_t)-1;;
	g_kCMD.u4RowAddr = (uint32_t)-1;;
	memset(g_kCMD.au1OOB, 0xFF, sizeof(g_kCMD.au1OOB));
	g_kCMD.pDataBuf=0;

	writew(0,NFI_CNFG_REG16);
	writew(0,NFI_PAGEFMT_REG16);

	/* Set default NFI access timing control */
	writel(NFI_DEFAULT_ACCESS_TIMING,NFI_ACCCON_REG32);

	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	mt6516_nand_reset();

	/* Initilize interrupt. Clear interrupt, read clear. */
	readw(NFI_INTR_REG16);

	/* Interrupt arise when read data or program data to/from AHB is done. */
	writew(0,NFI_INTR_EN_REG16);

	writew(CNFG_HW_ECC_EN,NFI_CNFG_REG16);

	ECC_Config();
	mt6516_nand_configure_fdm(8);
	mt6516_nand_configure_lock();

	return 0;
}
//-----------------------------------------------------------------------------
int board_nand_init(struct nand_chip *nand)

{
	int res = mt6516_nand_init(nand);
	int busw = 0;
	int id, nand_maf_id,nand_dev_id;
	struct mtd_info *mtd;
	debug("board_nand_init\n");

	memset(&devinfo,0,sizeof(devinfo));
	nand->select_chip = mt6516_nand_select_chip;
	nand->cmdfunc = mt6516_nand_command_bp;
	nand->read_byte = mt6516_nand_read_byte;
	nand->write_buf = mt6516_nand_write_buf;
	nand->dev_ready = mt6516_nand_dev_ready;
	//nand->enable_hwecc 	= mt6516_nand_enable_hwecc;
	nand->ecc.calculate = mt6516_nand_calculate_ecc;
	nand->ecc.correct = mt6516_nand_correct_data;
	//nand->verify_buf 	= mt6516_nand_verify_buf;
	nand->read_buf = mt6516_nand_read_buf;
	//nand->block_markbad = mt6516_nand_default_block_markbad;

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.hwctl = mt6516_nand_hwctl;
	nand->ecc.layout = &mt6516_nand_oob;
	nand->ecc.size = 512;
	nand->ecc.strength = 4;
	//nand->options 	    = NAND_NO_AUTOINCR;

	mtd = nand_to_mtd(nand);

	mt6516_nand_command_bp(mtd, NAND_CMD_READID, 0,0);

	// read id
	nand_maf_id = mt6516_nand_read_byte (mtd);
	nand_dev_id = mt6516_nand_read_byte (mtd);

	debug("Manufacturer ID: 0x%x, Chip ID: 0x%x\n",nand_maf_id,nand_dev_id);

	//add for multi-device support
	id = (nand_dev_id<<8)|nand_maf_id;

	if(get_device_info(id,&devinfo))
	{
		busw = devinfo.iowidth;
		writel (devinfo.timmingsetting,NFI_ACCCON_REG32);
		debug ("NAND device %x found  in table\n",devinfo.id);
	}
	else
	{
		debug ("No NAND device %x found in table!!!\n",id);
	}

	if(IO_WIDTH_16 == busw)
	{
		debug("Setting the MTD option to enable %d bit I/O\n", busw);
		nand->options |= NAND_BUSWIDTH_16;
	}
	else if(IO_WIDTH_8 == busw)
	{
		debug("Setting the MTD option to enable %d bit I/O\n", busw);
	}
	else
	{
		debug("Setting NFI_BUS_WIDTH (%d) is error, please check the NAND setting in UBOOT\n", busw);

	}

	if (res == 0)
	{
//        static part_dev_t dev;
//        dev.id     = 0;
//        dev.init   = 1;
//        dev.blkdev = (block_dev_desc_t*)nand;
//        dev.read   = mt6516_nand_part_read;
//		mt6516_part_register_device(&dev);
	}
	else
	{
		//PDN_Power_CONA_DOWN(PDN_PERI_NFI, KAL_TRUE);
	}

	return res;

}
#endif
//-----------------------------------------------------------------------------
#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
//void board_nand_select_device(struct nand_chip *nand, int chip)
//{
//	mt6516_nand_select_chip(NULL, chip);
//
//    return;
//}
#endif
//-----------------------------------------------------------------------------

#endif
