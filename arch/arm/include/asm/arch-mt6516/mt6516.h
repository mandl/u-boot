


#ifndef _ARCH_MT6516_H_
#define _ARCH_MT6516_H_



/* IO register definitions */
#define EFUSE_BASE      	(0x80000000)
#define CONFIG_BASE      	(0x80001000)
#define GPIO_BASE        	(0x80002000)
#define RGU_BASE         	(0x80003000)
#define DVFS_BASE         	(0x80004000)


#define EMI_BASE         	(0x80020000)
#define CIRQ_BASE        	(0x80021000)
#define DMA_BASE         	(0x80022000)

#define UART1_BASE 			(0x80023000)
#define UART2_BASE 			(0x80024000)
#define UART3_BASE 			(0x80025000)
#define UART4_BASE 			(0x8002B000)


#define GPT_BASE         	(0x80026000)
#define HDQ_BASE         	(0x80027000)
#define KP_BASE         	(0x80028000)
#define PWM_BASE         	(0x80029000)
#define RTC_BASE         	(0x8002C000)
#define SEJ_BASE         	(0x8002D000)
#define I2C3_BASE         	(0x8002E000)
#define IRDA_BASE       	(0x8002F000)


#define I2C_BASE         	(0x80030000)
#define MSDC1_BASE          (0x80031000)

#define NFI_BASE         	(0x80032000)
#define SIM_BASE 	      	(0x80033000)
#define MSDC2_BASE       	(0x80034000)
#define I2C2_BASE        	(0x80035000)
#define CCIF_BASE        	(0x80036000)
#define NFIECC_BASE	        (0x80038000)
#define AMCONFG_BASE     	(0x80039000)
#define AP2MD_BASE	     	(0x8003A000)
#define APVFE_BASE	     	(0x8003B000)
#define APSLP_BASE	     	(0x8003C000)
#define AUXADC_BASE	     	(0x8003D000)
#define APXGPT_BASE	     	(0x8003E000)
#define MSDC3_BASE       	(0x8003F000)


#define CSDBG_BASE			(0x80040000)


#define PLL_BASE        	(0x80060000)
#define DSI_PHY_BASE        (0x80060B00)
#define PMU_BASE        	(0x80061300)


#define GMC1_BASE			(0x80080000)
#define G2D_BASE			(0x80081000)
#define GCMQ_BASE			(0x80082000)
#define GIFDEC_BASE			(0x80083000)
#define IMGDMA_BASE			(0x80084000)
#define PNGDEC_BASE			(0x80085000)
#define MTVSPI_BASE			(0x80087000)
#define TVCON_BASE			(0x80088000)
#define TVENC_BASE			(0x80089000)
#define CAM_BASE			(0x8008A000)
#define CAM_ISP_BASE		(0x8008B000)
#define BLS_BASE			(0x8008C000)
#define CRZ_BASE			(0x8008D000)
#define DRZ_BASE			(0x8008E000)
#define ASM_BASE			(0x8008F000)


#define WT_BASE				 (0x80090000)
#define IMG_BASE			 (0x80091000)
#define GRAPH1SYS_CONFG_BASE (0x80092000)


#define JPEG_BASE			 (0x800A1000)
#define M3D_BASE			 (0x800A2000)
#define PRZ_BASE			 (0x800A3000)
#define IMGDMA1_BASE		 (0x800A4000)
#define MP4_DEBLK_BASE		 (0x800A5000)
#define FAKE_ENG2_BASE		 (0x800A6000)
#define GRAPH2SYS_BASE		 (0x800A7000)


#define MP4_BASE			 (0x800C0000)
#define H264_BASE			 (0x800C1000)


#define USB_BASE            (0x80100000)


#define LCD_BASE           	(0x80120000)


#define DPI_BASE            (0x80130000)

/* EMI Registers */
#define EMI_CON0 					(EMI_BASE+0x0000) /* Bank 0 configuration */
#define EMI_CON1 					(EMI_BASE+0x0004) /* Bank 1 configuration */
#define EMI_CON2 					(EMI_BASE+0x0008) /* Bank 2 configuration */
#define EMI_CON3 					(EMI_BASE+0x000C) /* Bank 3 configuration */
#define EMI_CON4 					(EMI_BASE+0x0010) /* Boot Mapping config  */
#define	EMI_CON5 					(EMI_BASE+0x0014)
#define SDRAM_MODE 					(EMI_BASE+0x0020)
#define SDRAM_COMD 					(EMI_BASE+0x0024)
#define SDRAM_SET 					(EMI_BASE+0x0028)
#define SDRAM_BASE					0x00000000


//----------------------------------------------------------------------------
#define MT6516_AP_MCU_SYS           (AMCONFG_BASE + 0x340)



//----------------------------------------------------------------------------
// GPT - Gerneral Purpose Timer Registers

