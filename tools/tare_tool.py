import paho.mqtt.client as mqtt
import logging as log
import json
import time

COMMAND_TARE = "tare"
COMMAND_NEXT = "next"

TOPIC_STATUS = "status"
TOPIC_CONTROL = "control"

STATUS_TARE_WAIT_EMPTY = "tare_wait_empty"
STATUS_TARE_WAIT_WEIGHT = "tare_wait_weight"

mqtt_broker: str = "localhost"
mqtt_broker_port: int = 1883

control_pqt = {
    "time": 0,
    "type": "control",
    "name": "demo",
    "commands":["none"],
    "data": 0
}

message_recieved = False
message = {}

pubClient = mqtt.Client()
subClient = mqtt.Client()
        
# The callback for when the client receives a CONNACK response from the server.
def on_connect_pub(client: mqtt.Client, userdata, flags, rc):
    log.info(f"Mqtt client connected with result code {rc}\n")

# The callback for when the client receives a CONNACK response from the server.
def on_connect_sub(client: mqtt.Client, userdata, flags, rc):
    log.info(f"Mqtt client connected with result code {rc}\n")

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(TOPIC_STATUS)

def on_publish(client: mqtt.Client, userdata, result):
    log.debug(f"Published with result: {result}.")
    pass

def publish(topic: str, message: str):
    ret = pubClient.publish(topic, message)
    log.debug(f"Published: [{topic}]:{message}->{ret}\n")

# The callback for when a PUBLISH message is received from the server.
def on_message(client: mqtt.Client, userdata, msg: mqtt.MQTTMessage):
    log.debug(f"Recieved: [{msg.topic}]:{msg.payload}\n")
    message = json.loads(msg.payload)
    message_recieved = True

if __name__ == "__main__":

    pubClient.on_connect = on_connect_pub
    pubClient.on_publish = on_publish
    pubClient.connect(mqtt_broker, mqtt_broker_port, 60)

    subClient.on_connect = on_connect_sub
    subClient.on_message = on_message
    subClient.connect_async(mqtt_broker, mqtt_broker_port, 60)
    subClient.loop_start()

    log.info("This script runs a tare/calibration on a load cell sensor.")
    cell_name = input("Enter the sensor name of the cell to calibrate:")

    log.info(f"Sending command to cell {cell_name}.")
    control_pqt["name"] = cell_name
    control_pqt["commands"][0] = COMMAND_TARE

    publish(TOPIC_CONTROL, json.dumps(control_pqt))
    #while(not message_recieved):
    #    pass
    time.sleep(2)

    message_recieved = False
    #if(not (STATUS_TARE_WAIT_EMPTY in message["status"])):
    #    log.error(f"Load cell {cell_name} didn't answer with {STATUS_TARE_WAIT_EMPTY}, can't proceed.")
    #    exit(1)

    input(f"Load cell {cell_name} is ready, remove any weigth and press enter.")

    control_pqt["commands"][0] - COMMAND_NEXT
    publish(TOPIC_CONTROL, json.dumps(control_pqt))
    #while(not message_recieved):
    #    pass
    time.sleep(2)

    message_recieved = False
    #if(not (STATUS_TARE_WAIT_EMPTY in message["status"])):
    #    log.error(f"Load cell {cell_name} didn't answer with {STATUS_TARE_WAIT_WEIGHT}, can't proceed.")
    #    exit(1)

    weight = input(f"Load cell zeroed, put a know weigth on it and enter it's known value here (in kg): ")

    control_pqt["data"] = weight
    publish(TOPIC_CONTROL, json.dumps(control_pqt))
   
    log.info(f"Calibration done for load cell {cell_name}, the cell should have returned to normal behaviour.")

    subClient.loop_stop()
    exit(0)
