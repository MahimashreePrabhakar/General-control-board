import tkinter as tk

from tkinter import ttk

import serial

import serial.tools.list_ports
 
class STM32Controller:

    def __init__(self, root):

        self.root = root

        self.root.title("STM32 Controller")
 
        # Serial connection variables

        self.serial_conn = None

        self.com_port = tk.StringVar()

        self.baud_rate = tk.StringVar(value="9600")
 
        # Create GUI components

        self.create_widgets()
 
    def create_widgets(self):

        # Connection Panel

        connection_frame = ttk.LabelFrame(self.root, text="Connection Panel")

        connection_frame.grid(row=0, column=0, padx=10, pady=10, sticky="ew")
 
        ttk.Label(connection_frame, text="COM Port:").grid(row=0, column=0, padx=5, pady=5)

        self.com_port_combo = ttk.Combobox(connection_frame, textvariable=self.com_port)

        self.com_port_combo['values'] = self.get_serial_ports()

        self.com_port_combo.grid(row=0, column=1, padx=5, pady=5)
 
        ttk.Label(connection_frame, text="Baud Rate:").grid(row=0, column=2, padx=5, pady=5)

        ttk.Entry(connection_frame, textvariable=self.baud_rate).grid(row=0, column=3, padx=5, pady=5)
 
        self.connect_button = ttk.Button(connection_frame, text="Connect", command=self.toggle_connection)

        self.connect_button.grid(row=0, column=4, padx=5, pady=5)
 
        # PWM Control

        pwm_frame = ttk.LabelFrame(self.root, text="PWM Control")

        pwm_frame.grid(row=1, column=0, padx=10, pady=10, sticky="ew")
 
        self.pwm_values = {}

        pwm_labels = ["Extruder 1M", "Extruder 2M", "Feed 1M", "Feed 2M", "L3", "L4", "L1", "L2"]

        for i, label in enumerate(pwm_labels, start=1):

            ttk.Label(pwm_frame, text=label).grid(row=i, column=0, padx=5, pady=5)

            self.pwm_values[label] = tk.StringVar(value="0")

            ttk.Entry(pwm_frame, textvariable=self.pwm_values[label]).grid(row=i, column=1, padx=5, pady=5)
 
        ttk.Button(pwm_frame, text="Send PWM Values", command=self.send_pwm_values).grid(row=len(pwm_labels) + 1, column=0, columnspan=2, pady=10)
 
        # GPIO Control

        gpio_frame = ttk.LabelFrame(self.root, text="GPIO Control")

        gpio_frame.grid(row=2, column=0, padx=10, pady=10, sticky="ew")
 
        self.gpio_states = {}

        gpio_labels = ["EXT1_EN", "EXT1_DIR", "EXT2_EN", "EXT2_DIR", "FEED1_EN", "FEED1_DIR", "FEED2_EN", "FEED2_DIR"]

        for i, label in enumerate(gpio_labels, start=1):

            ttk.Label(gpio_frame, text=label).grid(row=i, column=0, padx=5, pady=5)

            self.gpio_states[label] = tk.BooleanVar(value=False)

            ttk.Checkbutton(gpio_frame, variable=self.gpio_states[label]).grid(row=i, column=1, padx=5, pady=5)
 
        ttk.Button(gpio_frame, text="Update GPIO States", command=self.update_gpio_states).grid(row=len(gpio_labels) + 1, column=0, columnspan=2, pady=10)
 
        # Status and Logs

        status_frame = ttk.LabelFrame(self.root, text="Status and Logs")

        status_frame.grid(row=3, column=0, padx=10, pady=10, sticky="ew")
 
        self.status_label = ttk.Label(status_frame, text="Disconnected")

        self.status_label.grid(row=0, column=0, padx=5, pady=5)
 
        self.log_text = tk.Text(status_frame, height=10, width=50)

        self.log_text.grid(row=1, column=0, padx=5, pady=5)
 
    def get_serial_ports(self):

        ports = serial.tools.list_ports.comports()

        return [port.device for port in ports]
 
    def toggle_connection(self):

        if self.serial_conn and self.serial_conn.is_open:

            self.serial_conn.close()

            self.connect_button.config(text="Connect")

            self.status_label.config(text="Disconnected")

            self.log("Disconnected from serial port.")

        else:

            try:

                self.serial_conn = serial.Serial(self.com_port.get(), self.baud_rate.get(), timeout=1)

                self.connect_button.config(text="Disconnect")

                self.status_label.config(text="Connected")

                self.log(f"Connected to {self.com_port.get()} at {self.baud_rate.get()} baud.")

            except Exception as e:

                self.log(f"Failed to connect: {e}")
 
    def send_pwm_values(self):

        if self.serial_conn and self.serial_conn.is_open:

            for i, (label, value) in enumerate(self.pwm_values.items(), start=1):

                try:

                    pwm_value = int(value.get())

                    if 0 <= pwm_value <= 255:

                        command = f"PWM{i}_{pwm_value}\r\n"

                        self.serial_conn.write(command.encode())

                        self.log(f"Sent: {command.strip()}")

                    else:

                        self.log(f"Invalid PWM value for {label}: {pwm_value}")

                except ValueError:

                    self.log(f"Invalid input for {label}: {value.get()}")

        else:

            self.log("Serial connection is not open.")
 
    def update_gpio_states(self):

        if self.serial_conn and self.serial_conn.is_open:

            for i, (label, state) in enumerate(self.gpio_states.items(), start=1):

                command = f"GPIO{i}_{int(state.get())}\n"

                self.serial_conn.write(command.encode())

                self.log(f"Sent: {command.strip()}")

        else:

            self.log("Serial connection is not open.")
 
    def log(self, message):

        self.log_text.insert(tk.END, message + "\n")

        self.log_text.see(tk.END)
 
if __name__ == "__main__":

    root = tk.Tk()

    app = STM32Controller(root)

    root.mainloop()
 