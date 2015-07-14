#ifndef OXYGEN_REGS_H_INCLUDED
#define OXYGEN_REGS_H_INCLUDED

/* recording channel A */
#define OXYGEN_DMA_A_ADDRESS        0x00    /* 32-bit base address */
#define OXYGEN_DMA_A_COUNT        0x04    /* buffer counter (dwords) */
#define OXYGEN_DMA_A_TCOUNT        0x06    /* interrupt counter (dwords) */

/* recording channel B */
#define OXYGEN_DMA_B_ADDRESS        0x08
#define OXYGEN_DMA_B_COUNT        0x0c
#define OXYGEN_DMA_B_TCOUNT        0x0e

/* recording channel C */
#define OXYGEN_DMA_C_ADDRESS        0x10
#define OXYGEN_DMA_C_COUNT        0x14
#define OXYGEN_DMA_C_TCOUNT        0x16

/* SPDIF playback channel */
#define OXYGEN_DMA_SPDIF_ADDRESS    0x18
#define OXYGEN_DMA_SPDIF_COUNT        0x1c
#define OXYGEN_DMA_SPDIF_TCOUNT        0x1e

/* multichannel playback channel */
#define OXYGEN_DMA_MULTICH_ADDRESS    0x20
#define OXYGEN_DMA_MULTICH_COUNT    0x24    /* 24 bits */
#define OXYGEN_DMA_MULTICH_TCOUNT    0x28    /* 24 bits */

/* AC'97 (front panel) playback channel */
#define OXYGEN_DMA_AC97_ADDRESS        0x30
#define OXYGEN_DMA_AC97_COUNT        0x34
#define OXYGEN_DMA_AC97_TCOUNT        0x36

/* all registers 0x00..0x36 return current position on read */

#define OXYGEN_DMA_STATUS        0x40    /* 1 = running, 0 = stop */
#define  OXYGEN_CHANNEL_A        0x01
#define  OXYGEN_CHANNEL_B        0x02
#define  OXYGEN_CHANNEL_C        0x04
#define  OXYGEN_CHANNEL_SPDIF        0x08
#define  OXYGEN_CHANNEL_MULTICH        0x10
#define  OXYGEN_CHANNEL_AC97        0x20

#define OXYGEN_DMA_PAUSE        0x41    /* 1 = pause */
/* OXYGEN_CHANNEL_* */

#define OXYGEN_DMA_RESET        0x42
/* OXYGEN_CHANNEL_* */

#define OXYGEN_PLAY_CHANNELS        0x43
#define  OXYGEN_PLAY_CHANNELS_MASK    0x03
#define  OXYGEN_PLAY_CHANNELS_2        0x00
#define  OXYGEN_PLAY_CHANNELS_4        0x01
#define  OXYGEN_PLAY_CHANNELS_6        0x02
#define  OXYGEN_PLAY_CHANNELS_8        0x03
#define  OXYGEN_DMA_A_BURST_MASK    0x04
#define  OXYGEN_DMA_A_BURST_8        0x00    /* dwords */
#define  OXYGEN_DMA_A_BURST_16        0x04
#define  OXYGEN_DMA_MULTICH_BURST_MASK    0x08
#define  OXYGEN_DMA_MULTICH_BURST_8    0x00
#define  OXYGEN_DMA_MULTICH_BURST_16    0x08

#define OXYGEN_INTERRUPT_MASK        0x44
/* OXYGEN_CHANNEL_* */
#define  OXYGEN_INT_SPDIF_IN_DETECT    0x0100
#define  OXYGEN_INT_MCU            0x0200
#define  OXYGEN_INT_2WIRE        0x0400
#define  OXYGEN_INT_GPIO        0x0800
#define  OXYGEN_INT_MCB            0x2000
#define  OXYGEN_INT_AC97        0x4000

#define OXYGEN_INTERRUPT_STATUS        0x46
/* OXYGEN_CHANNEL_* amd OXYGEN_INT_* */
#define  OXYGEN_INT_MIDI        0x1000

