
const unsigned short GN2146ReadOnly[GN2146ROCnt] = 
{
	TX_LD_MAIN_SWING_REG_0				,
	TX_LD_MAIN_SWING_REG_1				,
	TX_LD_SWING_SEL_REG_0				,
	LOSL_STATUS_REG_0					,
	LOSL_LATCHED_STATUS_REG_0	        ,
	TXFAULT_GLOBAL_STATUS_REG_1         ,
        TEMP_SENSOR_TRIM_REG_0        		,
	TEMPERATURE_REG_0					,
	TEMPERATURE_REG_1					,	
	VCCL_SUPPLY_REG_0					,
	VCCL_SUPPLY_REG_1					,
	TXVCCH_SUPPLY_REG_0					,
	TXVCCH_SUPPLY_REG_1					,
	VERSION_REG_0                                           ,
        TX_LD_MAIN_SWING_SLOPE_REG_0				,			
	TX_LD_MAIN_SWING_SLOPE_REG_1	
};
const unsigned short GN2146Sync[GN2146SyncCnt] = 
{
	TX_CDR_BYPASS_CTRL_REG0						,
	TX_LOS_THRES_REG_0							,
	TX_LOS_HYST_REG_0							,
	TX_POLARITY_INVERT_CTRL_REG_0				,
	TX_DRV_FORCE_MUTE_REG_0						,
	TX_DRV_MUTE_CTRL_REG_0						,
	TX_DRV_PD_CTRL_REG_0						,
	TX_LD_MAIN_SWING_REG_0						,
	TX_LD_MAIN_SWING_REG_1						,
	TX_LD_SWING_SEL_REG_0						,
	TX_LD_CPA_CTRL_REG_0						,
	TX_CTLE_BOOST_OVRD_REG_0					,
	TX_PD_LANE_REG_0							,
	TX_PD_LOS_REG_0								,
	TX_PD_CDR_REG_0								,
	TX_PD_DRV_REG_0								,
	TX_PD_DRV_REG_1								,//17
	RX_CDR_BYPASS_CTRL_REG_0					,
	RX_LOS_THRES_REG_0							,   //19
	RX_LOS_HYST_REG_0							,
	RX_OFFSET_CORRECTION_CTRL_REG_0				,   //20
	RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0	,
	RX_POLARITY_INVERT_CTRL_REG_0				,
	RX_EQ_BOOST_REG_0							,
	RX_DRV_FORCE_MUTE_REG_0						,
	RX_DRV_MUTE_CTRL_REG_0						,
	RX_DRV_PD_CTRL_REG_0						,
	RX_DRV_MAIN_SWING_REG_0						,
	RX_DRV_INV_CTRL_REG_0						,
	RX_DRV_DEEMPH_CTRL_REG_0					,
	RX_DRV_PREDRV_CM_REF_REG_0					,
	RX_PD_LANE_REG_0							,
	RX_PD_LOS_REG_0								,					
	RX_PD_EQ_REG_0			 					,						
	RX_PD_CDR_REG_0								,						
	RX_PD_DRV_REG_0								,						
	RX_PD_DRV_REG_1								,				
	VCO_RANGE_OVERRIDE_CTRL_REG_0				,						
	RX_LOS_DET_CTRL_REG_0						,						
	LOS_SOFT_ASSERT_CTRL_REG_0					,						
	LOS_CHANGE_DETECT_REG_0						,						
	LOS_SOFT_ASSERT_CTRL_REG_1					,						
	LOL_CHANGE_DETECT_REG_1						,						
	LOSL_OUTPUT_CTRL_REG_0						,						
	LOSL_OUTPUT_CTRL_REG_1						,
	LOSL_STATUS_REG_0							,
	LOSL_LATCHED_STATUS_REG_0	        		,
	CDR_28G_INIT_REG_0							,
	TXDSBL_SOFT_REG_0							,
	TXFAULT_GLOBAL_STATUS_REG_1     	    	,
        TEMP_SENSOR_TRIM_REG_0        				,
	TEMPERATURE_REG_0							,
	TEMPERATURE_REG_1							,	
	VCCL_SUPPLY_REG_0							,
	VCCL_SUPPLY_REG_1							,
	TXVCCH_SUPPLY_REG_0							,
	TXVCCH_SUPPLY_REG_1							,
	ADC_PARAM_MON_CTRL_RESET_REG_0				,				
	ADC_PARAM_MON_CTRL_REG_0					,
	CDR_28G_INIT_REG_1							,
	CDR_28G_INIT_REG_2							,
        TX_BB_STEP_REG_0							,
        RX_BB_STEP_REG_0							,
        PD_ALL_REG_0								,
	PD_SENSE_REG_0								,
	TX_EYE_SHAPING_CTRL_REG_0					,
	VERSION_REG_0								,
	TX_LD_MAIN_SWING_SLOPE_REG_0				,			
	TX_LD_MAIN_SWING_SLOPE_REG_1							
};
	
	
	
