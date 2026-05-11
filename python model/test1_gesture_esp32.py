from edge_impulse_linux.runner import Runner
import serial

# Step 1: Load the model
runner = Runner('D:\\data\\machine learning\\edge impulse projects\\esp geture recognintion\\python model\\leo_16572-project-1-linux-x86_64-v5.eim')  # <-- update this path
runner.init()

# Step 2: Connect to ESP32
ser = serial.Serial('COM4', 115200)  # Replace with your COM port

# Step 3: Read and classify data
while True:
    line = ser.readline().decode().strip()
    try:
        values = [float(x) for x in line.split(',')]  # Assumes ax,ay,az,gx,gy,gz
        result = runner.classify(values)
        print(result['classification'])
    except Exception as e:
        print(f"Error: {e}")
