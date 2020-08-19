import json
import mysql.connector
import requests
import serial
import time


def get_sensor_data():
    # Point to the serial port and baudrate of the arduino via the serial library
    arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=None)
    # Read 500 bytes of data from the stream
    buffer = arduino.read(500)
    # Decode the stream to ISO-8859-1 format
    converted = buffer.decode('ISO-8859-1')
    # Extract and return a single reading from the captured stream content
    # - - Point to the first open bracket in the content
    index_of_first = converted.find('{', 0, 500)
    # - - Grab everythin from that pointer forward through the stream
    left_trim = converted[index_of_first:]
    # - - From the newly trimmed stream, find the first closing bracket and point to it
    index_of_last = left_trim.find('}', 0, 500)
    # - - Return just the contents as trimmed
    return  left_trim[:index_of_last+1]


def write_sensor_data(params):
    try:
        # Configure the mysql connector
        connection = mysql.connector.connect(host='', database='', user='', password='')
        # This is the **prepared** sql query to be solicited
        query = """INSERT INTO readings (fahrenheit, celsius, humidity, uv_output, uv_intensity, soil_moisture)
           VALUES (%s, %s, %s, %s, %s, %s)"""
        # These are the parameters that will be bound to the query wildcards
        records = (params['fahrenheit'], params['celsius'], params['humidity'], params['outputVoltage'], params['uvIntensity'], params['soilMoisture'])
        # Create a cursor at the connection point
        cursor = connection.cursor()
        # Execute the prepared query
        cursor.execute(query, records)
        connection.commit()
        # Close/End the cursor
        cursor.close()
        # Indicate the try completed successfully
        result = True
    except AttributeError:
        result = False
    return result

# Run the program
while True:

    # Collect the sensor data from the serial cable
    data = get_sensor_data()
    # Use the Json library to make it a native python dictionary
    params = json.loads(data)

    if data:
        # If the data exists, write it to the database
        write_sensor_data(params)
    else:
        print("ERROR: Unable to collect and write data.")
    # Sleep 10 minutes
    time.sleep(600)