#define OXYGEN_MISC            0x48
#define  OXYGEN_MISC_WRITE_PCI_SUBID    0x01
#define  OXYGEN_MISC_LATENCY_3F        0x02
#define  OXYGEN_MISC_REC_C_FROM_SPDIF    0x04
#define  OXYGEN_MISC_REC_B_FROM_AC97    0x08
#define  OXYGEN_MISC_REC_A_FROM_MULTICH    0x10
#define  OXYGEN_MISC_PCI_MEM_W_1_CLOCK    0x20
#define  OXYGEN_MISC_MIDI        0x40
#define  OXYGEN_MISC_CRYSTAL_MASK    0x80
#define  OXYGEN_MISC_CRYSTAL_24576    0x00
#define  OXYGEN_MISC_CRYSTAL_27        0x80    /* MHz */

#define OXYGEN_REC_FORMAT        0x4a
#define  OXYGEN_REC_FORMAT_A_MASK    0x03
#define  OXYGEN_REC_FORMAT_A_SHIFT    0
#define  OXYGEN_REC_FORMAT_B_MASK    0x0c
#define  OXYGEN_REC_FORMAT_B_SHIFT    2
#define  OXYGEN_REC_FORMAT_C_MASK    0x30
#define  OXYGEN_REC_FORMAT_C_SHIFT    4
#define  OXYGEN_FORMAT_16        0x00
#define  OXYGEN_FORMAT_24        0x01
#define  OXYGEN_FORMAT_32        0x02

#define OXYGEN_PLAY_FORMAT        0x4b
#define  OXYGEN_SPDIF_FORMAT_MASK    0x03
#define  OXYGEN_SPDIF_FORMAT_SHIFT    0
#define  OXYGEN_MULTICH_FORMAT_MASK    0x0c
#define  OXYGEN_MULTICH_FORMAT_SHIFT    2
/* OXYGEN_FORMAT_* */

#define OXYGEN_REC_CHANNELS        0x4c
#define  OXYGEN_REC_CHANNELS_MASK    0x07
#define  OXYGEN_REC_CHANNELS_2_2_2    0x00    /* DMA A, B, C */
#define  OXYGEN_REC_CHANNELS_4_2_2    0x01
#define  OXYGEN_REC_CHANNELS_6_0_2    0x02
#define  OXYGEN_REC_CHANNELS_6_2_0    0x03
#define  OXYGEN_REC_CHANNELS_8_0_0    0x04

#define OXYGEN_FUNCTION            0x50
#define  OXYGEN_FUNCTION_CLOCK_MASK    0x01
#define  OXYGEN_FUNCTION_CLOCK_PLL    0x00
#define  OXYGEN_FUNCTION_CLOCK_CRYSTAL    0x01
#define  OXYGEN_FUNCTION_RESET_CODEC    0x02
#define  OXYGEN_FUNCTION_RESET_POL    0x04
#define  OXYGEN_FUNCTION_PWDN        0x08
#define  OXYGEN_FUNCTION_PWDN_EN    0x10
#define  OXYGEN_FUNCTION_PWDN_POL    0x20
#define  OXYGEN_FUNCTION_2WIRE_SPI_MASK    0x40
#define  OXYGEN_FUNCTION_SPI        0x00
#define  OXYGEN_FUNCTION_2WIRE        0x40
#define  OXYGEN_FUNCTION_ENABLE_SPI_4_5    0x80    /* 0 = EEPROM */

