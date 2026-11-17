#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool lamp_on;
    bool plug_on;
    bool door_open;
    float current_temp;
    int32_t temp_limit;
    bool alarm_active;
} HomeState_t;

void Protocol_Init(bool initial_door_open);
void Protocol_ProcessIncoming(void);

// Event hooks called by main loop / sensor updates
void Protocol_UpdateTemperature(float temp_c);
void Protocol_UpdateDoor(bool door_open);
void Protocol_SendFullStatus(void);

const HomeState_t* Protocol_GetState(void);

#endif // PROTOCOL_H