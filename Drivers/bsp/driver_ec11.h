#ifndef __DRIVER_EC11_H
#define __DRIVER_EC11_H

#include "main.h"

typedef struct{
    int16_t cnt;
    uint8_t button_status;
}TypeDef_EC11;
extern TypeDef_EC11 ec11;

void ec11_init(void);

/*
 ******************** beep ***************************
 *****************************************************
**/
void rotary_beep(void);


#endif /* __DRIVER_EC11_H */









