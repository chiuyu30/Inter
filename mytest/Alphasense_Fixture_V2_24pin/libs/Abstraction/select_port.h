#ifndef SELECT_PORT
#define SELECT_PORT

#define PORT_COUNT               24

void Reset_switch(void);
UINT8 switch_port(UINT8 port);
UINT8 switch_port_sht(UINT8 port);
UINT8 control_multiplexer(UINT8 address, UINT8 command);
void check_port_gas_address(void);
extern UINT8 port_address[PORT_COUNT];

#endif