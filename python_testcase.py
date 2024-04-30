import serial
import unittest
import time
# Define command IDs
CMD_SET_LED_STATE = 0x01
CMD_GET_LED_STATE = 0x02
CMD_GET_SWITCH_STATE = 0x03

# define PARAMS
PARAM_ON    = 0X01
PARAM_OFF   = 0X00
PARAM_PORTA = 0X0A
PARAM_PORTB = 0X0B
PARAM_PIN0  = 0X00
PARAM_PIN1  = 0X01
LED_ONE     = 0X00
NO_PARAM3 = 0xFF

# Serial port configuration
SERIAL_PORT = 'COM5'  # Adjust this to match your serial port
BAUD_RATE = 9600

# Open serial port-
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

def send_command(request_id, cmd, params=None):
    # Send command packet over UART
    cmd_packet = [request_id, cmd]
    if params:
        cmd_packet.extend(params)
    ser.write(bytes(cmd_packet))

# def receive_response():
#     # Receive response packet from microcontroller
#     response = ser.read(1)  # Assuming maximum response length of 32 bytes
#     return response

def set_led_state(state):
    # Send SET_LED_STATE command to microcontroller
    request_id = 0
    CMD_ID = CMD_SET_LED_STATE
    params = [NO_PARAM3,NO_PARAM3,state,LED_ONE]
    send_command(request_id,CMD_ID,params)
    response = receive_response()
    if response:
        return response[0]  # Assuming response is a single byte representing LED state
    else:
        return None

def get_led_state(PARAM_PORT,PARAM_PIN):
    # Send GET_LED_STATE command to microcontroller
    request_id = 0
    CMD_ID = CMD_GET_LED_STATE
    params = [PARAM_PORT,PARAM_PIN,NO_PARAM3,NO_PARAM3]
    send_command(request_id, CMD_ID, params)
    # Receive and parse response
    response = receive_response()
    if response:
        return response[0]  # Assuming response is a single byte representing LED state
    else:
        return None
    
def get_switch_state(PARAM_IDX):
    request_id = 0
    CMD_ID = CMD_GET_SWITCH_STATE
    params = [NO_PARAM3,NO_PARAM3,NO_PARAM3,0]
    send_command(request_id, CMD_ID, params)
    # Receive and parse response
    response = receive_response()
    if response:
        return response[0]  # Assuming response is a single byte representing LED state
    else:
        return None
    
def receive_response(terminator=b'\xFF'):
    # Receive response packet from microcontroller
    response = []
    while True:
        char = ser.read(1)
        response.append(char)
        if char == terminator:
            break
    return response
        

class TestLED(unittest.TestCase):

    import unittest

class TestLED(unittest.TestCase):
    def setUp(self):
        # Run test_Aswitch and store its result
        self.switch_state = get_switch_state(0)
    
    @unittest.skipIf(get_switch_state(0) == b'\x01', "Skipping this test case")
    def test_Bturn_off(self):
             
             set_led_state(PARAM_OFF)
             self.assertEqual(get_led_state(PARAM_PORTA, PARAM_PIN0), b'\x00')

    
    @unittest.skipIf(get_switch_state(0) != b'\x01', "Skipping this test case")
    def test_Cturn_on(self):
        set_led_state(PARAM_ON)
        self.assertEqual(get_led_state(PARAM_PORTA, PARAM_PIN0), b'\x01')

if __name__ == '__main__':
    unittest.main()

    #  def test_Aswitch(self):
    #     self.assertEqual(get_switch_state(0),1)
             
     
    #  def test_Bturn_off(self):
    #     set_led_state(PARAM_OFF)
    #     self.assertEqual(get_led_state(PARAM_PORTA,PARAM_PIN0), 0) 
    #     time.sleep(1) 

   

    #  def test_Cturn_on(self):
    #     set_led_state(PARAM_ON)
    #     self.assertEqual(get_led_state(PARAM_PORTA,PARAM_PIN0), 1) 

      
  

        
# if __name__ == '__main__':
#     unittest.main()

# Example usage
# set_led_state(LED_ON)
# print("LED state:", led_state)

# Close serial port when done
# ser.close()