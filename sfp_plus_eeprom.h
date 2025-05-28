//=============================================================================
//
//		SFP+ EEPROM Memory map definitions
//		2007-Dec-04
//													    								    					    
//=============================================================================
 
// --------------------------------------------------------------
//
// Digital diagnostics monitoring interface : Address 0xA0 / Lower Memory
// 
// -------------------------------------------------------------- 

#define A0_PN								40
#define A0_WAVELENGTH                  		60
#define CC_BASE                       		63
#define RDT_FUNC                       		65
#define CC_EXT                       		95

// --------------------------------------------------------------
//
// Digital diagnostics monitoring interface : Address 0xA2
// 
// --------------------------------------------------------------

//
// #define STATUS_FIELD_AW_THRESHOLDS			0
// 
#define TEMP_HIGH_ALARM						0
#define TEMP_LOW_ALARM						2
#define TEMP_HIGH_WARNING					4
#define TEMP_LOW_WARNING					6
#define VOLT_HIGH_ALARM						8
#define VOLT_LOW_ALARM						10
#define VOLT_HIGH_WARNING					12
#define VOLT_LOW_WARNING					14
#define BIAS_HIGH_ALARM						16
#define BIAS_LOW_ALARM						18
#define BIAS_HIGH_WARNING					20
#define BIAS_LOW_WARNING					22
#define TX_POWER_HIGH_ALARM					24
#define TX_POWER_LOW_ALARM					26
#define TX_POWER_HIGH_WARNING				28
#define TX_POWER_LOW_WARNING			 	30
#define RX_POWER_HIGH_ALARM					32
#define RX_POWER_LOW_ALARM					34
#define RX_POWER_HIGH_WARNING			 	36
#define RX_POWER_LOW_WARNING				38
#define LASER_TEMP_HIGH_ALARM			   	40
#define LASER_TEMP_LOW_ALARM			   	42
#define LASER_TEMP_HIGH_WARNING			   	44
#define LASER_TEMP_LOW_WARNING			   	46
#define TEC_CURRENT_HIGH_ALARM			   	48
#define TEC_CURRENT_LOW_ALARM			   	50
#define TEC_CURRENT_HIGH_WARNING		   	52
#define TEC_CURRENT_LOW_WARNING			   	54

#define UNALLOCATED_ADD						92

#define CHECK_SUM                           95
#define DIAGNOSTICS_TEMP					96
#define DIAGNOSTICS_VCC						98
#define DIAGNOSTICS_TX_BIAS					100
#define DIAGNOSTICS_TX_POWER			 	102
#define DIAGNOSTICS_RX_POWER			 	104
#define DIAGNOSTICS_TEC_TEMP			  	106
#define DIAGNOSTICS_TEC_CURR			  	108

#define STATUS_CONTROL_ADD					110
#define ALARM_FLAGS_ADD						112
#define DIAGNOSTICS_TX_IN_EQ			 	114
#define DIAGNOSTICS_RX_OUT_EMPH			   	115
#define WARNING_FLAGS_ADD					116
#define EXT_STATUS_CONTROL_ADD				118
#define EXT_STATUS_CDR_UNLOCKED_Add		 	119




//A2h DDM area
// #define Ext_Control			118	

#define LoopBack_Add						0x6F	/* 111 */

#define FWVERS_USR							0x78	/* 120 */
#define PRODUCTID_Add						0x79	/* 121 */
#define PROTECTION_ADD						0x7A	/* 122 */
#define USER_PW_ADD							0x7B	/* 123 */

#define TABLE_SEL_Add						0x7F	/* 127 */

// Digital diagnostics monitoring interface : Address 0xA2
//////////////////////////////////////////////////////
////////// USER EEPROM Table 00/01h /////////////////
///////////////////////////////////////////////////// 



//A2h User space
#define ALARMMASK1_ADD						248 - 128			/* 0xF8 - 0x80 */
#define ALARMMASK2_ADD						249 - 128			/* 0xF9 - 0x80 */
#define WARNNMASK1_ADD						250 - 128			/* 0xFA - 0x80 */
#define WARNNMASK2_ADD						251 - 128			/* 0xFB - 0x80 */

