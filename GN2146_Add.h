
/////////////////////////////////////////////////////////////////////////////
#define	TX_CDR_BYPASS_CTRL_REG0						(unsigned short) 0x0000		
#define	TX_LOS_THRES_REG_0							(unsigned short) 0x0001		
#define	TX_LOS_HYST_REG_0							(unsigned short) 0x0002
#define	TX_POLARITY_INVERT_CTRL_REG_0				(unsigned short) 0x0006
#define	TX_DRV_FORCE_MUTE_REG_0						(unsigned short) 0x0012
#define	TX_DRV_MUTE_CTRL_REG_0						(unsigned short) 0x0013
#define	TX_DRV_PD_CTRL_REG_0						(unsigned short) 0x0014
#define	TX_LD_MAIN_SWING_REG_0						(unsigned short) 0x0018	//
#define	TX_LD_MAIN_SWING_REG_1						(unsigned short) 0x0019	//
#define	TX_LD_SWING_SEL_REG_0						(unsigned short) 0x001A	//
#define	TX_LD_CPA_CTRL_REG_0						(unsigned short) 0x001B
#define	TX_CTLE_BOOST_OVRD_REG_0					(unsigned short) 0x0030
#define	TX_PD_LANE_REG_0							(unsigned short) 0x004C
#define	TX_PD_LOS_REG_0								(unsigned short) 0x004D
#define	TX_PD_CDR_REG_0								(unsigned short) 0x0050
#define	TX_PD_DRV_REG_0								(unsigned short) 0x0051
#define	TX_PD_DRV_REG_1								(unsigned short) 0x0052	
//17, 14

#define	RX_CDR_BYPASS_CTRL_REG_0					(unsigned short) 0x0100
#define	RX_LOS_THRES_REG_0							(unsigned short) 0x0101
#define	RX_LOS_HYST_REG_0							(unsigned short) 0x0102
#define	RX_OFFSET_CORRECTION_CTRL_REG_0				(unsigned short) 0x0106
#define	RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0	(unsigned short) 0x0107
#define	RX_POLARITY_INVERT_CTRL_REG_0				(unsigned short) 0x0109	
#define	RX_EQ_BOOST_REG_0							(unsigned short) 0x010B
#define	RX_DRV_FORCE_MUTE_REG_0						(unsigned short) 0x0118
#define	RX_DRV_MUTE_CTRL_REG_0						(unsigned short) 0x0119
#define	RX_DRV_PD_CTRL_REG_0						(unsigned short) 0x011A
#define	RX_DRV_MAIN_SWING_REG_0						(unsigned short) 0x011B
#define	RX_DRV_INV_CTRL_REG_0						(unsigned short) 0x011C
#define	RX_DRV_DEEMPH_CTRL_REG_0					(unsigned short) 0x011D
#define	RX_DRV_PREDRV_CM_REF_REG_0					(unsigned short) 0x011F
#define	RX_PD_LANE_REG_0							(unsigned short) 0x0120
#define	RX_PD_LOS_REG_0								(unsigned short) 0x0121
#define	RX_PD_EQ_REG_0			 					(unsigned short) 0x0123
#define	RX_PD_CDR_REG_0								(unsigned short) 0x0124
#define	RX_PD_DRV_REG_0								(unsigned short) 0x0126
#define	RX_PD_DRV_REG_1								(unsigned short) 0x0127
//20

#define	VCO_RANGE_OVERRIDE_CTRL_REG_0				                (unsigned short) 0x0204
#define	RX_LOS_DET_CTRL_REG_0						        (unsigned short) 0x0206
#define	LOS_SOFT_ASSERT_CTRL_REG_0					        (unsigned short) 0x020A	
#define	LOS_CHANGE_DETECT_REG_0						        (unsigned short) 0x020C
#define	LOS_SOFT_ASSERT_CTRL_REG_1					        (unsigned short) 0x0212
#define	LOL_CHANGE_DETECT_REG_1						        (unsigned short) 0x0214
#define	LOSL_OUTPUT_CTRL_REG_0						        (unsigned short) 0x0216
#define	LOSL_OUTPUT_CTRL_REG_1						        (unsigned short) 0x0217   
#define	LOSL_STATUS_REG_0							(unsigned short) 0x0218		//RO
#define	LOSL_LATCHED_STATUS_REG_0					        (unsigned short) 0x0219		//RO
#define	CDR_28G_INIT_REG_0							(unsigned short) 0x0225
#define	TXDSBL_SOFT_REG_0							(unsigned short) 0x0234
#define	TXFAULT_GLOBAL_STATUS_REG_1					         (unsigned short) 0x0235		//RO
#define	TEMP_SENSOR_TRIM_REG_0						        (unsigned short) 0x0236		//RO
#define	TEMPERATURE_REG_0							(unsigned short) 0x0237		//RO
#define	TEMPERATURE_REG_1							(unsigned short) 0x0238		//RO
#define	VCCL_SUPPLY_REG_0							(unsigned short) 0x0239		//RO
#define	VCCL_SUPPLY_REG_1							(unsigned short) 0x023A		//RO
#define	TXVCCH_SUPPLY_REG_0							(unsigned short) 0x023B		//RO
#define	TXVCCH_SUPPLY_REG_1							(unsigned short) 0x023C		//RO
#define	ADC_PARAM_MON_CTRL_RESET_REG_0				                 (unsigned short) 0x0242
#define	ADC_PARAM_MON_CTRL_REG_0					        (unsigned short) 0x0245
#define	CDR_28G_INIT_REG_1							(unsigned short) 0x0255
#define	CDR_28G_INIT_REG_2							(unsigned short) 0x025E
#define	TX_BB_STEP_REG_0							(unsigned short) 0x0262
#define	RX_BB_STEP_REG_0							(unsigned short) 0x0263
#define	PD_ALL_REG_0								(unsigned short) 0x026A
#define	PD_SENSE_REG_0								(unsigned short) 0x026B		
#define	TX_EYE_SHAPING_CTRL_REG_0					        (unsigned short) 0x026E		
#define	VERSION_REG_0								(unsigned short) 0x026F		//RO
#define	TX_LD_MAIN_SWING_SLOPE_REG_0				                (unsigned short) 0x0291
#define	TX_LD_MAIN_SWING_SLOPE_REG_1				                (unsigned short) 0x0292
//31, RO 11

#define GN2146SyncCnt	69
#define GN2146ROCnt	16
#define GN2146IgROCnt	0



