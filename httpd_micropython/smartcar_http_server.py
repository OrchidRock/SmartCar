
# SmartCar Monitor
#
# 2022-5-12
# Rock.
#

# packages
import machine
import network
import time
import socket

L298N_IN1_GPIO = 16
L298N_IN2_GPIO = 5
L298N_IN3_GPIO = 4
L298N_IN4_GPIO = 0
L298N_ENA_GPIO = 14
L298N_ENB_GPIO = 12

LM393_OUT_GPIO = 2

UART2_RX_GPIO = 13
UART2_TX_GPIO = 15


css = """
<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style> html{font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}.button {background-color: #195B6A;border: none;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;position:absolute;width:100px;height:100px;}</style></head>"""

html = """<!DOCTYPE html>
<html>""" + css + """
<h1><title>ESP8266 SmartCar Monitor</title></h1><body><a href=\"?bm=0\"><button class=\"button\" style=\"top:10px;left:300px;background-color:red;\">S</button></a>&nbsp;<a href=\"?bm=1\"><button class=\"button\" style=\"top:250px;left:300px;background-color:green;\">Go</button></a>&nbsp;<a href=\"?bm=2\"><button class=\"button\" style=\"top:250px;left:440px;background-color:yellow\">B</button></a>&nbsp;<a href=\"?bm=3\"><button class=\"button\" style=\"top:50px;left:640px;\">U</button></a>&nbsp;<a href=\"?bm=4\"><button class=\"button\" style=\"top:200px;left:640px;\">D</button></a><a href=\"?fm=0\"><button class=\"button\" style=\"top:130px;left:150px;\">L</button></a>&nbsp;<a href=\"?fm=1\"><button  class=\"button\" style=\"top:130px;left:440px;\">R</button></a>&nbsp;<a href=\"?fm=2\"><button class=\"button\" style=\"top:130px;left:300px;\">O</button></a>&nbsp;</body></html>
"""


smartcar_pins = []
pwm_pins = []




def back_motor_stop():
    smartcar_pins[0].on()
    smartcar_pins[1].on()


def back_motor_run():
    smartcar_pins[0].on()
    smartcar_pins[1].off()


def back_motor_reverse():
    smartcar_pins[0].off()
    smartcar_pins[1].on()


def back_motor_speedup():
    duty = pwm_pins[0].duty()
    if (duty + 200) > 1024:
        pwm_pins[0].duty(1024)
    else:
        pwm_pins[0].duty(duty + 200)


def back_motor_speeddown():
    duty = pwm_pins[0].duty()
    if (duty - 200) < 400:
        pwm_pins[0].duty(400)
    else:
        pwm_pins[0].duty(duty - 200)


def front_motor_left():
    smartcar_pins[2].off()
    smartcar_pins[3].on()


def front_motor_right():
    smartcar_pins[2].on()
    smartcar_pins[3].off()


def front_motor_return():
    smartcar_pins[2].off()
    smartcar_pins[3].off()


def wifi_config(SSID, Password):
    """
    Wifi Configurtion
    """
    sta_if = network.WLAN(network.STA_IF)
    if sta_if.active():
        print("Wifi Interface In Active...")
    else:
        print("Config Wifi Failed -- Interface inactive")
    return

    sta_if.active(True)
    sta_if.connect(SSID, Password)
    while not sta_if.isconnected():
        time.sleep(1)
    print(sta_if.ifconfig())


def http_server_init(port):
    addr = socket.getaddrinfo('0.0.0.0', port)[0][-1]

    socket_handler = socket.socket()
    socket_handler.bind(addr)
    socket_handler.listen(10)

    print("Http Server Listening on", addr)

    return socket_handler


def http_get_table_handler(get_url):

    print(get_url)

    if (get_url.find('?bm=')):
        index = get_url.find('?bm=')
        command = get_url[index + 4:index + 5]
        # machine_motor_control(command, 0)
        if command == '0':
            back_motor_stop()
        elif command == '1':
            back_motor_run()
        elif command == '2':
            back_motor_reverse()
        elif command == '3':
            back_motor_speedup()
        elif command == '4':
            back_motor_speeddown()
        else:
            pass
    else:
        pass

    if (get_url.find('?fm=')):
        #print("front motor")
        index = get_url.find('?fm=')
        command = get_url[index + 4:index + 5]
        # machine_motor_control(command, 2)
        if command == '0':  # turn left
            #print("front Left")
            front_motor_left()
        elif command == '1':  # turn right
            front_motor_right()
        elif command == '2':  # auto return
            front_motor_return()
        else:
            pass
    else:
        print("Invaild http get request")
        return


def http_server_accept_loop(socket_handler):

    while True:
        conn, addr = socket_handler.accept()
        print('Client connected from', addr)
        conn_file = conn.makefile('rwb', 0)

        while True:
            line = conn_file.readline()
            if line[0:6] == b'GET /?':
                http_get_table_handler(str(line))
            else:
                pass
            if not line or line == b'\r\n':
                break

        response = html
    # print(response)
        conn.send(
            b'HTTP/1.0 200 OK\r\nContent-type: text/html\r\nConnection: close\r\n\r\n')
        print("HTML Size: ", len(html))
        s_len = conn.sendall(response)
        print("Sended HTML Size: ", s_len)
        # time.sleep(1)
        conn.close()

def motor_port_init():
    # machine_gpio_init()
    """
    GPIO16-D0: IN1      GPIO5-D1:  IN2
    GPIO4-D2 : IN3      GPIO0-D3 : IN4
    GPIO14: D5 Back_motor ENABLE
    GPIO12: D6 Front motor ENABLE
    """
    global smartcar_pins
    smartcar_pins = [machine.Pin(i, machine.Pin.OUT) for i in (L298N_IN1_GPIO, L298N_IN2_GPIO, L298N_IN3_GPIO, L298N_IN4_GPIO)]
    [pin.off() for pin in smartcar_pins]

    #machine_pwm_init(1000, 512)
    global pwm_pins
    pwm_pins = [machine.PWM(machine.Pin(i)) for i in (L298N_ENA_GPIO, L298N_ENB_GPIO)]
    pwm_pins[0].freq(1000)
    pwm_pins[0].duty(512)
    pwm_pins[1].freq(1000)
    pwm_pins[1].duty(920) # 80%

def lm393_ir_port_init():
    def func(v):
        back_motor_stop()
    ir_distence_port = machine.Pin(LM393_OUT_GPIO,  machine.Pin.IN)
    ir_distence_port.irq(trigger=machine.Pin.IRQ_FALLING, handler=func)

def uart2_tmc123g_init(br):

    def func(v):
        print("uart1 recving")

    global uart2
    uart2 = machine.UART(2, baudrate=br)
    uart2.init(115200, bits=8, parity=None, stop=1)
    uart2.irq(trigger=UART_RX_ANY, handler=func)

def start():
    """TODO: Docstring for start.
     :returns: TODO

    """

    # Motor Init
    motor_port_init()

    # LM393 IR Distence Sensor Init
    #lm393_ir_port_init()

    #  UART for TM4c123g MCU
    uart2_tmc123g_init(115200)

    # Wifi configure
    wifi_config('ROCK', '15239432943')

    # http_server_init Port: 80
    # server accept loop
    http_server_accept_loop(http_server_init(80))