#define OXYGEN_I2S_MULTICH_FORMAT    0x60
#define  OXYGEN_I2S_RATE_MASK        0x0007    /* LRCK */
#define  OXYGEN_RATE_32000        0x0000
#define  OXYGEN_RATE_44100        0x0001
#define  OXYGEN_RATE_48000        0x0002
#define  OXYGEN_RATE_64000        0x0003
#define  OXYGEN_RATE_88200        0x0004
#define  OXYGEN_RATE_96000        0x0005
#define  OXYGEN_RATE_176400        0x0006
#define  OXYGEN_RATE_192000        0x0007
#define  OXYGEN_I2S_FORMAT_MASK        0x0008
#define  OXYGEN_I2S_FORMAT_I2S        0x0000
#define  OXYGEN_I2S_FORMAT_LJUST    0x0008
#define  OXYGEN_I2S_MCLK_MASK        0x0030    /* MCLK/LRCK */
#define  OXYGEN_I2S_MCLK_128        0x0000
#define  OXYGEN_I2S_MCLK_256        0x0010
#define  OXYGEN_I2S_MCLK_512        0x0020
#define  OXYGEN_I2S_BITS_MASK        0x00c0
#define  OXYGEN_I2S_BITS_16        0x0000
#define  OXYGEN_I2S_BITS_20        0x0040
#define  OXYGEN_I2S_BITS_24        0x0080
#define  OXYGEN_I2S_BITS_32        0x00c0
#define  OXYGEN_I2S_MASTER        0x0100
#define  OXYGEN_I2S_BCLK_MASK        0x0600    /* BCLK/LRCK */
#define  OXYGEN_I2S_BCLK_64        0x0000
#define  OXYGEN_I2S_BCLK_128        0x0200
#define  OXYGEN_I2S_BCLK_256        0x0400
#define  OXYGEN_I2S_MUTE_MCLK        0x0800

#define OXYGEN_I2S_A_FORMAT        0x62
#define OXYGEN_I2S_B_FORMAT        0x64
#define OXYGEN_I2S_C_FORMAT        0x66
/* like OXYGEN_I2S_MULTICH_FORMAT */

#define OXYGEN_SPDIF_CONTROL        0x70
#define  OXYGEN_SPDIF_OUT_ENABLE    0x00000002
#define  OXYGEN_SPDIF_LOOPBACK        0x00000004    /* in to out */
#define  OXYGEN_SPDIF_SENSE_MASK    0x00000008
#define  OXYGEN_SPDIF_LOCK_MASK        0x00000010
#define  OXYGEN_SPDIF_RATE_MASK        0x00000020
#define  OXYGEN_SPDIF_SPDVALID        0x00000040
#define  OXYGEN_SPDIF_SENSE_PAR        0x00000200
#define  OXYGEN_SPDIF_LOCK_PAR        0x00000400
#define  OXYGEN_SPDIF_SENSE_STATUS    0x00000800
#define  OXYGEN_SPDIF_LOCK_STATUS    0x00001000
#define  OXYGEN_SPDIF_SENSE_INT        0x00002000    /* r/wc */
#define  OXYGEN_SPDIF_LOCK_INT        0x00004000    /* r/wc */
#define  OXYGEN_SPDIF_RATE_INT        0x00008000    /* r/wc */
#define  OXYGEN_SPDIF_IN_CLOCK_MASK    0x00010000
#define  OXYGEN_SPDIF_IN_CLOCK_96    0x00000000    /* <= 96 kHz */
#define  OXYGEN_SPDIF_IN_CLOCK_192    0x00010000    /* > 96 kHz */
#define  OXYGEN_SPDIF_OUT_RATE_MASK    0x07000000
#define  OXYGEN_SPDIF_OUT_RATE_SHIFT    24
/* OXYGEN_RATE_* << OXYGEN_SPDIF_OUT_RATE_SHIFT */

#define OXYGEN_SPDIF_OUTPUT_BITS    0x74
#define  OXYGEN_SPDIF_NONAUDIO        0x00000002
#define  OXYGEN_SPDIF_C            0x00000004
#define  OXYGEN_SPDIF_PREEMPHASIS    0x00000008
#define  OXYGEN_SPDIF_CATEGORY_MASK    0x000007f0
#define  OXYGEN_SPDIF_CATEGORY_SHIFT    4
#define  OXYGEN_SPDIF_ORIGINAL        0x00000800
#define  OXYGEN_SPDIF_CS_RATE_MASK    0x0000f000
#define  OXYGEN_SPDIF_CS_RATE_SHIFT    12
#define  OXYGEN_SPDIF_V            0x00010000    /* 0 = valid */

#define OXYGEN_SPDIF_INPUT_BITS        0x78
/* 32 bits, IEC958_AES_* */

#define OXYGEN_EEPROM_CONTROL        0x80
#define  OXYGEN_EEPROM_ADDRESS_MASK    0x7f
#define  OXYGEN_EEPROM_DIR_MASK        0x80
#define  OXYGEN_EEPROM_DIR_READ        0x00
#define  OXYGEN_EEPROM_DIR_WRITE    0x80