#define MODULE_ID_SET						0xFC - 0x80			/* 0xFC - 0x80 */
#define AMCC_LINK_STATUS					0xFD - 0x80			/* 0xFD - 0x80 */
#define AMCC_LINK_STATUS_ADDRESS            0xFD            
#define MODULE_CONTROL_STATUS				0xFE - 0x80			/* 0xFE - 0x80 */
#define MODULE_ID_STATUS					0xFF - 0x80			/* 0xFF - 0x80 */

//SAMSUNG ver
#define SFF_8690_CURRENT_STATUS_VEN			0xF8 - 0x80
#define SFF_8690_LATCHED_STATUS_VEN			0xF9 - 0x80
#define TX_MC_VEN							0xFA - 0x80
#define TX_YC_VEN							0xFB - 0x80
#define RX_MC_VEN							0xFC - 0x80
#define RX_YC_VEN							0xFD - 0x80
#define TX_MAX_CH_VEN						0xFE - 0x80

///////////////////////////////////////////////////////////////////////////////
// Vendor Function memory map : Address 0xA2 Page Common channel 0x10 (128~247) 
///////////////////////////////////////////////////////////////////////////////
#define ADC_TEMP_ADD						0x80 - 0x80
#define ADC_VCC_ADD							0x82 - 0x80
#define ADC_LD_THERMISTOR_RAW_ADD			0x84 - 0x80
#define ADC_TX_PWR_ADD						0x86 - 0x80			/* reflection */
#define ADC_RX_PWR_ADD						0x88 - 0x80
#define ADC_LD_THERMISTOR_ADD				0x8A - 0x80
#define ADC_TEC_VOLTAGE_ADD					0x8C - 0x80
#define ADC_TEC_CURRENT_ADD					0x8E - 0x80
#define ADC_APD_FB_ADD						0x90 - 0x80
#define ADC_TX_WAVE_ADD						0x92 - 0x80			/* trnasmission */
#define ADC_VAPD_MON_ADD					0x94 - 0x80
#define WAVELENGTH_LOCKING_EN          		0x96 - 0x80
#define ADC_ET_MON_ADD						0x97 - 0x80
 
#define TEST_BUF_2                       	0x9C-0x80

#define AMCC_POLARITY                     	0x9A-0x80
#define AMCC_MODULATION_OFFSET         		0x9B-0x80
#define AMCC_DUMMY_DATA                  	0x9D-0x80
#define AMCC_DATALENGTH                   	0x9E-0x80
#define AMCC_DATA                        	0x9F-0x80

#define INT_TEMP_CAL_0_ADD					0xA0 - 0x80 
#define INT_TEMP_CAL_1_ADD					0xA4 - 0x80 
#define INT_TEMP_CAL_2_ADD					0xA8 - 0x80 

#define INT_TX_PWR_CAL_0_ADD				0xAC - 0x80
#define INT_RX_PWR_CAL_0_ADD				0xC0 - 0x80
#define INT_VCC_OFFEST_ADD					0xD6 - 0x80 

#define DAC_APC_SET_Add						0xE0 - 0x80		
#define DAC_AMP_SET_Add						0xE2 - 0x80		
#define DAC_TEC_SET_Add						0xE4 - 0x80		
#define DAC_APD_SET_Add						0xE6 - 0x80		
#define DAC_AGC_SET_Add						0xE8 - 0x80		
#define DAC_AGC_SLOPE     					0xEA - 0x80		
#define DAC_AGC_OFFSET						0xEC - 0x80

#define MOD_OFFSEST_EN_Add					0xEE - 0x80
#define MOD_OFFSEST_Add                     0xEF - 0x80

#define	LDMODE_Add							0xF0 - 0x80
#define	TINDEX_Add							0xF1 - 0x80

#define	SYNC_FLAG_Add						0xF2 - 0x80
#define	DeviceRST_Add						0xF3 - 0x80
#define	CTRLPIN_Add							0xF4 - 0x80


