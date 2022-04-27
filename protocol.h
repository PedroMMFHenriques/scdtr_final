#pragma once

// <sender ID> <msg size> <CMD> <data> ... <data>

#define BROADCAST 0x00

#define WAKEUP 0x20
#define MEASURE 0x21
#define CALIBRATE 0x22
#define DONE_CALIBRATE 0x23
#define MEASURE_ACK 0x24

#define CONSENSUS_START 0x77
#define CONSENSUS_SOLUTION 0x78


// PART I CMDS //
#define CMD_d  0x25
#define CMD_gd 0x26
#define CMD_r  0x27
#define CMD_gr 0x28
#define CMD_gl 0x29
#define CMD_o  0x2A
#define CMD_go 0x2B
#define CMD_a  0x2C
#define CMD_ga 0x2D
#define CMD_w  0x2E
#define CMD_gw 0x2F
#define CMD_b  0x30
#define CMD_gb 0x31
#define CMD_gx 0x32
#define CMD_gp 0x33
#define CMD_gt 0x34
#define CMD_sl 0x35
#define CMD_sd 0x36
#define CMD_Bl 0x37
#define CMD_Bd 0x38
#define CMD_ge 0x39
#define CMD_gv 0x3A
#define CMD_gf 0x3B

/* PARTE II */
#define CMD_gO 0x3F
#define CMD_gU 0x40
#define CMD_gL 0x41
#define CMD_gc 0x42
#define CMD_O  0x43
#define CMD_U  0x44
#define CMD_c  0x45
#define CMD_R  0x46

/**REPLIES**/

// PART I CMDS //
#define R_CMD_d  0x55
#define R_CMD_gd 0x56
#define R_CMD_r  0x57
#define R_CMD_gr 0x58
#define R_CMD_gl 0x59
#define R_CMD_o  0x5A
#define R_CMD_go 0x5B
#define R_CMD_a  0x5C
#define R_CMD_ga 0x5D
#define R_CMD_w  0x5E
#define R_CMD_gw 0x5F
#define R_CMD_b  0x60
#define R_CMD_gb 0x61
#define R_CMD_gx 0x62
#define R_CMD_gp 0x63
#define R_CMD_gt 0x64
#define R_CMD_sl 0x65
#define R_CMD_sd 0x66
#define R_CMD_Bl 0x67
#define R_CMD_Bd 0x68
#define R_CMD_ge 0x69
#define R_CMD_gv 0x6A
#define R_CMD_gf 0x6B


/* PARTE II */
#define R_CMD_gO 0x6F
#define R_CMD_gU 0x70
#define R_CMD_gL 0x71
#define R_CMD_gc 0x72
#define R_CMD_O  0x73
#define R_CMD_U  0x74
#define R_CMD_c  0x75
#define R_CMD_R  0x76