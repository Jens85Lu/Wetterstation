import serial
import matplotlib.pyplot as plt
from collections import deque

ser = serial.Serial('/dev/ttyACM3', 115200)  # Adjust the port and baud rate as needed

data = deque(maxlen=100)  # Store the last 100 temperature readings

plt.ion()  # Enable interactive mode
fig, ax = plt.subplots()
line, = ax.plot([],[])

try:
    while True:
        try:
            value = float(ser.readline().decode().strip())
            print(f"Temperature: {value} °C")
            data.append(value)  # Add new value to the deque
            line.set_ydata(data)  # Update the line data
            line.set_xdata(range(len(data)))  # Update the x-axis data
            #ax.set_xlim(0, len(data)-1)  # Set x-axis limits
            #ax.set_ylim(20, 40)  # Set y-axis limits
            ax.relim()  # Recalculate limits
            ax.autoscale_view()  # Autoscale the view
            plt.draw()  # Redraw the plot
            plt.pause(0.01)  # Pause to allow the plot to update
        except ValueError:
            pass
except KeyboardInterrupt:
    ser.close()  # Close the serial connection on exit
    print("Plotting stopped.")
    plt.close()