#define OXYGEN_EEPROM_STATUS        0x81
#define  OXYGEN_EEPROM_VALID        0x40
#define  OXYGEN_EEPROM_BUSY        0x80

#define OXYGEN_EEPROM_DATA        0x82    /* 16 bits */

#define OXYGEN_2WIRE_CONTROL        0x90
#define  OXYGEN_2WIRE_DIR_MASK        0x01
#define  OXYGEN_2WIRE_DIR_WRITE        0x00
#define  OXYGEN_2WIRE_DIR_READ        0x01
#define  OXYGEN_2WIRE_ADDRESS_MASK    0xfe    /* slave device address */
#define  OXYGEN_2WIRE_ADDRESS_SHIFT    1

#define OXYGEN_2WIRE_MAP        0x91    /* address, 8 bits */
#define OXYGEN_2WIRE_DATA        0x92    /* data, 16 bits */

#define OXYGEN_2WIRE_BUS_STATUS        0x94
#define  OXYGEN_2WIRE_BUSY        0x0001
#define  OXYGEN_2WIRE_LENGTH_MASK    0x0002
#define  OXYGEN_2WIRE_LENGTH_8        0x0000
#define  OXYGEN_2WIRE_LENGTH_16        0x0002
#define  OXYGEN_2WIRE_MANUAL_READ    0x0004    /* 0 = auto read */
#define  OXYGEN_2WIRE_WRITE_MAP_ONLY    0x0008
#define  OXYGEN_2WIRE_SLAVE_AD_MASK    0x0030    /* AD0, AD1 */
#define  OXYGEN_2WIRE_INTERRUPT_MASK    0x0040    /* 0 = int. if not responding */
#define  OXYGEN_2WIRE_SLAVE_NO_RESPONSE    0x0080
#define  OXYGEN_2WIRE_SPEED_MASK    0x0100
#define  OXYGEN_2WIRE_SPEED_STANDARD    0x0000
#define  OXYGEN_2WIRE_SPEED_FAST    0x0100
#define  OXYGEN_2WIRE_CLOCK_SYNC    0x0200
#define  OXYGEN_2WIRE_BUS_RESET        0x0400

#define OXYGEN_SPI_CONTROL        0x98
#define  OXYGEN_SPI_BUSY        0x01    /* read */
#define  OXYGEN_SPI_TRIGGER        0x01    /* write */
#define  OXYGEN_SPI_DATA_LENGTH_MASK    0x02
#define  OXYGEN_SPI_DATA_LENGTH_2    0x00
#define  OXYGEN_SPI_DATA_LENGTH_3    0x02
#define  OXYGEN_SPI_CLOCK_MASK        0xc0
#define  OXYGEN_SPI_CLOCK_160        0x00    /* ns */
#define  OXYGEN_SPI_CLOCK_320        0x40
#define  OXYGEN_SPI_CLOCK_640        0x80
#define  OXYGEN_SPI_CLOCK_1280        0xc0
#define  OXYGEN_SPI_CODEC_MASK        0x70    /* 0..5 */
#define  OXYGEN_SPI_CODEC_SHIFT        4
#define  OXYGEN_SPI_CEN_MASK        0x80
#define  OXYGEN_SPI_CEN_LATCH_CLOCK_LO    0x00
#define  OXYGEN_SPI_CEN_LATCH_CLOCK_HI    0x80

#define OXYGEN_SPI_DATA1        0x99
#define OXYGEN_SPI_DATA2        0x9a
#define OXYGEN_SPI_DATA3        0x9b

#define OXYGEN_MPU401            0xa0

#define OXYGEN_MPU401_CONTROL        0xa2
#define  OXYGEN_MPU401_LOOPBACK        0x01    /* TXD to RXD */

#define OXYGEN_GPI_DATA            0xa4
/* bits 0..5 = pin XGPI0..XGPI5 */

#define OXYGEN_GPI_INTERRUPT_MASK    0xa5
/* bits 0..5, 1 = enable */