#define	MANI2C_Add							0xF5 - 0x80
#define	MANCTRL_Add							0xF6 - 0x80
#define	MANADDRESS_Add						0xF7 - 0x80
#define	MANREAD_Add							0xF9 - 0x80
#define	MANWRITE_Add						0xFA - 0x80

#define	VENDOR_PW_ADD						0xFB - 0x80
#define	FACTORYDEF_Add						0xFF - 0x80


#ifdef CDR_GN2146
///////////////////////////////////////////////////////////////////////////////
// GN2146 (CDR) memory map : Address 0xA2 Page 0x11 (128~247) 
///////////////////////////////////////////////////////////////////////////////
#define	TX_CDR_BYPASS_CTRL_REG_0_Add					0x80 - 0x80
#define	TX_LOS_THRES_REG_0_Add						    0x81 - 0x80
#define	TX_LOS_HYST_REG_0_Add						    0x82 - 0x80
#define	TX_POLARITY_INVERT_CTRL_REG_0_Add			    0x83 - 0x80
#define	TX_DRV_FORCE_MUTE_REG_0_Add					    0x84 - 0x80
#define	TX_DRV_MUTE_CTRL_REG_0_Add					    0x85 - 0x80
#define	TX_DRV_PD_CTRL_REG_0_Add					    0x86 - 0x80
#define	TX_LD_MAIN_SWING_REG_0_Add					    0x87 - 0x80
#define	TX_LD_MAIN_SWING_REG_1_Add					    0x88 - 0x80
#define	TX_LD_SWING_SEL_REG_0_Add					    0x89 - 0x80
#define	TX_LD_CPA_CTRL_REG_0_Add					    0x8A - 0x80
#define	TX_CTLE_BOOST_OVRD_REG_0_Add				    0x8B - 0x80
#define	TX_PD_LANE_REG_0_Add						    0x8C - 0x80
#define	TX_PD_LOS_REG_0_Add							    0x8D - 0x80
#define	TX_PD_CDR_REG_0_Add							    0x8E - 0x80
#define	TX_PD_DRV_REG_0_Add							    0x8F - 0x80
#define	TX_PD_DRV_REG_1_Add							    0x90 - 0x80
//17                                                
                                                    
#define	RX_CDR_BYPASS_CTRL_REG_0_Add				    0x91 - 0x80
#define	RX_LOS_THRES_REG_0_Add						    0x92 - 0x80
#define	RX_LOS_HYST_REG_0_Add						    0x93 - 0x80
#define	RX_OFFSET_CORRECTION_CTRL_REG_0_Add			    0x94 - 0x80
#define	RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0_Add    0x95 - 0x80
#define	RX_POLARITY_INVERT_CTRL_REG_0_Add			    0x96 - 0x80
#define	RX_EQ_BOOST_REG_0_Add						    0x97 - 0x80
#define	RX_DRV_FORCE_MUTE_REG_0_Add				    	0x98 - 0x80
#define	RX_DRV_MUTE_CTRL_REG_0_Add					    0x99 - 0x80
#define	RX_DRV_PD_CTRL_REG_0_Add					    0x9A - 0x80
#define	RX_DRV_MAIN_SWING_REG_0_Add					    0x9B - 0x80
#define	RX_DRV_INV_CTRL_REG_0_Add					    0x9C - 0x80
#define	RX_DRV_DEEMPH_CTRL_REG_0_Add				    0x9D - 0x80
#define	RX_DRV_PREDRV_CM_REF_REG_0_Add				    0x9E - 0x80
#define	RX_PD_LANE_REG_0_Add						    0x9F - 0x80
#define	RX_PD_LOS_REG_0_Add							    0xA0 - 0x80
#define	RX_PD_EQ_REG_0_Add			 				    0xA1 - 0x80
#define	RX_PD_CDR_REG_0_Add							    0xA2 - 0x80
#define	RX_PD_DRV_REG_0_Add							    0xA3 - 0x80
#define	RX_PD_DRV_REG_1_Add								0xA4 - 0x80
//20