const unsigned char GN2146ReadOnlybit[GN2146SyncCnt] = 
{
	0xF8,		// TX_CDR_BYPASS_CTRL_REG0						
	0xF0,		// TX_LOS_THRES_REG_0							
	0xFC,		// TX_LOS_HYST_REG_0							
	0xFC,		// TX_POLARITY_INVERT_CTRL_REG_0				
	0xFE,		// TX_DRV_FORCE_MUTE_REG_0						
	0xFE,		// TX_DRV_MUTE_CTRL_REG_0						
	0xF8,		// TX_DRV_PD_CTRL_REG_0						
	0xF8,		// TX_LD_MAIN_SWING_REG_0						
	0x80,		// TX_LD_MAIN_SWING_REG_1						
	0xFD,		// TX_LD_SWING_SEL_REG_0						
	0x80,		// TX_LD_CPA_CTRL_REG_0						
	0xC0,		// TX_CTLE_BOOST_OVRD_REG_0					
	0xFE,		// TX_PD_LANE_REG_0							
	0xFE,		// TX_PD_LOS_REG_0								
	0xFE,		// TX_PD_CDR_REG_0								
	0xFE,		// TX_PD_DRV_REG_0								
	0xFE,		// TX_PD_DRV_REG_1								//17
	0xF8,		// RX_CDR_BYPASS_CTRL_REG_0					
	0xC0,		// RX_LOS_THRES_REG_0							
	0xF8,		// RX_LOS_HYST_REG_0							
	0xF8,		// RX_OFFSET_CORRECTION_CTRL_REG_0				
	0x00,		// RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0	
	0xFC,		// RX_POLARITY_INVERT_CTRL_REG_0				
	0x80,		// RX_EQ_BOOST_REG_0							
	0xFE,		// RX_DRV_FORCE_MUTE_REG_0						
	0xFE,		// RX_DRV_MUTE_CTRL_REG_0						
	0xFE,		// RX_DRV_PD_CTRL_REG_0						
	0x00,		// RX_DRV_MAIN_SWING_REG_0						
	0xFE,		// RX_DRV_INV_CTRL_REG_0						
	0xF0,		// RX_DRV_DEEMPH_CTRL_REG_0					
	0x80,		// RX_DRV_PREDRV_CM_REF_REG_0					
	0xFE,		// RX_PD_LANE_REG_0							
	0xFE,		// RX_PD_LOS_REG_0								
	0xE7,		// RX_PD_EQ_REG_0			 					
	0xFE,		// RX_PD_CDR_REG_0								
	0xFE,		// RX_PD_DRV_REG_0								
	0xF6,		// RX_PD_DRV_REG_1								//20
	0xC0,		// VCO_RANGE_OVERRIDE_CTRL_REG_0				
	0xFC,		// RX_LOS_DET_CTRL_REG_0						
	0xF0,		// LOS_SOFT_ASSERT_CTRL_REG_0					
	0xFC,		// LOS_CHANGE_DETECT_REG_0						
	0xF0,		// LOS_SOFT_ASSERT_CTRL_REG_0					
	0xFC,		// LOL_CHANGE_DETECT_REG_0						
	0xF0,		// LOSL_OUTPUT_CTRL_REG_0						
	0xFE,		// LOSL_OUTPUT_CTRL_REG_1						
	0x00,		// LOSL_STATUS_REG_0					RO						
	0x00,		// LOSL_LATCHED_STATUS_REG_0			RO	
	0xE0,		// CDR_28G_INIT_REG_0
	0xFE,		// TXDSBL_SOFT_REG_0							
	0x00,		// TXFAULT_GLOBAL_STATUS_REG_1			RO		
	0x00,		// TEMP_SENSOR_TRIM_REG_0				RO		
	0x00,		// TEMPERATURE_REG_0					RO		
	0x00,		// TEMPERATURE_REG_1					RO		
	0x00,		// VCCL_SUPPLY_REG_0					RO		
	0x00,		// VCCL_SUPPLY_REG_1					RO		
	0x00,		// TXVCCH_SUPPLY_REG_0					RO		
	0x00,		// TXVCCH_SUPPLY_REG_1					RO		
	0xFE,		// ADC_PARAM_MON_CTRL_RESET_REG_0				
	0xF8,		// ADC_PARAM_MON_CTRL_REG_0		
	0x80,		// CDR_28G_INIT_REG_1
	0x80,		// CDR_28G_INIT_REG_2
	0x80,		// TX_BB_STEP_REG_0							
	0x80,		// RX_BB_STEP_REG_0							
	0xFE,		// PD_ALL_REG_0								
	0xF8,		// PD_SENSE_REG_0	
	0x80,		// TX_EYE_SHAPING_CTRL_REG_0
	0x00,		// VERSION_REG_0						RO		
	0x00,		// TX_LD_MAIN_SWING_SLOPE_REG_0				
	0xFC,		// TX_LD_MAIN_SWING_SLOPE_REG_1				
};








