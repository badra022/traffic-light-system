/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 5/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/
#include "MATH_macros.h"
#include "STD_types.h"

#include "rcc.h"
#include "dma.h"

#define RCC_AHB1ENR		*((volatile u32*)(0x40023800 + 0x30))

/************************************************************/
/*						FUNCTION DEFINITIONS				*/
/************************************************************/
void DMA_voidInit(DMA_dtype* DMA_ptr){
	if((u32)DMA_ptr == (u32)DMA1_BASE_ADDRESS){
		SET_BIT(RCC_AHB1ENR, 21);
	}
	else{
		SET_BIT(RCC_AHB1ENR, 22);
	}
}


ErrorStatus DMA_voidConfigureStream(DMA_dtype* DMA_ptr, u8 stream_idx, streamCofig_dtype* streamConfig_ptr){
	if(stream_idx > 7){
		return ERROR;
	}

	if((streamConfig_ptr->transfer_direction == MEMORY_TO_MEMORY) && ((u32)DMA_ptr == (u32)DMA1_BASE_ADDRESS)){
		return ERROR;
	}

	if((streamConfig_ptr->memory_size == TWO_BYTES) && (streamConfig_ptr->peripheral_size == ONE_BYTE)){
		if(streamConfig_ptr->number_of_transactions % 2){
			return ERROR;
		}
	}
	else if((streamConfig_ptr->memory_size == FOUR_BYTES) && (streamConfig_ptr->peripheral_size == ONE_BYTE)){
		if(streamConfig_ptr->number_of_transactions % 4){
			return ERROR;
		}
	}
	else if((streamConfig_ptr->memory_size == FOUR_BYTES) && (streamConfig_ptr->peripheral_size == TWO_BYTES)){
		if(streamConfig_ptr->number_of_transactions % 2){
			return ERROR;
		}
	}

	/* check BURST configs */
	if(((streamConfig_ptr->burst.memory_beats != 0) || (streamConfig_ptr->burst.peripheral_beats != 0))){
		if(((streamConfig_ptr->fifo_threshold + 1) * 4) % 4){
			return ERROR;
		}
	}

	if(((streamConfig_ptr->burst.memory_beats != 0) || (streamConfig_ptr->burst.peripheral_beats != 0))
			&& streamConfig_ptr->circular_mode == TRUE){ /* if BURST enabled with circular mode */
		/*check if DMA_SxNDTR = Multiple of ((Mburst beat) × (Msize)/(Psize)) is satisfied or not */
		u16 estimated_number_of_transcations = streamConfig_ptr->burst.memory_beats * (streamConfig_ptr->memory_size / streamConfig_ptr->peripheral_size);
		if(estimated_number_of_transcations != streamConfig_ptr->number_of_transactions){
			return ERROR;
		}
	}

	if((streamConfig_ptr->circular_mode == TRUE) && (streamConfig_ptr->flow_controller == PERIPHERAL_FLOW_CONTROLLER)){
		return ERROR;
	}

	if((streamConfig_ptr->peripheral_size != streamConfig_ptr->memory_size) && (streamConfig_ptr->direct_mode == DIRECT_MODE_ENABLED)){
		return ERROR;
	}

	/* disable the stream and wait for it to be disabled */
	DMA_ptr->S[stream_idx].CR = 0x00;
	while(GET_BIT(DMA_ptr->S[stream_idx].CR, 0));

	/* set the number of transcations in NDTR */
	DMA_ptr->S[stream_idx].NDTR = streamConfig_ptr->number_of_transactions;

	/* apply incremention configurations */
	DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->peripheral_ptr_mode << 9)
								|(streamConfig_ptr->memory_ptr_mode << 10);

	/* apply memory and peripheral data size */
	DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->memory_size << 13)
								|(streamConfig_ptr->peripheral_size << 11);

	/* apply interrupts configurations */
	SET_VALUE(DMA_ptr->S[stream_idx].FCR, 7, streamConfig_ptr->fifo_error_interrupt_enable);
	DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->direct_mode_error_interrupt_enable << 1)
								|(streamConfig_ptr->half_transfer_interrupt_enable << 3)
								|(streamConfig_ptr->transfer_error_interrupt_enable << 2)
								|(streamConfig_ptr->transfer_interrupt_enable << 4);

	/* select channel and select priority of the stream */
	DMA_ptr->S[stream_idx].CR |= streamConfig_ptr->channel_number << 25;
	DMA_ptr->S[stream_idx].CR |= streamConfig_ptr->stream_priority << 16;

	/* apply burst configurations */
	DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->burst.memory_beats << 23)
								|(streamConfig_ptr->burst.peripheral_beats << 21);

	/* specify the fifo threshold */
	DMA_ptr->S[stream_idx].FCR |= streamConfig_ptr->fifo_threshold;

	/* apply the transfer direction */
	DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->transfer_direction << 6);

	if((streamConfig_ptr->transfer_direction == PERIPHERAL_TO_MEMORY)
		||  (streamConfig_ptr->transfer_direction == MEMORY_TO_PERIPHERAL)){
		/* apply the flow controller config */
		DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->flow_controller << 5);

		/* apply direct mode config */
		DMA_ptr->S[stream_idx].FCR &= ~(0x3 << 2);
		DMA_ptr->S[stream_idx].FCR |= (streamConfig_ptr->direct_mode << 2);

		/* apply circular mode config */
		DMA_ptr->S[stream_idx].CR |= (streamConfig_ptr->circular_mode << 8);

		if(streamConfig_ptr->transfer_direction == PERIPHERAL_TO_MEMORY){
			/* set the addresses src and destination */
			DMA_ptr->S[stream_idx].PAR = streamConfig_ptr->src_address;
			DMA_ptr->S[stream_idx].M0AR = streamConfig_ptr->dest_address;
		}
		else{
			/* set the addresses src and destination */
			DMA_ptr->S[stream_idx].M0AR = streamConfig_ptr->src_address;
			DMA_ptr->S[stream_idx].PAR = streamConfig_ptr->dest_address;
		}

	}
	else if(streamConfig_ptr->transfer_direction == MEMORY_TO_MEMORY){
		/*select DMA flow controller */
		CLR_BIT(DMA_ptr->S[stream_idx].CR, 5);

		/* select indirect mode */
		SET_BIT(DMA_ptr->S[stream_idx].FCR, 2);

		/* disable circular mode */
		CLR_BIT(DMA_ptr->S[stream_idx].CR, 8);

		/* set the addresses src and destination */
		DMA_ptr->S[stream_idx].PAR = streamConfig_ptr->src_address;
		DMA_ptr->S[stream_idx].M0AR = streamConfig_ptr->dest_address;
	}

	return SUCCESS;
}


