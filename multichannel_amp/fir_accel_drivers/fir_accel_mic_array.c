/**
 * @file    fir_accel_mic_array.c
 *
 * @brief
 *  
 * @par
 * Created On: 29/10/22
 * Created By: G. Howell
 */

/*------------------- INCLUDES -----------------------------------*/

#include "fir_accel_mic_array.h"
#include "fir_accel_tcb.h"

/*------------------- EXTERN VARIABLES ---------------------------*/
/*------------------- PRIVATE MACROS AND DEFINES -----------------*/
/*------------------- PRIVATE TYPEDEFS ---------------------------*/
/*------------------- STATIC VARIABLES ---------------------------*/
/*------------------- GLOBAL VARIABLES ---------------------------*/

FIRA_INFO firInfo1[FIR_ACC_NUM_CHANNELS];
ADI_FIR_ACC_CONFIG firAccConfig1[FIR_ACC_NUM_CHANNELS];

ADI_FIR_ACC_CONFIG *pfirAccConfig1 = firAccConfig1;

ADI_FIR_ACC_BUFF firAccBuff1[FIR_ACC_NUM_CHANNELS];

// fir input buffers
float firInputBuff1[FIR_ACC_TAP_LENGTH+FIR_ACC_WINDOW_SIZE-1u] = {0};

// fir putput buffers
float firOutputBuff1[FIR_ACC_WINDOW_SIZE] = {0};
float firOutputBuff2[FIR_ACC_WINDOW_SIZE] = {0};

// coefficient buffer
float firCoeffBuff1[FIR_ACC_TAP_LENGTH] =
{
	#include "fir_accel_drivers/firCoeff.dat"
};

/*------------------- STATIC FUNCTION PROTOTYPES -----------------*/
/*------------------- STATIC FUNCTIONS ---------------------------*/
/*------------------- GLOBAL FUNCTIONS ---------------------------*/

/**
 * @brief 	Initalises the fir accelerator
 *
 * @param
 *
 * @return  Fir accel result
 */
void fir_accelerator_initialise(void)
{
	*pREG_SPU0_SECUREP155 = 2u;

	// set up fir 1 buffers
	firAccBuff1[0].pCoeffBuff = (void*)firCoeffBuff1;
	firAccBuff1[0].pInputBuff = (void*)firInputBuff1;
	firAccBuff1[0].pOutputBuff = (void*)firOutputBuff1;

	firAccBuff1[1].pCoeffBuff = (void*)firCoeffBuff1;
	firAccBuff1[1].pInputBuff = (void*)firInputBuff1;
	firAccBuff1[1].pOutputBuff = (void*)firOutputBuff2;

	firInfo1[0].delayLen = FIR_ACC_TAP_LENGTH-1;
	firInfo1[0].windowLen = FIR_ACC_AUDIO_BLOCK_SIZE;
	firInfo1[0].inputBuffLen = FIR_ACC_AUDIO_BLOCK_SIZE+FIR_ACC_TAP_LENGTH-1u;

	firInfo1[1].delayLen = FIR_ACC_TAP_LENGTH-1;
	firInfo1[1].windowLen = FIR_ACC_AUDIO_BLOCK_SIZE;
	firInfo1[1].inputBuffLen = FIR_ACC_AUDIO_BLOCK_SIZE+FIR_ACC_TAP_LENGTH-1u;

	// set up fir accel
	fir_accel_init(firAccBuff1, firAccTcbs, firInfo1, firAccConfig1);
	//	fir_enable_it();
}

void fir_accelerator_run(ADI_FIR_ACC_CONFIG *pFirAccConfig, ADI_FIR_ACC_IN_DATA *pFirInputData)
{
	// run fir accel
	fira_add_input_data(pFirAccConfig, pFirInputData);
	fir_enable(pFirAccConfig);
	fir_wait_till_done();
	fir_accel_reset(pFirAccConfig);
}

