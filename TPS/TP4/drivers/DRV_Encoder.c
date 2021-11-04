/***************************************************************************//**
  @file     DRV_Encoder.c
  @brief    Rotary encoder driver
  @author   Grupo 1
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "DRV_Board.h"
#include "DRV_Encoder.h"
#include "DRV_Timers.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//Definiciones de pines que irian en config
#define PIN_A 		PORTNUM2PIN(PA,1)
#define PIN_B 		PORTNUM2PIN(PB,23)
#define PIN_SWITCH 	PORTNUM2PIN(PA,2)
#define SW_ACTIVE 	LOW

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

//Estados de la FSM
enum states {START, CW1, CW2, CW3, ACW1, ACW2, ACW3};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static encoderResult_t check_event(bool A, bool B);
static void get_current_pin_values(void);
static void encoder_callback(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

//Valores actuales de A, B y D que se actualizan con get_current_pin_values()
static bool A_current_value;
static bool B_current_value;
static bool switch_current_value;

//Data de encoder
static bool status;
static encoderResult_t encoder_event;

//Def del timer
static tim_id_t encoder_timer;

//Semáforo del encoder
static OS_SEM semEncoder;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initEncoder() {
    //Inicializo Timer
	initTimers();
	encoder_timer = timerGetId();

    //Inicializo Data
    encoder_event = ENC_NONE;
    status = false;

    //Seteo los Pines
	gpioMode(PIN_A, INPUT);
	gpioMode(PIN_B, INPUT);
	gpioMode(PIN_SWITCH, INPUT);

    //Seteo el timer para que llame periodicamente a encoder_callback con 1ms
	timerStart(encoder_timer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, encoder_callback);

	//Create semaphore
    OS_ERR os_err;
	OSSemCreate(&semEncoder, "Sem Encoder", 0u, &os_err);
}

bool encoderGetStatus(){
	if(status){
		status = false;
		return true;
	}else{
		return false;
	}
}

encoderResult_t encoderGetEvent(){
	return encoder_event;
}

OS_SEM* encoderSemPointer(){
	return &semEncoder;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

//FSM que determina si se realizo un movimiento en sentido horario (CW) o antihorario (ACW)
static encoderResult_t check_event(bool A, bool B){

    static enum states current_state = START;
    static bool last_sw = false;

    OS_ERR os_err;

    encoderResult_t result = ENC_NONE;

    // Reviso si hubo un flanco del switch
    bool current_sw = switch_current_value == SW_ACTIVE;
    if(!last_sw && current_sw){
    	result = ENC_CLICK;
    	status = true;

    	OSSemPost(&semEncoder, OS_OPT_POST_1, &os_err);
    }
    last_sw = current_sw;

    // Analizo el estado de la FSM del encoder
    switch(current_state){
        case START:
                if (A && !B){  
                    current_state = CW1;
                }
                if (!A && B){
                    current_state = ACW1;
                }
        break;
        case CW1:
                if(!A && !B){
                    current_state = CW2;
                }
                else if(A && !B){
                }
                else{
                    current_state = START;
                }
        break;
        case CW2:
                if(!A && B){
                	current_state = CW3;
                }
                else if(!A && !B){
                }
                else{
                    current_state = START;
                }
        break;
        case CW3:
                if(A && B){
                    current_state = START;
                    result = ENC_RIGHT;
                    status = true;

                    OSSemPost(&semEncoder, OS_OPT_POST_1, &os_err);
                } 
                else if(!A && B){
                      
                }
                else{
                    current_state = START;
                }
        break;
        case ACW1:
                if(!A && !B){
                    current_state = ACW2;
                }
                else if(!A && B){
                }
                else{
                    current_state = START;
                }
        break;
        case ACW2:
                if(A && !B){
                    current_state = ACW3;
                }
                else if(!A && !B){
                    
                }
                else{
                    current_state = START;
                }
        break;
        case ACW3:
                if(A && B){
                    current_state = START;
                    result = ENC_LEFT;
                    status = true;

                    OSSemPost(&semEncoder, OS_OPT_POST_1, &os_err);
                } 
                else if(A && !B){
                      
                }
                else{
                    current_state = START;
                }
        break;
        default: break;
    }
    return result;
}

static void get_current_pin_values(void){

	A_current_value = gpioRead(PIN_A);
	B_current_value = gpioRead(PIN_B);
    switch_current_value = gpioRead(PIN_SWITCH);

}

static void encoder_callback(void){

    get_current_pin_values(); // Busco valores actuales de A, B y switch

    encoder_event = check_event(A_current_value, B_current_value);   // FSM analiza si ocurre un evento
}
