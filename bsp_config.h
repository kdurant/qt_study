#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

#define SAMPLEENABLE 0x0000000C
#define SAMPLELEN 0x00000008
#define SAMPLERATE 0x00000009
#define FIRSTSTARTPOS 0x00000022
#define FIRSTLEN 0x00000023
#define SECONDLEN 0x00000024
#define SECONDSTARTPOS 0x00000025
#define SUMTHRESHOLD 0x00000026
#define SUBTHRESHOLD 0x00000027

#define COMMAND_POS 24
#define COMMAND_LEN 8
#define PCK_NUMER_POS 32
#define PCK_NUMBER_LEN 8
#define VALID_LEN_POS 40
#define VALID_LEN_LEN 8

#define AD_DATA_POS 48

#define CHANNAL_0_FLAG "eb90a55a0000"
#define CHANNAL_1_FLAG "eb90a55a0f0f"
#define CHANNAL_2_FLAG "eb90a55af0f0"
#define CHANNAL_3_FLAG "eb90a55affff"

#endif  // BSP_CONFIG_H