#define	VCO_RANGE_OVERRIDE_CTRL_REG_0_Add				0xA5 - 0x80
#define	RX_LOS_DET_CTRL_REG_0_Add						0xA6 - 0x80
#define	LOS_SOFT_ASSERT_CTRL_REG_0_Add					0xA7 - 0x80
#define	LOS_CHANGE_DETECT_REG_0_Add						0xA8 - 0x80
#define	LOS_SOFT_ASSERT_CTRL_REG_1_Add					0xA9 - 0x80
#define	LOL_CHANGE_DETECT_REG_1_Add						0xAA - 0x80
#define	LOSL_OUTPUT_CTRL_REG_0_Add						0xAB - 0x80
#define	LOSL_OUTPUT_CTRL_REG_1_Add						0xAC - 0x80
#define	LOSL_STATUS_REG_0_Add							0xAD - 0x80
#define	LOSL_LATCHED_STATUS_REG_0_Add					0xAE - 0x80
#define	CDR_28G_INIT_REG_0_Add							0xAF - 0x80
#define	TXDSBL_SOFT_REG_0_Add							0xB0 - 0x80
#define	TXFAULT_GLOBAL_STATUS_REG_1_Add					0xB1 - 0x80
#define	TEMP_SENSOR_TRIM_REG_0_Add						0xB2 - 0x80
#define	TEMPERATURE_REG_0_Add							0xB3 - 0x80
#define	TEMPERATURE_REG_1_Add							0xB4 - 0x80
#define	VCCL_SUPPLY_REG_0_Add							0xB5 - 0x80
#define	VCCL_SUPPLY_REG_1_Add							0xB6 - 0x80
#define	TXVCCH_SUPPLY_REG_0_Add							0xB7 - 0x80
#define	TXVCCH_SUPPLY_REG_1_Add							0xB8 - 0x80
#define	ADC_PARAM_MON_CTRL_RESET_REG_0_Add				0xB9 - 0x80
#define	ADC_PARAM_MON_CTRL_REG_0_Add					0xBA - 0x80
#define	CDR_28G_INIT_REG_1_Add							0xBB - 0x80
#define	CDR_28G_INIT_REG_2_Add							0xBC - 0x80
#define	TX_BB_STEP_REG_0_Add							0xBD - 0x80
#define	RX_BB_STEP_REG_0_Add							0xBE - 0x80
#define	PD_ALL_REG_0_Add								0xBF - 0x80
#define	PD_SENSE_REG_0_Add								0xC0 - 0x80
#define	TX_EYE_SHAPING_CTRL_REG_0_Add					0xC1 - 0x80
#define	VERSION_REG_0_Add								0xC2 - 0x80
#define	TX_LD_MAIN_SWING_SLOPE_REG_0_Add				0xC3 - 0x80
#define	TX_LD_MAIN_SWING_SLOPE_REG_1_Add				0xC4 - 0x80
	
#define	GN2146FAC_Add									0xC5 - 0x80


#define	APD1_FeedbackErr_Add				       		0xFA - 0x80
#define	APD1_ClampStatus_Add				          	0xFC - 0x80
#define	APD1_DutyCycle_Add				                0xFE - 0x80

#endif

///////////////////////////////////////////////////////////////////////////////
// HXC42200(CDR) memory map : Address 0xA2 Page 0x11 (128~247) 
///////////////////////////////////////////////////////////////////////////////
 
#define	HXC42200FAC_Add						    0xFF - 0x80

///////////////////////////////////////////////////////////////////////////////
// LUT memory map : Address 0xA2 Page 0x14, TEC LUT (128~247) /*NOT USE*/
///////////////////////////////////////////////////////////////////////////////
#define	APCConfiguration_Add					0xEC - 0x80
#define	APCTarget_Add							0xEE - 0x80
#define	APCMAXBIas_Add							0xF0 - 0x80
#define	APCInitBias_Add							0xF2 - 0x80
#define	APCIStep_Add							0xF4 - 0x80
#define	APCBiasReg_Add							0xF6 - 0x80
#define	APC_Option								0xF8 - 0x80
#define	APCBiasRegOpen_Add						0xFA - 0x80
#define	MPD_QT_LowThres_Add						0xFC - 0x80
#define	MPD_QT_HighThres_Add					0xFE - 0x80



