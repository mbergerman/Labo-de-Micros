#include "DRV_Board.h"
#include "DRV_Encoder.h"
#include "DRV_Timers.h"

//Definiciones de pines que irian en config
#define PIN_A PORTNUM2PIN(PA,1)
#define PIN_B PORTNUM2PIN(PB,23)
#define PIN_SWA PORTNUM2PIN(PA,2)
#define SW_ACTIVE LOW

//Valores actuales de A, B y C que se actualizan con getCurr()
static bool A_curr;
static bool B_curr;
static bool C_curr;

//Data de encoder
static bool status;
static enum encoderResults data;

//Def del timer
static tim_id_t encoder_timer;

//Estados de la FSM
enum states {START, CW1, CW2, CW3, ACW1, ACW2, ACW3};



//FSM que determina si se realizo un movimiento en sentido horario (CW) o antihorario (ACW)
static enum encoderResults CheckMovement(bool A, bool B){

    static enum states CurrentState = START;
    enum encoderResults result = NONE;
    //status = false;

    switch(CurrentState){
        case START:
                if (A && !B){  
                    CurrentState = CW1;
                }
                if (!A && B){
                    CurrentState = ACW1;
                }
        break;
        case CW1:
                if(!A && !B){
                    CurrentState = CW2;
                }
                else if(A && !B){
                }
                else{
                    CurrentState = START;
                }
        break;
        case CW2:
                if(!A && B){
                    CurrentState = CW3;
                }
                else if(!A && !B){
                }
                else{
                    CurrentState = START;
                }
        break;
        case CW3:
                if(A && B){
                    CurrentState = START;
                    result = RIGHT;
                    status = true;
                } 
                else if(!A && B){
                      
                }
                else{
                    CurrentState = START;
                }
        break;
        case ACW1:
                if(!A && !B){
                    CurrentState = ACW2;
                }
                else if(!A && B){
                }
                else{
                    CurrentState = START;
                }
        break;
        case ACW2:
                if(A && !B){
                    CurrentState = ACW3;
                }
                else if(!A && !B){
                    
                }
                else{
                    CurrentState = START;
                }
        break;
        case ACW3:
                if(A && B){
                    CurrentState = START;
                    result = LEFT;
                    status = true;
                } 
                else if(A && !B){
                      
                }
                else{
                    CurrentState = START;
                }
        break;
        default:
        break;
    }
    return result;
}


static void getCurr(void){

	A_curr = gpioRead(PIN_A);
	B_curr = gpioRead(PIN_B);
    C_curr = gpioRead(PIN_SWA);

}

static void callbackEncoder(void) {   

    getCurr(); // Busco valores actuales de A, B y C

    enum encoderResults newCheck = CheckMovement(A_curr, B_curr);   //FSM analiza los valores actuales

    // Si se precionó el boton, el resultado de data es CLICK. Notar que si se aprieta el botón, se ignora el resultado de la FSM
    if(C_curr == SW_ACTIVE)
    {
        data = CLICK;
    }
    else
    {
        data = newCheck;
    }

}

bool getStatus(){
	if(status){
		status = false;
		return true;
	}else{
		return false;
	}
}

 enum encoderResults getData(){

    return data;
 }

void initEncoder() {
    //Inicializo Timer
	initTimers();
	encoder_timer = timerGetId();

    //Inicializo Data 
    data = NONE;
    status = false;

    //Seteo los Pines
	gpioMode(PIN_A, INPUT);
	gpioMode(PIN_B, INPUT);
	gpioMode(PIN_SWA, INPUT);

    //Seteo el timer para que llame periodicamente a callbackEncoder con 10ms
	timerStart(encoder_timer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, callbackEncoder);
}
