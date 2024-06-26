# Power Window Control System Using FreeRTOS

## Introduction

The Power Window Control System project utilizes the STM microcontroller and FreeRTOS to control the front passenger door window from both the passenger and driver panels. It incorporates manual and one-touch auto open/close functions, a window lock feature, and jam protection. The system includes two limit switches to prevent the window from moving beyond its upper and lower bounds and uses a push button for obstacle detection. By integrating these features, the project aims to enhance passenger safety and convenience through reliable and responsive window control.

## System Basic Features

1. **Manual Open/Close Function**:  
   When the power window switch is pushed or pulled continuously, the window opens or closes until the switch is released.
   
2. **One-Touch Auto Open/Close Function**:  
   When the power window switch is pushed or pulled shortly, the window fully opens or closes.
   
3. **Window Lock Function**:  
   When the window lock switch is turned on, the opening and closing of all windows except the driver’s window is disabled.
   
4. **Jam Protection Function**:  
   This function automatically stops the power window and moves it downward about 0.5 second if foreign matter gets caught in the window during one touch auto close operation.

   ![System Basic Features](Images/systemFeatures.jpg)
   
## Hardware Requirements

Your system must contain the following hardware:

1. **STM32 NUCLEO-F446**:  
   The microcontroller used for system control and task management.
   
2. **Top and Bottom Limit Switches**:  
   To prevent the window from moving beyond its upper and lower bounds.
   
3. **DC Motor**:  
   Used to indicate the operation of the window.
   
4. **Push Buttons**:  
   To operate the up and down movement of the window on both the passenger and driver sides.
   
5. **ON/OFF Switch**:  
   To lock the passenger panel from the driver panel.

## Software Requirements

### Functional Requirements

1. **Task Management**:
   - **Lock Task**: Monitors the lock button state and adjusts the priority of the driver task accordingly.
   - **Jam Task**: Controls the motor to turn it down for a specified duration.
   - **Receive Queue**: Receives commands from lower priority tasks through a queue and controls the motor state accordingly.
   - **Driver Task**: Handles button inputs from the user, determines the operating mode (automatic or manual), and sends control signals to the motor.
   - **Passenger Task**: Similar to the driver task but for passenger buttons.

2. **Motor Control**:  
   The system controls the motor to move the window up, down, or stop based on user inputs, synchronized to prevent conflicting commands.

3. **Button Inputs**:  
   The system monitors button inputs from both the driver and passenger, debouncing to prevent false triggers. Short presses activate automatic mode, and long presses activate manual mode.

4. **Semaphore and Mutex Handling**:  
   The system utilizes semaphores and mutexes for access to shared resources such as the motor and queues, ensuring synchronization and exclusive access.

5. **Interrupt Handling**:  
   The system handles interrupts efficiently to respond to external events like button presses, prioritizing interrupts for timely response.

6. **Error Handling**:  
   The system detects and handles errors such as motor failure or sensor malfunctions, logging error conditions for debugging and troubleshooting.

### Non-Functional Requirements

1. **Performance**:  
   The system responds to user inputs within an acceptable time frame. Task scheduling is optimized to minimize latency and maximize throughput.

2. **Reliability**:  
   The system operates reliably under normal and abnormal conditions, implementing recovery mechanisms to handle faults gracefully.

3. **Portability**:  
   The system is portable across different hardware platforms with minimal modifications, abstracting hardware-specific configurations.

4. **Maintainability**:  
   The code is well-structured and documented for ease of maintenance and future enhancements, following modular design principles.

## Task Descriptions

1. **Lock Task**
   - **Description**: Waits for a semaphore from an ISR, indicating a lock event, checks the lock button state, and adjusts the driver task priority.
   - **Functionality**:
     - Waits for a semaphore to signal a lock event.
     - Checks the lock button state.
     - Sets the driver task priority to 2 and turns on a red LED if the lock button is pressed.
     - Sets the driver task priority to 1 and turns off the red LED if the lock button is released.
   - **Priority**: HIGH (4).

2. **Jam Task**
   - **Description**: Activates the motor to turn it down for 1 second upon receiving a semaphore from an ISR.
   - **Functionality**:
     - Waits for a semaphore indicating a motor jam event.
     - Activates the motor to turn it down.
     - Delays for 1 second using a timer.
     - Stops the motor.
   - **Priority**: HIGH (5).

3. **Receive Queue Task**
   - **Description**: Waits for data from lower priority tasks through a queue and controls the motor state accordingly.
   - **Functionality**:
     - Waits for data from a queue.
     - Receives motor control commands (OFF, UP, DOWN) from other tasks.
     - Controls the motor state based on the received command.
   - **Priority**: MEDIUM (3).

4. **Driver Task**
   - **Description**: Handles button inputs from the user, determines the operating mode, and sends control signals to the motor.
   - **Functionality**:
     - Takes a mutex for exclusive access to critical sections.
     - Polls UP and DOWN buttons for user inputs.
     - Detects short presses for automatic mode and long presses for manual mode.
     - Sends motor control commands (UP, DOWN, OFF) to the queue.
   - **Priority**: LOW (1).

5. **Passenger Task**
   - **Description**: Similar to the driver task but handles passenger button inputs.
   - **Functionality**:
     - Takes a mutex for exclusive access to critical sections.
     - Polls UP and DOWN passenger buttons for inputs.
     - Detects short and long presses to determine the mode of operation.
     - Sends motor control commands to the queue.
   - **Priority**: LOW (1).
  
## Installation

To set up the Power Window Control System, follow these steps:

1. **Hardware Setup**:
   - Connect the STM32 microcontroller to the top and bottom limit switches.
   - Connect the DC motor to the microcontroller to control window movement.
   - Attach push buttons for the up and down controls on both passenger and driver sides.
   - Connect the ON/OFF switch for the window lock functionality.

2. **Software Setup**:
   - Clone this repository to your local machine:
     ```sh
     git clone https://github.com/mostsfamahmoud/FreeRTOS_Power_Window_Control.git
     ```
   - Open the project in STM32CubeIDE.
   - Configure the FreeRTOS settings as required.
   - Compile and upload the firmware to the microcontroller.

3. **Configuration**:
   - Ensure that the limit switches and push buttons are properly connected and configured in the firmware.
   - Test the system to verify the manual and auto open/close functions, window lock, and jam protection features.

## Conclusion

The Power Window Control System is designed to provide a reliable and efficient solution for automating window operations. By adhering to the outlined requirements, the system will meet user expectations for functionality, performance, and reliability.