///////////////////////////////////////////////////////////////////////////////
// LUT memory map : Address 0xA2 Page 0x13 MODLUTAB (128~247) 
///////////////////////////////////////////////////////////////////////////////
//#define	LUTFirstIndex_Add						0x00 - 0x80
#define MOD_LUT_CONST_0                         0x80 - 0x80
#define MOD_LUT_CONST_1                         0x84 - 0x80
#define MOD_LUT_CONST_2                         0x88 - 0x80

#define SOA_LUT_CONST_0                         0x90 - 0x80
#define SOA_LUT_CONST_1                         0x94 - 0x80
#define SOA_LUT_CONST_2                         0x98 - 0x80

#define	BEFORE_SOA_OFFSET_Add					0x9E - 0x80
#define	LUTHighIndex_Add						0xC8 - 0x80


///////////////////////////////////////////////////////////////////////////////
// LUT memory map : Address 0xA2 Page 0x14, TEC LUT (128~247) 
///////////////////////////////////////////////////////////////////////////////
#define TEC_LUT_CONST_0                         0x80 - 0x80
#define TEC_LUT_CONST_1                         0x84 - 0x80
#define TEC_LUT_CONST_2                         0x88 - 0x80
#define	TEC_LUT_Add					    		0x8C - 0x80
#define	TECCN_Add					    		0xEC - 0x80
#define	TECTUDY_Add					    		0xEE - 0x80
#define	TECKP_Add					    		0xF0 - 0x80
#define	TECKI_Add					    		0xF2 - 0x80
#define	TECKD_Add					    		0xF4 - 0x80
#define	TECSHIFT_Add					    	0xF6 - 0x80
#define	TECPosERRClamp_Add						0xF8 - 0x80
#define	TECNegERRClamp_Add						0xFA - 0x80
#define	TECPositiveClamp_Add					0xFC - 0x80
#define	TECNegativeClamp_Add					0xFE - 0x80
// #define	TECIConfig_Add							0xF0 - 0x80

///////////////////////////////////////////////////////////////////////////////
// LUT memory map : Address 0xA2 Page 0x15, APD LUT (128~247) 
///////////////////////////////////////////////////////////////////////////////

#define	APD1_ErrorClamp_Add				    	0xEC - 0x80
#define	APD1_DutyClamp_Add				    	0xEE - 0x80
#define	APD1_InternalGain_Add			    	0xF0 - 0x80
#define	APD1_DutyStartClamp_Add			    	0xF2 - 0x80
#define	APD1_IntergratorClamp_Add		    	0xF4 - 0x80
#define	APD1_NegativeErrorClamp_Add			 	0xF6 - 0x80
#define	APD1_PositiveErrorClamp_Add				0xF8 - 0x80
#define	APD1_ProportionalGain_Add				0xFA - 0x80
#define	APD1_CN_Add				                0xFC - 0x80                                               

// Digital diagnostics monitoring interface : Address 0xA2
//////////////////////////////////////////////////////
////////// 8690 LIMIT EEPROM Table 40/h /////////////////
///////////////////////////////////////////////////// 

#define LIMIT_FISRT_FREQUENCY_Add				0x80 - 0x80
#define LIMIT_LAST_FREQUENCY_Add				0x82 - 0x80
#define LIMIT_GRID_SPACING_Add					0x84 - 0x80
#define PHASE_OFFSET_VALUE1_Add					0x90 - 0x80
#define PHASE_OFFSET_VALUE2_Add					0x91 - 0x80
#define PHASE_OFFSET_VALUE3_Add					0x92 - 0x80
#define REAR_OFFSET_VALUE1_Add					0x93 - 0x80
#define REAR_OFFSET_VALUE2_Add					0x94 - 0x80
#define REAR_OFFSET_VALUE3_Add					0x95 - 0x80
#define REAR_PHASE_mV_HYS1_Add					0x96 - 0x80 //2byte 
#define REAR_PHASE_mV_HYS2_Add					0x98 - 0x80 //2byte 
#define WAVELENGTH_HYS_Add						0x9A - 0x80
#define SOA_LUT_RESULT_Add						0x9B - 0x80 //2byte



