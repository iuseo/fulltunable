
static unsigned char GN2146SyncDefault[GN2146SyncCnt] = 
{
	0x04,		// TX_CDR_BYPASS_CTRL_REG0						//0x80					
	0x0A,		// TX_LOS_THRES_REG_0							//0x81
	0x01,		// TX_LOS_HYST_REG_0							//0x82
	0x00,		// TX_POLARITY_INVERT_CTRL_REG_0				//0x83
	0x00,		// TX_DRV_FORCE_MUTE_REG_0						//0x84
	0x01,		// TX_DRV_MUTE_CTRL_REG_0						//0x85
	0x02,		// TX_DRV_PD_CTRL_REG_0							//0x86
	0x00,		// TX_LD_MAIN_SWING_REG_0			//RO		//0x87				
	0x00,		// TX_LD_MAIN_SWING_REG_1			//RO		//0x88	
	0x00,		// TX_LD_SWING_SEL_REG_0			//RO		//0x89				
	0x50,		// TX_LD_CPA_CTRL_REG_0							//0x8A
	0x1F,		// TX_CTLE_BOOST_OVRD_REG_0						//0x8B
	0x00,		// TX_PD_LANE_REG_0								//0x8C
	0x00,		// TX_PD_LOS_REG_0								//0x8D
	0x00,		// TX_PD_CDR_REG_0								//0x8E
	0x00,		// TX_PD_DRV_REG_0								//0x8F
	0x01,		// TX_PD_DRV_REG_1		17						//0x90
	0x04,		// RX_CDR_BYPASS_CTRL_REG_0						//0x91
	0x12,		// RX_LOS_THRES_REG_0							//0x92
	0x06,		// RX_LOS_HYST_REG_0							//0x93
	0x02,		// RX_OFFSET_CORRECTION_CTRL_REG_0				//0x94
	0x7F,		// RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0		//0x95
	0x01,		// RX_POLARITY_INVERT_CTRL_REG_0				//0x96
	0x28,		// RX_EQ_BOOST_REG_0							//0x97
	0x00,		// RX_DRV_FORCE_MUTE_REG_0						//0x98
	0x01,		// RX_DRV_MUTE_CTRL_REG_0						//0x99
	0x00,		// RX_DRV_PD_CTRL_REG_0							//0x9A
	0x8C,		// RX_DRV_MAIN_SWING_REG_0						//0x9B
	0x00,		// RX_DRV_INV_CTRL_REG_0						//0x9C
	0x05,		// RX_DRV_DEEMPH_CTRL_REG_0						//0x9D
	0x25,		// RX_DRV_PREDRV_CM_REF_REG_0					//0x9E
	0x00,		// RX_PD_LANE_REG_0								//0x9F
	0x00,		// RX_PD_LOS_REG_0								//0xA0
	0x18,		// RX_PD_EQ_REG_0			 					//0xA1
	0x00,		// RX_PD_CDR_REG_0								//0xA2
	0x00,		// RX_PD_DRV_REG_0								//0xA3
	0x08,		// RX_PD_DRV_REG_1		20						//0xA4
	0x24,		// VCO_RANGE_OVERRIDE_CTRL_REG_0				//0xA5
	0x01,		// RX_LOS_DET_CTRL_REG_0						//0xA6
	0x00,		// LOS_SOFT_ASSERT_CTRL_REG_0					//0xA7
	0x00,		// LOS_CHANGE_DETECT_REG_0						//0xA8
	0x00,		// LOS_SOFT_ASSERT_CTRL_REG_1					//0xA9
	0x00,		// LOL_CHANGE_DETECT_REG_1						//0xAA
	0x02,		// LOSL_OUTPUT_CTRL_REG_0						//0xAB
	0x00,		// LOSL_OUTPUT_CTRL_REG_1						//0xAC
	0x00,		// LOSL_STATUS_REG_0				//RO		//0xAD			
	0x00,		// LOSL_LATCHED_STATUS_REG_0		//RO		//0xAE
	0x1F,		// CDR_28G_INIT_REG_0							//0xAF
	0x00,		// TXDSBL_SOFT_REG_0							//0xB0
	0x00,		// TXFAULT_GLOBAL_STATUS_REG_1		//RO		//0xB1
	0x00,		// TEMP_SENSOR_TRIM_REG_0			//RO		//0xB2
	0x00,		// TEMPERATURE_REG_0				//RO		//0xB3
	0x00,		// TEMPERATURE_REG_1				//RO		//0xB4
	0x00,		// VCCL_SUPPLY_REG_0				//RO		//0xB5
	0x00,		// VCCL_SUPPLY_REG_1				//RO		//0xB6
	0x00,		// TXVCCH_SUPPLY_REG_0				//RO		//0xB7
	0x00,		// TXVCCH_SUPPLY_REG_1				//RO		//0xB8
	0x00,		// ADC_PARAM_MON_CTRL_RESET_REG_0				//0xB9
	0x07,		// ADC_PARAM_MON_CTRL_REG_0						//0xBA
	0x5C,		// CDR_28G_INIT_REG_1							//0xBB
	0x5C,		// CDR_28G_INIT_REG_1							//0xBC
	0x46,		// TX_BB_STEP_REG_0								//0xBD
	0x46,		// RX_BB_STEP_REG_0								//0xBE
	0x00,		// PD_ALL_REG_0									//0xBF
	0x00,		// PD_SENSE_REG_0								//0xC0
	0x00,		// TX_EYE_SHAPING_CTRL_REG_0					//0xC1
	0x00,		// VERSION_REG_0					//RO		//0xC2				
	0xBB,		// TX_LD_MAIN_SWING_SLOPE_REG_0					//0xC3
	0x03		// TX_LD_MAIN_SWING_SLOPE_REG_1		28			//0xC4
};              		