#define MT6516_XGPT_IRQEN	    (APXGPT_BASE + 0x00)
#define MT6516_XGPT_IRQSTA      (APXGPT_BASE + 0x04)
#define MT6516_XGPT_IRQACK      (APXGPT_BASE + 0x08)
#define MT6516_XGPT1_CON        (APXGPT_BASE + 0x10)
#define MT6516_XGPT1_PRESCALE   (APXGPT_BASE + 0x14)
#define MT6516_XGPT1_COUNT      (APXGPT_BASE + 0x18)
#define MT6516_XGPT1_COMPARE    (APXGPT_BASE + 0x1c)
#define MT6516_XGPT2_CON        (APXGPT_BASE + 0x20)
#define MT6516_XGPT2_PRESCALE   (APXGPT_BASE + 0x24)
#define MT6516_XGPT2_COUNT      (APXGPT_BASE + 0x28)
#define MT6516_XGPT2_COMPARE    (APXGPT_BASE + 0x2c)
#define MT6516_XGPT3_CON        (APXGPT_BASE + 0x30)
#define MT6516_XGPT3_PRESCALE   (APXGPT_BASE + 0x34)
#define MT6516_XGPT3_COUNT      (APXGPT_BASE + 0x38)
#define MT6516_XGPT3_COMPARE    (APXGPT_BASE + 0x3c)
#define MT6516_XGPT4_CON        (APXGPT_BASE + 0x40)
#define MT6516_XGPT4_PRESCALE   (APXGPT_BASE + 0x44)
#define MT6516_XGPT4_COUNT      (APXGPT_BASE + 0x48)
#define MT6516_XGPT4_COMPARE    (APXGPT_BASE + 0x4c)
#define MT6516_XGPT5_CON        (APXGPT_BASE + 0x50)
#define MT6516_XGPT5_PRESCALE   (APXGPT_BASE + 0x54)
#define MT6516_XGPT5_COUNT      (APXGPT_BASE + 0x58)
#define MT6516_XGPT5_COMPARE    (APXGPT_BASE + 0x5c)
#define MT6516_XGPT6_CON        (APXGPT_BASE + 0x60)
#define MT6516_XGPT6_PRESCALE   (APXGPT_BASE + 0x64)
#define MT6516_XGPT6_COUNT      (APXGPT_BASE + 0x68)
#define MT6516_XGPT6_COMPARE    (APXGPT_BASE + 0x6c)
#define MT6516_XGPT7_CON        (APXGPT_BASE + 0x70)
#define MT6516_XGPT7_PRESCALE   (APXGPT_BASE + 0x74)
#define MT6516_XGPT7_COUNT      (APXGPT_BASE + 0x78)
#define MT6516_XGPT7_COMPARE    (APXGPT_BASE + 0x7c)



/* MT6516 EMI freq. definition */
#define EMI_52MHZ                   52000000
#define EMI_58_5MHZ                 58500000
#define EMI_104MHZ                  104000000
#define EMI_117MHZ                  117000000
#define EMI_130MHZ                  130000000

/* MT6516 clock definitions */
#define CPU_468MHZ_EMI_117MHZ       1
#define CPU_234MHZ_EMI_117MHZ       2
#define CPU_416MHZ_EMI_104MHZ       3
#define CPU_208MHZ_EMI_104MHZ       4
#define CPU_468MHZ_EMI_58_5MHZ      5
#define CPU_234MHZ_EMI_58_5MHZ      6
#define CPU_416MHZ_EMI_52MHZ        7
#define CPU_208MHZ_EMI_52MHZ        8
#define CPU_390MHZ_EMI_130MHZ       9



#define GPIO_MODE_BASE  (GPIO_BASE+0x0600)

 //APMCUSYS_PDN_CLR0
// clock and PDN register
#define APMCUSYS_PDN_CON0              (AMCONFG_BASE+0x0300)
#define APMCUSYS_PDN_SET0              (AMCONFG_BASE+0x0320)
#define APMCUSYS_PDN_CLR0              (AMCONFG_BASE+0x0340)

#define APMCUSYS_PDN_CON1              (AMCONFG_BASE+0x0360)
#define APMCUSYS_PDN_SET1              (AMCONFG_BASE+0x0380)
#define APMCUSYS_PDN_CLR1              (AMCONFG_BASE+0x03A0)


typedef enum
{
    PDN_PERI_DMA        =       0,
    PDN_PERI_USB        =       1,
    PDN_PERI_SEJ        =       2,
    PDN_PERI_I2C3       =       3,
    PDN_PERI_GPT        =       4,
    PDN_PERI_KP         =       5,
    PDN_PERI_GPIO       =       6,
    PDN_PERI_UART1      =       7,
    PDN_PERI_UART2      =       8,
    PDN_PERI_UART3      =       9,
    PDN_PERI_SIM        =       10,
    PDN_PERI_PWM        =       11,
    PDN_PERI_PWM1       =       12,
    PDN_PERI_PWM2       =       13,
    PDN_PERI_PWM3       =       14,
    PDN_PERI_MSDC       =       15,
    PDN_PERI_SWDBG      =       16,
    PDN_PERI_NFI        =       17,
    PDN_PERI_I2C2       =       18,
    PDN_PERI_IRDA       =       19,
    PDN_PERI_I2C        =       21,
    PDN_PERI_SIM2       =       22,
    PDN_PERI_MSDC2      =       23,
    PDN_PERI_ADC        =       26,
    PDN_PERI_TP         =       27,
    PDN_PERI_XGPT       =       28,
    PDN_PERI_UART4      =       29,
    PDN_PERI_MSDC3      =       30,
    PDN_PERI_ONEWIRE    =       31

}APMCUSYS_PDN0;

#define HW_VER              (CONFIG_BASE+0x0000)
#define SW_VER              (CONFIG_BASE+0x0004)
#define HW_CODE             (CONFIG_BASE+0x0008)
#define SW_MISC_L           (CONFIG_BASE+0x0010)
#define SW_MISC_H           (CONFIG_BASE+0x0014)
#define HW_MISC             (CONFIG_BASE+0x0020)


#endif