///////////////////////////////////////////////////////////////////////////////
// DEBUG memory map : Address 0xA2 Page 0x70, Debug PAGE (128~255) 
///////////////////////////////////////////////////////////////////////////////

#define	InrushDone_Add				    		0x80 - 0x80
#define STATE_ADD				              	0x81 - 0x80

#define BUCK_delay_time							0x82 - 0x80		//2byte
/*inrush test*/
#define CALCULATE_STEP_Add						0x84 - 0x80
#define REACH_STEP_Add							0x85 - 0x80

/*low temp test*/
#define GN2146_RESET_DELAY_Add					0x87 - 0x80

/* 0x01 = TEC_MASK, 0x02 = LOL_MASK, 0x04 = LOS_MASK*/                          
#define FAULT_MASK                              0x8F - 0x80

#define TTOSA_SERIAL_NUMBER_ADD				    0x90 - 0x80			/* 0x90 ~ 0x9F, 16 bytes */

/*Wavelength reporting*/
#define WAVE_PN_REPORTING_ENABLE                0XAE - 0x80
#define WAVE_REPORTING_ENABLE                   0xAF - 0x80

#define RX_LOS_OUT_EN							0xB0 - 0x80		//0 : Dis, 1 : En
#define RSSI_TH_H								0xB1 - 0x80		//2byte
#define RSSI_TH_L								0xB3 - 0x80		//2byte

#define LUTLTemp_Add						    0xBA - 0x80
#define LUTRTemp_Add						    0xBC - 0x80
#define LUTHTemp_Add						    0xBE - 0x80



/*shutdown&recovery*/
#define SHUTDOWN_TEMP                           0xD4 - 0x80
#define RECOVERY_TEMP                           0xD6 - 0x80
#define SHUTDOWN_FLAG                           0xD8 - 0x80
#define SHUTDOWN_ENABLE                         0xD9 - 0x80
#define CRCINDEX                                0xDF - 0x80

#define TEC_DELTA_TEMP_Add			          	0xE0 - 0x80
#define	SOFT_RATE_SEL_DEF0_Add			  		0xE1 - 0x80
#define	SOFT_RATE_SEL_DEF1_Add			  		0xE2 - 0x80
#define	TX_EQ_DEF_Add			  	        	0xE3 - 0x80
#define	RX_EM_DEF_Add			  	        	0xE4 - 0x80
#define TX_EYE_SHAPING_10G_Add					0xE5 - 0x80
#define TX_EYE_SHAPING_25G_Add					0xE6 - 0x80
#define RX_EQ_BOOST_10G_Add						0xE7 - 0x80
#define RX_EQ_BOOST_25G_Add						0xE8 - 0x80
#define RDT_BUFF_Add							0xE9 - 0x80
#define UART_ROD_PERIOD_Add						0xEA - 0x80

#define DURATION_CONTROL_BIT                    0xEE - 0x80
#define EEPROM_RESET                            0xEF - 0x80

#define SOFTWARE_RESET							0xF6 - 0x80

#define PAGE_OFFSET                             0xFA - 0x80 //4byte

#define TX_IN_EQ_DEFAULT			0x50
#define RX_OUT_EMPH_DEFAULT			0x00

#define RS0_DEFAULT					0
#define RS1_DEFAULT					0

#define TX_EYE_SHAPING_10G_DEFAULT	31
#define TX_EYE_SHAPING_25G_DEFAULT	0

#define RX_EQ_BOOST_10G_DEFAULT		10
#define RX_EQ_BOOST_25G_DEFAULT		50


