# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import uos, machine
#uos.dupterm(None, 1) # disable REPL on UART(0)
import gc
import webrepl

# 2022-5-21 by Rock
#
import smartcar_http_server

webrepl.start()
gc.collect()

##
smartcar_http_server.start()



