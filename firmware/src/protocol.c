#include "protocol.h"
#include "uart_bt.h"
#include "relay.h"
#include <string.h>
#include <stdlib.h>

static HomeState_t state;

// Helper: formats integer to ASCII string
static void int_to_str(int32_t val, char *buf) {
    char tmp[12];
    int i = 0;
    bool neg = false;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    if (val < 0) {
        neg = true;
        val = -val;
    }
    while (val > 0) {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    int pos = 0;
    if (neg) {
        buf[pos++] = '-';
    }
    while (i > 0) {
        buf[pos++] = tmp[--i];
    }
    buf[pos] = '\0';
}

// Helper: formats "TEMP:XX.X" without needing printf float support
static void send_temp_line(float temp_c) {
    char msg[24] = "TEMP:";
    char num[12];

    if (temp_c < 0.0f) {
        temp_c = 0.0f;
    }

    int32_t tenths = (int32_t)(temp_c * 10.0f + 0.5f);
    int32_t whole = tenths / 10;
    int32_t frac = tenths % 10;

    int_to_str(whole, num);
    strcat(msg, num);
    strcat(msg, ".");

    char frac_str[2] = { (char)('0' + frac), '\0' };
    strcat(msg, frac_str);

    UART_BT_SendLine(msg);
}

static void evaluate_alarm(void) {
    bool should_alarm = (state.current_temp > (float)state.temp_limit);
    if (should_alarm != state.alarm_active) {
        state.alarm_active = should_alarm;
        UART_BT_SendLine(state.alarm_active ? "ALARM:1" : "ALARM:0");
    }
}

void Protocol_Init(bool initial_door_open) {
    state.lamp_on = false;
    state.plug_on = false;
    state.door_open = initial_door_open;
    state.current_temp = 25.0f;
    state.temp_limit = 30;
    state.alarm_active = false;

    Relay_SetLamp(false);
    Relay_SetPlug(false);
}

void Protocol_SendFullStatus(void) {
    UART_BT_SendLine(state.lamp_on ? "LAMP:1" : "LAMP:0");
    UART_BT_SendLine(state.plug_on ? "PLUG:1" : "PLUG:0");
    UART_BT_SendLine(state.door_open ? "DOOR:OPEN" : "DOOR:CLOSED");
    send_temp_line(state.current_temp);
    UART_BT_SendLine(state.alarm_active ? "ALARM:1" : "ALARM:0");
}

void Protocol_ProcessIncoming(void) {
    UART_BT_Poll();
    if (!UART_BT_HasLine()) {
        return;
    }

    char line[BT_RX_BUFFER_SIZE];
    UART_BT_GetLine(line, sizeof(line));

    if (strcmp(line, "LAMP:1") == 0) {
        state.lamp_on = true;
        Relay_SetLamp(true);
        UART_BT_SendLine("ACK:LAMP:1");
    }
    else if (strcmp(line, "LAMP:0") == 0) {
        state.lamp_on = false;
        Relay_SetLamp(false);
        UART_BT_SendLine("ACK:LAMP:0");
    }
    else if (strcmp(line, "PLUG:1") == 0) {
        state.plug_on = true;
        Relay_SetPlug(true);
        UART_BT_SendLine("ACK:PLUG:1");
    }
    else if (strcmp(line, "PLUG:0") == 0) {
        state.plug_on = false;
        Relay_SetPlug(false);
        UART_BT_SendLine("ACK:PLUG:0");
    }
    else if (strncmp(line, "LIMIT:", 6) == 0) {
        int32_t new_limit = (int32_t)strtol(&line[6], NULL, 10);
        state.temp_limit = new_limit;

        char ack[24] = "ACK:LIMIT:";
        char num[12];
        int_to_str(new_limit, num);
        strcat(ack, num);
        UART_BT_SendLine(ack);

        // Immediately check if new limit triggers or clears the alarm
        evaluate_alarm();
    }
    else if (strcmp(line, "STATUS?") == 0) {
        Protocol_SendFullStatus();
    }
}

void Protocol_UpdateTemperature(float temp_c) {
    state.current_temp = temp_c;
    send_temp_line(temp_c);
    evaluate_alarm();
}

void Protocol_UpdateDoor(bool door_open) {
    if (door_open != state.door_open) {
        state.door_open = door_open;
        UART_BT_SendLine(state.door_open ? "DOOR:OPEN" : "DOOR:CLOSED");
    }
}

const HomeState_t* Protocol_GetState(void) {
    return &state;
}