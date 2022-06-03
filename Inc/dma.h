/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 5/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/

/************************************************************/
/*			GUARD FILE TO PREVENT MULTIPLE INCLUSION		*/
/************************************************************/
#ifndef INC_DMA_H_
#define INC_DMA_H_



#define _DMA2_STREAM0_VECTOR_IRQ		56
#define _DMA2_STREAM1_VECTOR_IRQ		57
#define _DMA2_STREAM2_VECTOR_IRQ		58
#define _DMA2_STREAM3_VECTOR_IRQ		59
#define _DMA2_STREAM4_VECTOR_IRQ		60
#define _DMA2_STREAM5_VECTOR_IRQ		68
#define _DMA2_STREAM6_VECTOR_IRQ		69
#define _DMA2_STREAM7_VECTOR_IRQ		70

#define DMA1		((DMA_dtype*)(DMA1_BASE_ADDRESS))
#define DMA2		((DMA_dtype*)(DMA2_BASE_ADDRESS))

typedef enum{
	PERIPHERAL_TO_MEMORY,
	MEMORY_TO_PERIPHERAL,
	MEMORY_TO_MEMORY,
}transferDirection_dtype;

typedef enum{
	LOW_PRIORITY,
	MED_PRIORITY,
	HIGH_PRIORITY,
	VERY_HIGH_PRIORITY,
}streamPriority_dtype;

typedef enum{
	SINGLE_TRANSFER,
	BEATS_4,
	BEATS_8,
	BEATS_16,
}burstBeats_dtype;

typedef struct{
	burstBeats_dtype memory_beats;
	burstBeats_dtype peripheral_beats;
}burstConfig_dtype;

typedef enum{
	ONE_BYTE,
	TWO_BYTES,
	FOUR_BYTES,
}registerSize_dtype;

typedef enum{
	FIXED,
	INCREMENT,
}incrementMode_dtype;

typedef enum{
	QUARTER,
	HALF,
	THREE_QUARTER,
	FULL,
}fifoThreshold_dtype;

typedef enum{
	DMA_FLOW_CONTROLLER,
	PERIPHERAL_FLOW_CONTROLLER,
}flowCtrl_dtype;

typedef enum{
	DIRECT_MODE_ENABLED,
	DIRECT_MODE_DISABLED,
}directMode_dtype;

typedef struct{
	u32 					src_address;
	u32 					dest_address;
	u16 					number_of_transactions;
	transferDirection_dtype transfer_direction;
	u8						channel_number;
	streamPriority_dtype 	stream_priority;
	burstConfig_dtype 		burst;
	registerSize_dtype 		memory_size;
	registerSize_dtype 		peripheral_size;
	u8 						circular_mode;
	incrementMode_dtype 	memory_ptr_mode;
	incrementMode_dtype 	peripheral_ptr_mode;
	u8 						transfer_interrupt_enable;
	u8 						half_transfer_interrupt_enable;
	u8 						transfer_error_interrupt_enable;
	u8 						direct_mode_error_interrupt_enable;
	u8						fifo_error_interrupt_enable;
	fifoThreshold_dtype		fifo_threshold;
	flowCtrl_dtype			flow_controller;
	directMode_dtype		direct_mode;

}streamCofig_dtype;

typedef enum{
	HALF_COMPLETE 		= 4,
	COMPLETE 			= 5,
	TRANSFER_ERROR 		= 3,
	FIFO_ERROR 			= 0,
	DIRECT_MODE_ERROR 	= 2,
	NO_STATE_AVAILABLE	= 1,
}transferState_dtype;

/************************************************************/
/*						FUNCTION PROTOTYPES					*/
/************************************************************/
void DMA_voidInit(DMA_dtype* DMA_ptr);
u8 DMA_voidConfigureStream(DMA_dtype* DMA_ptr, u8 stream_idx, streamCofig_dtype* streamConfig_ptr);
void DMA_voidStartTransfer(DMA_dtype* DMA_ptr, u8 stream_idx);
transferState_dtype DMA_transferState(DMA_dtype* DMA_ptr, u8 stream_idx);

#endif /* INC_DMA_H_ */