void DMA_voidStartTransfer(DMA_dtype* DMA_ptr, u8 stream_idx){
	SET_BIT(DMA_ptr->S[stream_idx].CR, 0);
}

transferState_dtype DMA_transferState(DMA_dtype* DMA_ptr, u8 stream_idx){
	if(GET_BIT(DMA_ptr->ISR[stream_idx / 2], (FIFO_ERROR << (6 * (stream_idx % 2)))))
		{
			SET_BIT(DMA_ptr->IFCR[stream_idx / 2], (FIFO_ERROR << (6 * (stream_idx % 2))));
			return FIFO_ERROR;
		}
	if(GET_BIT(DMA_ptr->ISR[stream_idx / 2], (DIRECT_MODE_ERROR << (6 * (stream_idx % 2)))))
		{
			SET_BIT(DMA_ptr->IFCR[stream_idx / 2], (DIRECT_MODE_ERROR << (6 * (stream_idx % 2))));
			return DIRECT_MODE_ERROR;
		}
	if(GET_BIT(DMA_ptr->ISR[stream_idx / 2], (TRANSFER_ERROR << (6 * (stream_idx % 2)))))
		{
			SET_BIT(DMA_ptr->IFCR[stream_idx / 2], (TRANSFER_ERROR << (6 * (stream_idx % 2))));
			return TRANSFER_ERROR;
		}
	if(GET_BIT(DMA_ptr->ISR[stream_idx / 2], (HALF_COMPLETE << (6 * (stream_idx % 2)))))
		{
			SET_BIT(DMA_ptr->IFCR[stream_idx / 2], (HALF_COMPLETE << (6 * (stream_idx % 2))));
			return HALF_COMPLETE;
		}
	if(GET_BIT(DMA_ptr->ISR[stream_idx / 2], (COMPLETE << (6 * (stream_idx % 2)))))
		{
			SET_BIT(DMA_ptr->IFCR[stream_idx / 2], (COMPLETE << (6 * (stream_idx % 2))));
			return COMPLETE;
		}
	return NO_STATE_AVAILABLE;
}
