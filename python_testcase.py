import serial
import unittest

# Define command IDs
CMD_SET_PIN_STATE = 0x01
CMD_GET_PIN_STATE = 0x02


# define PARAMS
PARAM_ON    = 0X01
PARAM_OFF   = 0X00
PARAM_PORTA = 0X0A
PARAM_PORTB = 0X0B
PARAM_PIN0  = 0X00
PARAM_PIN1  = 0X01
PARAM_PIN8  = 0X08


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

def set_pin_state(port,pin,state):
    # Send SET_LED_STATE command to microcontroller
    request_id = 0
    CMD_ID = CMD_SET_PIN_STATE
    params = [port,pin,state]
    send_command(request_id,CMD_ID,params)
    response = receive_response()
    if response:
        return response[0]  # Assuming response is a single byte representing LED state
    else:
        return None

def get_pin_state(PARAM_PORT,PARAM_PIN):
    # Send GET_LED_STATE command to microcontroller
    request_id = 0
    CMD_ID = CMD_GET_PIN_STATE
    params = [PARAM_PORT,PARAM_PIN,NO_PARAM3]
    send_command(request_id, CMD_ID, params)
    # Receive and parse response
    response = receive_response()
    if response:
        return response[0]  # Assuming response is a single byte representing LED state
    else:
        return None
    
    
def receive_response(terminator=b'\x50'):
    # Receive response packet from microcontroller
    response = []
    Time=0
    while True:
        # if (Time<10000):
        #  Time += 1
         char = ser.read(1)
         response.append(char)
         if char == terminator:
           break
    return response
        



class TestLED(unittest.TestCase):
    # def setUp(self):
        # Run test_Aswitch and store its result


        
    #     self.switch_state = get_pin_state(PARAM_PORTA,PARAM_PIN8)
    
    # @unittest.skipIf(get_pin_state(PARAM_PORTA,PARAM_PIN1) == b'\x01', "Skipping this test case")
    def test_pressed(self):

        set_pin_state(PARAM_PORTA,PARAM_PIN0,PARAM_ON)
        self.assertEqual(get_pin_state(PARAM_PORTA, PARAM_PIN8), b'\x01')

    
    # @unittest.skipIf(get_pin_state(0) != b'\x01', "Skipping this test case")
    def test_released(self):
        set_pin_state(PARAM_PORTA,PARAM_PIN0,PARAM_OFF)
        self.assertEqual(get_pin_state(PARAM_PORTA, PARAM_PIN8), b'\x00')

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