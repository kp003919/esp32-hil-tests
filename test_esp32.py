import serial
import time

PORT = "COM7"  # Update this to your ESP32's serial port
BAUD = 115200  # Match this to your ESP32's baud rate

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

def send_test(cmd):
    ser.reset_input_buffer()
    ser.write((cmd + "\n").encode())

    deadline = time.time() + 2
    while time.time() < deadline:
        line = ser.readline().decode(errors="ignore").strip()
        if line.startswith("[TEST]"):
            return line
    return None

# ---- Run tests ----
tests = ["PING", "TEST_UPTIME", "TEST_DHT", "TEST_GPS", "TEST_RTLS", "TEST_PULSE"]

for t in tests:
    print(f"\nRunning {t}...")
    resp = send_test(t)   
    print("Response:", resp)
    