#define OXYGEN_GPIO_DATA        0xa6
/* bits 0..9 */

#define OXYGEN_GPIO_CONTROL        0xa8
/* bits 0..9, 0 = input, 1 = output */
#define  OXYGEN_GPIO1_XSLAVE_RDY    0x8000

#define OXYGEN_GPIO_INTERRUPT_MASK    0xaa
/* bits 0..9, 1 = enable */

#define OXYGEN_DEVICE_SENSE        0xac
#define  OXYGEN_HEAD_PHONE_DETECT    0x01
#define  OXYGEN_HEAD_PHONE_MASK        0x06
#define  OXYGEN_HEAD_PHONE_PASSIVE_SPK    0x00
#define  OXYGEN_HEAD_PHONE_HP        0x02
#define  OXYGEN_HEAD_PHONE_ACTIVE_SPK    0x04

#define OXYGEN_MCU_2WIRE_DATA        0xb0

#define OXYGEN_MCU_2WIRE_MAP        0xb2

#define OXYGEN_MCU_2WIRE_STATUS        0xb3
#define  OXYGEN_MCU_2WIRE_BUSY        0x01
#define  OXYGEN_MCU_2WIRE_LENGTH_MASK    0x06
#define  OXYGEN_MCU_2WIRE_LENGTH_1    0x00
#define  OXYGEN_MCU_2WIRE_LENGTH_2    0x02
#define  OXYGEN_MCU_2WIRE_LENGTH_3    0x04
#define  OXYGEN_MCU_2WIRE_WRITE        0x08    /* r/wc */
#define  OXYGEN_MCU_2WIRE_READ        0x10    /* r/wc */
#define  OXYGEN_MCU_2WIRE_DRV_XACT_FAIL    0x20    /* r/wc */
#define  OXYGEN_MCU_2WIRE_RESET        0x40

#define OXYGEN_MCU_2WIRE_CONTROL    0xb4
#define  OXYGEN_MCU_2WIRE_DRV_ACK    0x01
#define  OXYGEN_MCU_2WIRE_DRV_XACT    0x02
#define  OXYGEN_MCU_2WIRE_INT_MASK    0x04
#define  OXYGEN_MCU_2WIRE_SYNC_MASK    0x08
#define  OXYGEN_MCU_2WIRE_SYNC_RDY_PIN    0x00
#define  OXYGEN_MCU_2WIRE_SYNC_DATA    0x08
#define  OXYGEN_MCU_2WIRE_ADDRESS_MASK    0x30
#define  OXYGEN_MCU_2WIRE_ADDRESS_10    0x00
#define  OXYGEN_MCU_2WIRE_ADDRESS_12    0x10
#define  OXYGEN_MCU_2WIRE_ADDRESS_14    0x20
#define  OXYGEN_MCU_2WIRE_ADDRESS_16    0x30
#define  OXYGEN_MCU_2WIRE_INT_POL    0x40
#define  OXYGEN_MCU_2WIRE_SYNC_ENABLE    0x80

#define OXYGEN_PLAY_ROUTING        0xc0
#define  OXYGEN_PLAY_MUTE01        0x0001
#define  OXYGEN_PLAY_MUTE23        0x0002
#define  OXYGEN_PLAY_MUTE45        0x0004
#define  OXYGEN_PLAY_MUTE67        0x0008
#define  OXYGEN_PLAY_MULTICH_MASK    0x0010
#define  OXYGEN_PLAY_MULTICH_I2S_DAC    0x0000
#define  OXYGEN_PLAY_MULTICH_AC97    0x0010
#define  OXYGEN_PLAY_SPDIF_MASK        0x00e0
#define  OXYGEN_PLAY_SPDIF_SPDIF    0x0000
#define  OXYGEN_PLAY_SPDIF_MULTICH_01    0x0020
#define  OXYGEN_PLAY_SPDIF_MULTICH_23    0x0040
#define  OXYGEN_PLAY_SPDIF_MULTICH_45    0x0060
#define  OXYGEN_PLAY_SPDIF_MULTICH_67    0x0080
#define  OXYGEN_PLAY_SPDIF_REC_A    0x00a0
#define  OXYGEN_PLAY_SPDIF_REC_B    0x00c0
#define  OXYGEN_PLAY_SPDIF_I2S_ADC_3    0x00e0
#define  OXYGEN_PLAY_DAC0_SOURCE_MASK    0x0300
#define  OXYGEN_PLAY_DAC0_SOURCE_SHIFT    8
#define  OXYGEN_PLAY_DAC1_SOURCE_MASK    0x0c00
#define  OXYGEN_PLAY_DAC1_SOURCE_SHIFT    10
#define  OXYGEN_PLAY_DAC2_SOURCE_MASK    0x3000
#define  OXYGEN_PLAY_DAC2_SOURCE_SHIFT    12
#define  OXYGEN_PLAY_DAC3_SOURCE_MASK    0xc000
#define  OXYGEN_PLAY_DAC3_SOURCE_SHIFT    14

