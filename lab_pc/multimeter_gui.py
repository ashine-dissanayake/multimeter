# Multimeter GUI Interface.
# Author: Team 8 (ENGG3800)
# Style - flake8 (using linter)

from PyQt5 import QtCore, QtGui, QtWidgets
from threading import Thread
import serial.tools.list_ports
import queue
import sys
import time

# Send packet size.
PACKET_SIZE = 26


def read_ports():
    """
    Reads a list of available COM ports from this device.

    param:
        (None)
    return:
        (List) list of COM ports.
    """

    ports_data = serial.tools.list_ports.comports()
    return ports_data


class MultimeterGUI(object):
    """
    Multimeter GUI. Creates the layout and the functionality of all
    the buttons present on the layout.
    """

    def init_gui(self, Multimeter):
        """
        Initialise the layout.

        param:
            (None)

        return:
            (None)
        """
        Multimeter.resize(1650, 1000)
        Multimeter.setStyleSheet("background-color: white;")
        Multimeter.setWindowTitle("Multimeter Window")
        Multimeter.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))
        Multimeter.setLayoutDirection(QtCore.Qt.LeftToRight)
        Multimeter.setTabShape(QtWidgets.QTabWidget.Triangular)

        self.centralwidget = QtWidgets.QWidget(Multimeter)
        self.MainTitle = QtWidgets.QLabel(self.centralwidget)
        self.MainTitle.setGeometry(QtCore.QRect(0, 0, 1650, 61))

        # Set font.
        font = QtGui.QFont()
        Multimeter.setFont(font)
        font.setFamily("Arial")
        font.setPointSize(36)
        font.setBold(True)
        font.setUnderline(True)
        font.setWeight(75)
        self.MainTitle.setFont(font)
        self.MainTitle.setAlignment(QtCore.Qt.AlignCenter)
        self.MainTitle.setText("Multimeter PC Communication")

        # Initialise all frames on the layout.
        self.mode = 0
        self.init_mode()
        self.init_hold()
        self.init_message()
        self.init_max_min()
        self.init_brightness_level()
        self.init_serial()
        self.init_continuity()

        Multimeter.setCentralWidget(self.centralwidget)

        # Queue to store the serial input data.
        self._queue = queue.Queue()
        # Initialise a thread to None.
        self._thread = None

        # self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Multimeter)

    def init_message(self):
        """
        Message frame contains dialog input and button.

        param:
            (None)

        return:
            (None)
        """

        # Message frame.
        self.message_frame = QtWidgets.QFrame(self.centralwidget)
        self.message_frame.setGeometry(QtCore.QRect(50, 425, 750, 150))
        self.message_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.message_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.message_frame.setStyleSheet("background-color: rgb(230,230,250);")

        # User input.
        self.message_value = QtWidgets.QLineEdit(self.message_frame)
        self.message_value.setGeometry(QtCore.QRect(180, 70, 500, 65))
        font = QtGui.QFont()  # setting font.

        font.setFamily("Arial")
        font.setPointSize(16)
        self.message_value.setFont(font)
        self.message_value.setText("")  # default input dialog box.
        self.message_value.setAlignment(QtCore.Qt.AlignCenter)
        self.message_value.setStyleSheet("background-color: white;")
        # Setting hte character length to 14.
        self.message_value.setMaxLength(16)

        # Message Title.
        self.message_label = QtWidgets.QLabel(self.message_frame)
        self.message_label.setGeometry(QtCore.QRect(10, 10, 231, 51))
        font.setPointSize(18)
        font.setBold(True)
        self.message_label.setFont(font)
        self.message_label.setText("Message")

    def init_hold(self):
        """"
        Hold frame contains user-friendly button.

        param:
            (None)

        return:
            (None)
        """

        # Hold frame.
        self.hold_frame = QtWidgets.QFrame(self.centralwidget)
        self.hold_frame.setGeometry(QtCore.QRect(50, 600, 750, 150))
        self.hold_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.hold_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.hold_frame.setStyleSheet("background-color: rgb(230,230,250);")

        # Hold label.
        self.hold_label = QtWidgets.QLabel(self.hold_frame)
        self.hold_label.setGeometry(QtCore.QRect(10, 10, 241, 41))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        self.hold_label.setFont(font)
        self.hold_label.setText("HOLD")

        # Hold button.
        self.hold_button = QtWidgets.QPushButton(self.hold_frame)
        self.hold_button.setGeometry(QtCore.QRect(350, 30, 131, 85))
        font.setBold(False)
        font.setPointSize(24)
        self.hold_button.setFont(font)
        self.hold_button.setStyleSheet("background-color: #ff3b3b;")
        self.hold_button.setText("OFF")
        self.hold_button.clicked.connect(self.holdAction)

    def init_max_min(self):
        """
        Displays the multimeter sent measurements.

        param:
            (None)

        return:
            (None)
        """

        self.measurement_frame = QtWidgets.QFrame(self.centralwidget)
        self.measurement_frame.setGeometry(QtCore.QRect(850, 400, 750, 555))
        self.measurement_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.measurement_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.measurement_frame.setStyleSheet("background-color: #fed8b1")

        self.curr_data_lbl = QtWidgets.QLabel(self.measurement_frame)
        self.curr_data_lbl.setGeometry(QtCore.QRect(10, 10, 341, 41))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        font.setWeight(75)
        self.curr_data_lbl.setFont(font)
        self.curr_data_lbl.setText("Current Data")

        self.curr_mode_lbl = QtWidgets.QLabel(self.measurement_frame)
        self.curr_mode_lbl.setGeometry(QtCore.QRect(10, 60, 250, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_mode_lbl.setFont(font)
        self.curr_mode_lbl.setText("Current Mode")

        self.curr_brightness_lbl = QtWidgets.QLabel(self.measurement_frame)
        self.curr_brightness_lbl.setGeometry(QtCore.QRect(10, 100, 300, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_brightness_lbl.setFont(font)
        self.curr_brightness_lbl.setText("Current Brightness Level")

        self.curr_mode_val = QtWidgets.QLabel(self.measurement_frame)
        self.curr_mode_val.setGeometry(QtCore.QRect(325, 60, 225, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_mode_val.setFont(font)
        self.curr_mode_val.setFrameShape(QtWidgets.QFrame.Box)
        self.curr_mode_val.setStyleSheet("background-color: white;")

        self.curr_brightness_val = QtWidgets.QLabel(self.measurement_frame)
        self.curr_brightness_val.setGeometry(QtCore.QRect(325, 100, 225, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_brightness_val.setFont(font)
        self.curr_brightness_val.setFrameShape(QtWidgets.QFrame.Box)
        self.curr_brightness_val.setStyleSheet("background-color: white;")

        self.curr_hold_lbl = QtWidgets.QLabel(self.measurement_frame)
        self.curr_hold_lbl.setGeometry(QtCore.QRect(10, 140, 300, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_hold_lbl.setFont(font)
        self.curr_hold_lbl.setText("Current Hold Status")

        self.curr_hold_val = QtWidgets.QLabel(self.measurement_frame)
        self.curr_hold_val.setGeometry(QtCore.QRect(325, 140, 225, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_hold_val.setFont(font)
        self.curr_hold_val.setFrameShape(QtWidgets.QFrame.Box)
        self.curr_hold_val.setStyleSheet("background-color: white;")

        self.meas_lbl = QtWidgets.QLabel(self.measurement_frame)
        self.meas_lbl.setGeometry(QtCore.QRect(10, 200, 341, 41))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        font.setWeight(75)
        self.meas_lbl.setFont(font)
        self.meas_lbl.setText("Measurement")

        self.meas_button = QtWidgets.QPushButton(self.measurement_frame)
        self.meas_button.setGeometry(QtCore.QRect(557, 480, 161, 61))
        self.meas_button.setStyleSheet("background-color: white;")
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.meas_button.setFont(font)
        self.meas_button.setText("RESET")
        self.meas_button.clicked.connect(self.reset_action)

        self.curr_tab_widget = QtWidgets.QTabWidget(self.measurement_frame)
        self.curr_tab_widget.setGeometry(QtCore.QRect(30, 250, 690, 210))
        self.curr_tab_widget.setStyleSheet("background-color: white;")

        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_tab_widget.setFont(font)

        self.curr_ac_voltage = QtWidgets.QWidget()
        self.curr_dc_voltage = QtWidgets.QWidget()
        self.curr_resistance = QtWidgets.QWidget()
        self.curr_continuity = QtWidgets.QWidget()

        self.curr_tab_widget.addTab(self.curr_ac_voltage, "AC or DC Voltage")

        self.curr_voltage_lbl = QtWidgets.QLabel(self.curr_ac_voltage)
        self.curr_voltage_lbl.setGeometry(QtCore.QRect(10, 10, 240, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_voltage_lbl.setFont(font)
        self.curr_voltage_lbl.setText("Current Voltage (V):")

        self.curr_voltage_val = QtWidgets.QLabel(self.curr_ac_voltage)
        self.curr_voltage_val.setGeometry(QtCore.QRect(275, 10, 400, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_voltage_val.setFont(font)
        self.curr_voltage_val.setFrameShape(QtWidgets.QFrame.Box)
        self.curr_voltage_val.setFrameShadow(QtWidgets.QFrame.Plain)

        self.max_voltage_lbl = QtWidgets.QLabel(self.curr_ac_voltage)
        self.max_voltage_lbl.setGeometry(QtCore.QRect(10, 50, 240, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.max_voltage_lbl.setFont(font)
        self.max_voltage_lbl.setText("Maximum Voltage (V):")

        self.max_voltage_val = QtWidgets.QLabel(self.curr_ac_voltage)
        self.max_voltage_val.setGeometry(QtCore.QRect(275, 50, 400, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.max_voltage_val.setFont(font)
        self.max_voltage_val.setFrameShape(QtWidgets.QFrame.Box)
        self.max_voltage_val.setFrameShadow(QtWidgets.QFrame.Plain)

        self.min_voltage_lbl = QtWidgets.QLabel(self.curr_ac_voltage)
        self.min_voltage_lbl.setGeometry(QtCore.QRect(10, 90, 240, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.min_voltage_lbl.setFont(font)
        self.min_voltage_lbl.setText("Minimum Voltage (V):")

        self.min_voltage_val = QtWidgets.QLabel(self.curr_ac_voltage)
        self.min_voltage_val.setGeometry(QtCore.QRect(275, 90, 400, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.min_voltage_val.setFont(font)
        self.min_voltage_val.setFrameShape(QtWidgets.QFrame.Box)
        self.min_voltage_val.setFrameShadow(QtWidgets.QFrame.Plain)

        self.dc_offset_lbl = QtWidgets.QLabel(self.curr_ac_voltage)
        self.dc_offset_lbl.setGeometry(QtCore.QRect(10, 130, 240, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.dc_offset_lbl.setFont(font)
        self.dc_offset_lbl.setText("DC Offset (V):")

        self.dc_offset_val = QtWidgets.QLabel(self.curr_ac_voltage)
        self.dc_offset_val.setGeometry(QtCore.QRect(275, 130, 400, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.dc_offset_val.setFont(font)
        self.dc_offset_val.setFrameShape(QtWidgets.QFrame.Box)
        self.dc_offset_val.setFrameShadow(QtWidgets.QFrame.Plain)

        self.curr_tab_widget.addTab(self.curr_resistance, "Resistance")
        self.curr_resistance_val = QtWidgets.QLabel(self.curr_resistance)
        self.curr_resistance_val.setGeometry(QtCore.QRect(325, 10, 350, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_resistance_val.setFont(font)
        self.curr_resistance_val.setFrameShape(QtWidgets.QFrame.Box)
        self.curr_resistance_val.setFrameShadow(QtWidgets.QFrame.Plain)
        self.curr_res_lbl = QtWidgets.QLabel(self.curr_resistance)
        self.curr_res_lbl.setGeometry(QtCore.QRect(10, 10, 271, 21))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_res_lbl.setFont(font)
        self.curr_res_lbl.setText("Current Resistance (Ω):")

        self.curr_tab_widget.addTab(self.curr_continuity, "Continuity")
        self.curr_continuity_lbl = QtWidgets.QLabel(self.curr_continuity)
        self.curr_continuity_lbl.setGeometry(QtCore.QRect(10, 10, 300, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_continuity_lbl.setFont(font)
        self.curr_continuity_lbl.setText("Continuity Threshold (Ω)")

        self.curr_continuity_val = QtWidgets.QLabel(self.curr_continuity)
        self.curr_continuity_val.setGeometry(QtCore.QRect(325, 10, 350, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.curr_continuity_val.setFont(font)
        self.curr_continuity_val.setFrameShape(QtWidgets.QFrame.Box)

        self.open_close_result = QtWidgets.QLabel(self.curr_continuity)
        self.open_close_result.setGeometry(QtCore.QRect(325, 50, 350, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.open_close_result.setFont(font)
        self.open_close_result.setFrameShape(QtWidgets.QFrame.Box)

        self.open_closed_label = QtWidgets.QLabel(self.curr_continuity)
        self.open_closed_label.setGeometry(QtCore.QRect(10, 50, 161, 25))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.open_closed_label.setFont(font)
        self.open_closed_label.setText("Open/Closed?")

        # Handler to determine if reset is pressed.
        self.has_reset = False

    def init_brightness_level(self):
        """"
        Brightness level Frame.

        param:
            (None)

        return:
            (None)
        """

        # Initialise brightness frame
        self.brightness_frame = QtWidgets.QFrame(self.centralwidget)
        self.brightness_frame.setGeometry(QtCore.QRect(50, 775, 750, 181))
        self.brightness_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.brightness_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.brightness_frame.setStyleSheet("background-color: \
            rgb(230,230,250);")

        # Initialise brightness title
        self.brightness_lbl = QtWidgets.QLabel(self.brightness_frame)
        self.brightness_lbl.setGeometry(QtCore.QRect(10, 10, 600, 51))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        self.brightness_lbl.setFont(font)
        self.brightness_lbl.setText("Brightness Level")

        # User-select brightness level
        self.brightness_box = QtWidgets.QSpinBox(self.brightness_frame)
        self.brightness_box.setGeometry(QtCore.QRect(250, 70, 191, 90))
        self.brightness_box.setStyleSheet("background-color: white;")
        self.brightness_box.setAlignment(QtCore.Qt.AlignCenter)
        self.brightness_box.setMinimum(1)  # Minimum brightness level
        self.brightness_box.setMaximum(5)  # Maximum brightness level
        self.brightness_box.setValue(3)
        font.setBold(False)
        font.setPointSize(18)
        self.brightness_box.setFont(font)

    def init_mode(self):
        """"
        Mode Frame.

        param:
            (None)

        return:
            (None)
        """

        # Mode frame.
        self.mode_frame = QtWidgets.QFrame(self.centralwidget)
        self.mode_frame.setGeometry(QtCore.QRect(50, 100, 750, 125))
        self.mode_frame.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.mode_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.mode_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.mode_frame.setStyleSheet("background-color: rgb(230,230,250);")

        # Mode label
        self.mode_lbl = QtWidgets.QLabel(self.mode_frame)
        self.mode_lbl.setGeometry(QtCore.QRect(10, 10, 341, 31))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        self.mode_lbl.setFont(font)
        self.mode_lbl.setText("Mode Selection")

        # Initialise user-selected mode box.
        self.mode_box = QtWidgets.QComboBox(self.mode_frame)
        self.mode_box.setGeometry(QtCore.QRect(150, 60, 350, 35))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(12)
        self.mode_box.setFont(font)

        self.mode_box.addItem("DC Voltage Mode")
        self.mode_box.addItem("AC Voltage Mode")
        self.mode_box.addItem("Resistance Mode")
        self.mode_box.addItem("Continuity Mode")
        self.mode_box.setStyleSheet("background-color: white;")

    def init_continuity(self):
        """"
        Continuity Frame.

        param:
            (None)

        return:
            (None)
        """

        # Initialise continuity threshold frame.
        self.continuity_frame = QtWidgets.QFrame(self.centralwidget)
        self.continuity_frame.setGeometry(QtCore.QRect(50, 250, 750, 150))
        self.continuity_frame.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.continuity_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.continuity_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.continuity_frame.setStyleSheet("background-color: \
            rgb(230,230,250);")

        # Continuity threshold title.
        self.continuity_title = QtWidgets.QLabel(self.continuity_frame)
        self.continuity_title.setGeometry(QtCore.QRect(10, 10, 341, 31))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        font.setWeight(75)
        self.continuity_title.setFont(font)
        self.continuity_title.setText("Coninuity Threshold")

        # User-selected continuity threshold.
        font.setBold(False)
        self.double_spin_box = QtWidgets.QDoubleSpinBox(self.continuity_frame)
        self.double_spin_box.setGeometry(QtCore.QRect(250, 50, 191, 90))
        self.double_spin_box.setAlignment(QtCore.Qt.AlignCenter)
        self.double_spin_box.setDecimals(1)
        self.double_spin_box.setMaximum(20.0)
        self.double_spin_box.setSingleStep(0.1)
        self.double_spin_box.setStyleSheet("background-color: white;")
        self.double_spin_box.setFont(font)

    def init_serial(self):
        """"
        Serial Frame.

        param:
            (None)

        return:
            (None)
        """

        # Initialise serial frame.
        self.serial_frame = QtWidgets.QFrame(self.centralwidget)
        self.serial_frame.setGeometry(QtCore.QRect(850, 100, 750, 275))
        self.serial_frame.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.serial_frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.serial_frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.serial_frame.setStyleSheet("background-color: rgb(230,230,250);")

        # Initialise serial title.
        self.serial_lbl = QtWidgets.QLabel(self.serial_frame)
        self.serial_lbl.setGeometry(QtCore.QRect(10, 10, 341, 31))
        font = QtGui.QFont()
        font.setFamily("Arial")
        font.setPointSize(18)
        font.setBold(True)
        font.setWeight(75)
        self.serial_lbl.setFont(font)
        self.serial_lbl.setText("Serial")

        # Connected or Not Connected. Default Not Connected.
        self.is_connected = QtWidgets.QLabel(self.serial_frame)
        self.is_connected.setGeometry(QtCore.QRect(150, 3, 341, 40))
        self.is_connected.setFont(font)
        self.is_connected.setText("[No Connection]")
        self.is_connected.setStyleSheet("color: red;")

        # Port Label
        self.port_lbl = QtWidgets.QLabel(self.serial_frame)
        self.port_lbl.setGeometry(QtCore.QRect(10, 60, 250, 35))
        font.setBold(False)
        font.setPointSize(12)
        self.port_lbl.setFont(font)
        self.port_lbl.setText("Port")

        # Baudrate Label
        self.baudrate_lbl = QtWidgets.QLabel(self.serial_frame)
        self.baudrate_lbl.setGeometry(QtCore.QRect(10, 120, 250, 35))
        self.baudrate_lbl.setFont(font)
        self.baudrate_lbl.setText("Baudrate")

        # Initialise all the ports availible to connect to.
        self.port_list = \
            [port.device for port in serial.tools.list_ports.comports()]
        self.port_box = QtWidgets.QComboBox(self.serial_frame)
        self.port_box.setGeometry(QtCore.QRect(275, 60, 225, 35))
        self.port_box.setFont(font)

        for port in self.port_list:
            self.port_box.addItem(str(port))

        self.port_box.setStyleSheet("background-color: white;")

        # Initialise user-selected baudrate.
        self.baud_box = QtWidgets.QComboBox(self.serial_frame)
        self.baud_box.setGeometry(QtCore.QRect(275, 120, 225, 35))
        self.baud_box.setFont(font)
        self.baud_box.addItem("9600")
        self.baud_box.addItem("115200")
        self.baud_box.addItem("1200")
        self.baud_box.setStyleSheet("background-color: white;")

        # Connect button.
        self.connect_button = QtWidgets.QPushButton(self.serial_frame)
        self.connect_button.setGeometry(QtCore.QRect(30, 200, 161, 61))
        self.connect_button.setFont(font)
        self.connect_button.setStyleSheet("background-color: white;")
        self.connect_button.setText("CONNECT")
        self.connect_button.clicked.connect(self.start_thread)

        # Send to GUI button
        self.update_button = QtWidgets.QPushButton(self.serial_frame)
        self.update_button.setGeometry(QtCore.QRect(557, 200, 161, 61))
        self.update_button.setFont(font)
        self.update_button.setStyleSheet("background-color: white;")
        self.update_button.setText("SEND TO\nMULTIMETER")
        self.update_button.clicked.connect(self.send_serial)

    def holdAction(self):
        """
            Method called when hold button is pressed.

            param:
                (None)
            return:
                (None)
        """

        _translate = QtCore.QCoreApplication.translate

        # Determining whether button is on or off.
        if self.hold_button.text() == "OFF":
            self.hold_button.setText(_translate("Multimeter", "ON"))
            self.hold_button.setStyleSheet("background-color: #5cab54;")

        elif self.hold_button.text() == "ON":
            self.hold_button.setText(_translate("Multimeter", "OFF"))
            self.hold_button.setStyleSheet("background-color: #ff3b3b;")

    def send_serial(self):
        """
            Method to send to serial.

            param:
                (None)
            return:
                (None)
        """

        # Initialising packet.
        preamble = 0xAA

        mode = self.mode_box.currentIndex()

        # Formatting continuity threshold.
        cont_thres = "{0:.2f}".format(self.double_spin_box.value()).zfill(5)

        # Hold section.
        hold = 0
        if (self.hold_button.text() == "ON"):
            hold = 1

        # Brighness.
        blvl = self.brightness_box.value()

        message = self.message_value.text()

        packet_preamble = "0x{:02X}".format(0xAA)

        # Full packet received
        print('---------------------------')
        print('|       SEND PACKET       |')
        print('---------------------------')
        print('| PREAMBLE:', packet_preamble)
        print('| MODE:', mode)
        print('| CONTINUITY THRES:', cont_thres)
        print('| HOLD STATUS:', hold)
        print('| BRIGHTNESS LVL:', blvl)
        print('| RESET:', self.has_reset)
        print('| MESSAGE:', message)
        print('---------------------------')

        # Creating the packet.
        response_buffer = chr(preamble) + str(mode) + cont_thres + str(hold) \
            + str(blvl) + str(int(self.has_reset)) + message
        formated_buffer = str(response_buffer).ljust(PACKET_SIZE, '\0')

        print(len(formated_buffer), " : ", formated_buffer)
        self._thread.get_ser().write(formated_buffer.encode())
        self.has_reset = False

    def change_mode(self):
        """
        When the tab is changed in the mode frame.

        param:
            (None)
        return:
            (None)
        """
        self.mode = self.tabWidget.currentIndex()

    def reset_action(self):
        """
        Resets the maximum and minimum values.

        param:
            (None)
        return:
            (None)
        """
        self.min_voltage_val.setText("0.00")
        self.max_voltage_val.setText("0.00")

        #  Setting handler to true.
        self.has_reset = True
        self.send_serial()

    def start_thread(self):
        """
        Starting thread and connect to selected port.

        param:
            (None)
        return:
            (None)
        """

        # Start serial threading.
        port = self.port_box.currentText()
        baudrate = self.baud_box.currentText()

        # Initialising thread.
        self._thread = SerialThreading(self._queue, port, baudrate)
        try:
            self._thread.start()
            check_thread = Thread(target=self.process_serial_input)
            check_thread.start()
            print("(THREAD) Successfully connected to " + port + "!")
            self.is_connected.setText("[Connected]")
            self.is_connected.setStyleSheet("color: green;")
        except Exception as e:
            # Ensure that the layout does not freeze.
            print(f"(ERROR {e}) Unable to connect to " + port + "!")

    def update_mode(self, message):
        """
        Process the MODE packet sent from the GUI and assign  the vaules to
        various labels.

        param:
            (None)
        return:
            (None)
        """

        try:
            # Mode type.
            if (int(message[4]) == 0):  # DC
                self.curr_mode_val.setText("DC Voltage")
                self.curr_tab_widget.setCurrentIndex(0)
                unit = message[5]
                temp = float(message[6:12])
                self.curr_voltage_val.setText(str(temp))
                temp = float(message[12:18])
                self.min_voltage_val.setText(str(temp))
                temp = float(message[18:24])
                self.max_voltage_val.setText(str(temp))
                self.dc_offset_val.setText("0.00")

            elif (int(message[4]) == 1):  # AC
                self.curr_mode_val.setText("AC Voltage")
                self.curr_tab_widget.setCurrentIndex(0)
                unit = message[5]
                # Getting particular sections of the payload.
                temp = float(message[6:12])
                self.curr_voltage_val.setText(str(temp))
                temp = float(message[12:18])
                self.min_voltage_val.setText(str(temp))
                temp = float(message[18:24])
                self.max_voltage_val.setText(str(temp))
                temp = float(message[24:32])
                self.dc_offset_val.setText(str(temp))
                unit = message[5]

            elif (int(message[4]) == 2):  # RESISTANCE
                self.curr_mode_val.setText("Resistance")
                self.curr_tab_widget.setCurrentIndex(1)
                unit = message[5]

                if (unit == '0'):
                    unit = ''

                resistance_value = []

                for ind in range(6, 20):
                    if message[ind] == '\0':
                        break
                    resistance_value.append(message[ind])
                self.curr_res_lbl.setText(f"Current Resistance ({unit}Ω):")
                self.curr_resistance_val.setText(''.join(resistance_value))

            elif (int(message[4]) == 3):  # CONTINUITY
                self.curr_mode_val.setText("Continuity")
                self.curr_tab_widget.setCurrentIndex(2)

                if message[6] == '0':
                    self.open_close_result.setText("OPEN")
                elif message[6] == '1':
                    self.open_close_result.setText("CLOSED")

                cont_thres = float(message[7:13])
                self.curr_continuity_val.setText("{0:.2f}".format(cont_thres))

        except Exception as ex:
            print(f"(Error:) {ex}")
            pass

    def update_hold(self, message):
        """
        Process the HOLD packet sent from the GUI and assign the
        vaules to labels.

        param:
            (None)
        return:
            (None)
        """

        if (int(message[4]) == 0):
            self.curr_hold_val.setText("OFF")
        elif (int(message[4]) == 1):
            self.curr_hold_val.setText("ON")

    def update_brightness_lvl(self, message):
        """
        Process the BLVL packet sent from the GUI and assign the values
        to various labels.

        param:
            (None)
        return:
            (None)
        """

        self.curr_brightness_val.setText(message[4])

    def process_serial_input(self):
        """
        The main loop which receives and process the serial input data.
        Packets are 31 bytes long.

        param:
            (None)
        return:
            (None)
        """

        input_buffer = [0] * 31
        is_packet = 0
        buffer_index = 0

        while True:

            # Get value from queue.
            input_data = self._thread.get_queue().get()
            self._thread.flush_input()

            # JOIN preamble
            if input_data == b'\xbb':
                joinResponse = chr(0xBB)
                self._thread.get_ser().write(joinResponse.encode())

            # Process packets containing either HOLD, BLVL and MODE.
            if (input_data == b'\xaa'):
                is_packet = 1
            elif (is_packet == 1):  # Store values.
                input_buffer[buffer_index] = input_data
                buffer_index = buffer_index + 1

            if (buffer_index == 31):  # Process packet.
                packet_good = True
                for i in range(1, 31):
                    try:
                        input_buffer[i] = input_buffer[i].decode()  # Decode.
                    except Exception as e:
                        # Ensure that the layout does not freeze.
                        packet_good = False
                        print(f"(ERROR {e}): Unable to complete")
                        is_packet = 0
                        buffer_index = 0

                if packet_good is True:
                    packet_payload = ''.join(input_buffer[1:])

                    is_packet = 0
                    buffer_index = 0

                    packet_preamble = "0x{:02X}".format(0xAA)

                    # Full packet received
                    print('----------------------------')
                    print('|      RECEIVE PACKET      |')
                    print('----------------------------')
                    print('| PREAMBLE:', packet_preamble)
                    print('| TYPE:', input_buffer[0])
                    print('| PAYLOAD:', packet_payload)
                    print('----------------------------')

                # Raw data.
                    if (input_buffer[0] == b'\x12'):
                        self.update_mode(packet_payload)
                    elif (input_buffer[0] == b'\x13'):
                        self.update_hold(packet_payload)
                    elif (input_buffer[0] == b'\x14'):
                        self.update_brightness_lvl(packet_payload)


class SerialThreading(Thread):
    """ Threading class which is used to receive serial input data. """

    def __init__(self, data_queue, port, baud):
        super().__init__()
        self._data_queue = data_queue
        self._port = port
        self._ser = None
        self._baud = baud

    def run(self):
        # Check if serial ports are available
        if self._port != 'None':
            self._ser = serial.Serial(self._port, self._baud, timeout=1)
            # print("Connection Successful!")
            # Sleep the thread for 0.2s
            time.sleep(2)

            while True:
                if self._ser.inWaiting():
                    input_data = self._ser.read(1)
                    self._data_queue.put(input_data)
        else:
            print('Connection Issue!')

    def get_queue(self):
        """
        :return: The queue of the thread object.
        """
        return self._data_queue

    def get_ser(self):
        """
        :return: The serial object of the thread object.
        """
        return self._ser

    def flush_input(self):
        """
        Flush the serial io.

        param:
            (None)
        return:
            (None)
        """

        self._ser.flush()


def main():
    app = QtWidgets.QApplication(sys.argv)
    Multimeter = QtWidgets.QMainWindow()
    gui = MultimeterGUI()
    gui.init_gui(Multimeter)
    Multimeter.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