#define OXYGEN_REC_ROUTING        0xc2
#define  OXYGEN_MUTE_I2S_ADC_1        0x01
#define  OXYGEN_MUTE_I2S_ADC_2        0x02
#define  OXYGEN_MUTE_I2S_ADC_3        0x04
#define  OXYGEN_REC_A_ROUTE_MASK    0x08
#define  OXYGEN_REC_A_ROUTE_I2S_ADC_1    0x00
#define  OXYGEN_REC_A_ROUTE_AC97_0    0x08
#define  OXYGEN_REC_B_ROUTE_MASK    0x10
#define  OXYGEN_REC_B_ROUTE_I2S_ADC_2    0x00
#define  OXYGEN_REC_B_ROUTE_AC97_1    0x10
#define  OXYGEN_REC_C_ROUTE_MASK    0x20
#define  OXYGEN_REC_C_ROUTE_SPDIF    0x00
#define  OXYGEN_REC_C_ROUTE_I2S_ADC_3    0x20

#define OXYGEN_ADC_MONITOR        0xc3
#define  OXYGEN_ADC_MONITOR_A        0x01
#define  OXYGEN_ADC_MONITOR_A_HALF_VOL    0x02
#define  OXYGEN_ADC_MONITOR_B        0x04
#define  OXYGEN_ADC_MONITOR_B_HALF_VOL    0x08
#define  OXYGEN_ADC_MONITOR_C        0x10
#define  OXYGEN_ADC_MONITOR_C_HALF_VOL    0x20

#define OXYGEN_A_MONITOR_ROUTING    0xc4
#define  OXYGEN_A_MONITOR_ROUTE_0_MASK    0x03
#define  OXYGEN_A_MONITOR_ROUTE_0_SHIFT    0
#define  OXYGEN_A_MONITOR_ROUTE_1_MASK    0x0c
#define  OXYGEN_A_MONITOR_ROUTE_1_SHIFT    2
#define  OXYGEN_A_MONITOR_ROUTE_2_MASK    0x30
#define  OXYGEN_A_MONITOR_ROUTE_2_SHIFT    4
#define  OXYGEN_A_MONITOR_ROUTE_3_MASK    0xc0
#define  OXYGEN_A_MONITOR_ROUTE_3_SHIFT    6

#define OXYGEN_AC97_CONTROL        0xd0
#define  OXYGEN_AC97_COLD_RESET        0x0001
#define  OXYGEN_AC97_SUSPENDED        0x0002    /* read */
#define  OXYGEN_AC97_RESUME        0x0002    /* write */
#define  OXYGEN_AC97_CLOCK_DISABLE    0x0004
#define  OXYGEN_AC97_NO_CODEC_0        0x0008
#define  OXYGEN_AC97_CODEC_0        0x0010
#define  OXYGEN_AC97_CODEC_1        0x0020

#define OXYGEN_AC97_INTERRUPT_MASK    0xd2
#define  OXYGEN_AC97_INT_READ_DONE    0x01
#define  OXYGEN_AC97_INT_WRITE_DONE    0x02
#define  OXYGEN_AC97_INT_CODEC_0    0x10
#define  OXYGEN_AC97_INT_CODEC_1    0x20

#define OXYGEN_AC97_INTERRUPT_STATUS    0xd3
/* OXYGEN_AC97_INT_* */

#define OXYGEN_AC97_OUT_CONFIG        0xd4
#define  OXYGEN_AC97_CODEC1_SLOT3    0x00000001
#define  OXYGEN_AC97_CODEC1_SLOT3_VSR    0x00000002
#define  OXYGEN_AC97_CODEC1_SLOT4    0x00000010
#define  OXYGEN_AC97_CODEC1_SLOT4_VSR    0x00000020
#define  OXYGEN_AC97_CODEC0_FRONTL    0x00000100
#define  OXYGEN_AC97_CODEC0_FRONTR    0x00000200
#define  OXYGEN_AC97_CODEC0_SIDEL    0x00000400
#define  OXYGEN_AC97_CODEC0_SIDER    0x00000800
#define  OXYGEN_AC97_CODEC0_CENTER    0x00001000
#define  OXYGEN_AC97_CODEC0_BASE    0x00002000
#define  OXYGEN_AC97_CODEC0_REARL    0x00004000
#define  OXYGEN_AC97_CODEC0_REARR    0x00008000

#define OXYGEN_AC97_IN_CONFIG        0xd8
#define  OXYGEN_AC97_CODEC1_LINEL    0x00000001
#define  OXYGEN_AC97_CODEC1_LINEL_VSR    0x00000002
#define  OXYGEN_AC97_CODEC1_LINEL_16    0x00000000
#define  OXYGEN_AC97_CODEC1_LINEL_18    0x00000004
#define  OXYGEN_AC97_CODEC1_LINEL_20    0x00000008
#define  OXYGEN_AC97_CODEC1_LINER    0x00000010
#define  OXYGEN_AC97_CODEC1_LINER_VSR    0x00000020
#define  OXYGEN_AC97_CODEC1_LINER_16    0x00000000
#define  OXYGEN_AC97_CODEC1_LINER_18    0x00000040
#define  OXYGEN_AC97_CODEC1_LINER_20    0x00000080
#define  OXYGEN_AC97_CODEC0_LINEL    0x00000100
#define  OXYGEN_AC97_CODEC0_LINER    0x00000200

#define OXYGEN_AC97_REGS        0xdc
#define  OXYGEN_AC97_REG_DATA_MASK    0x0000ffff
#define  OXYGEN_AC97_REG_ADDR_MASK    0x007f0000
#define  OXYGEN_AC97_REG_ADDR_SHIFT    16
#define  OXYGEN_AC97_REG_DIR_MASK    0x00800000
#define  OXYGEN_AC97_REG_DIR_WRITE    0x00000000
#define  OXYGEN_AC97_REG_DIR_READ    0x00800000
#define  OXYGEN_AC97_REG_CODEC_MASK    0x01000000
#define  OXYGEN_AC97_REG_CODEC_SHIFT    24

#define OXYGEN_TEST            0xe0
#define  OXYGEN_TEST_RAM_SUCCEEDED    0x01
#define  OXYGEN_TEST_PLAYBACK_RAM    0x02
#define  OXYGEN_TEST_RECORD_RAM        0x04
#define  OXYGEN_TEST_PLL        0x08
#define  OXYGEN_TEST_2WIRE_LOOPBACK    0x10

#define OXYGEN_DMA_FLUSH        0xe1
/* OXYGEN_CHANNEL_* */

#define OXYGEN_CODEC_VERSION        0xe4
#define  OXYGEN_XCID_MASK        0x07

#define OXYGEN_REVISION            0xe6
#define  OXYGEN_REVISION_XPKGID_MASK    0x0007
#define  OXYGEN_REVISION_MASK        0xfff8
#define  OXYGEN_REVISION_2        0x0008    /* bit flag */
#define  OXYGEN_REVISION_8787        0x0014    /* 8 bits */

#define OXYGEN_OFFSIN_48K        0xe8
#define OXYGEN_OFFSBASE_48K        0xe9
#define  OXYGEN_OFFSBASE_MASK        0x0fff
#define OXYGEN_OFFSIN_44K        0xec
#define OXYGEN_OFFSBASE_44K        0xed

#endif
