# STM32 Microcontroller Laboratory Projects

![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Proteus](https://img.shields.io/badge/Proteus-1C79C0?style=for-the-badge&logo=proteus&logoColor=white)



![STM32](https://github.com/vophamk23/Microprocessor_Microcontroller_HCMUT/blob/main/STM32%20LAB%202%20-%20TIMER%20INTERRUPT%20and%20LED%20SCANNING/11.%20Digital%20Clock%20and%20Led%20Matrix%20System.png)
![C](https://github.com/vophamk23/Microprocessor_Microcontroller_HCMUT/blob/main/STM32%20LAB%203%20-%20BUTTONS%20SWITCHES/vTraffic%20Light%20System.png)
![Proteus](https://github.com/vophamk23/Microprocessor_Microcontroller_HCMUT/blob/main/STM32%20LAB%205%20-%20UART%20ADC/UART-ADC.png)


> Complete laboratory exercises for embedded systems programming using STM32F103C6 microcontroller

**Course:** Microprocessor & Microcontroller - HCMUT | **Semester:** 2024-2025

---

## 📋 Overview

Collection of 5 laboratory projects covering embedded systems fundamentals to advanced concepts with:
- STM32CubeIDE project files
- Proteus circuit simulations
- Detailed documentation
- Well-commented source code

**Target MCU:** STM32F103C6TX (ARM Cortex-M3)

---

## 🛠️ Requirements

**Hardware:**
- STM32F103C6 (Blue Pill) / Nucleo Board
- ST-Link V2 programmer
- Basic components: LEDs, 7-segment displays, buttons, resistors

**Software:**
- STM32CubeIDE (v1.13.0+)
- Proteus Design Suite 8.x
- STM32CubeProgrammer

**Setup:**
```bash
git clone https://github.com/yourusername/stm32-microcontroller-labs.git
cd "STM32 LAB 1 - LED ANIMATION"
# Open in STM32CubeIDE: File -> Open Projects from File System
```

---

## 📂 Structure

```
Microprocessor_Microcontroller_HCMUT/
├── STM32 LAB 1 - LED ANIMATION/
│   ├── Ex1_ToggleLED/
│   ├── Ex2_TrafficLight/
│   ├── Ex3-10.../
│   └── LAB1_Requirements.pdf
├── STM32 LAB 2 - TIMER INTERRUPT/
├── STM32 LAB 3 - BUTTONS SWITCHES/
├── STM32 LAB 4 - COOPERATIVE SCHEDULER/
└── STM32 LAB 5 - UART ADC/
```

---

## 🧪 Labs Overview

### LAB 1: LED Animation & Basic I/O
**Difficulty:** ⭐⭐☆☆☆ | **Duration:** Week 1-2

**Topics:** GPIO configuration, LED control, 7-segment displays, multiplexing

**Key Exercises:**
- Traffic light systems (2-way, 4-way)
- 7-segment countdown displays
- 12-LED analog clock

```c
// Example: 7-Segment Display
void display7SEG(int num) {
    const uint8_t segments[10] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
    };
    HAL_GPIO_WritePort(GPIOB, segments[num]);
}
```

---

### LAB 2: Timer Interrupt & LED Scanning
**Difficulty:** ⭐⭐⭐☆☆ | **Duration:** Week 3-4

**Topics:** Hardware timers, interrupts, software timers, LED matrix scanning

**Key Concepts:**
- Timer configuration (TIM2)
- Interrupt Service Routines
- Display multiplexing
- 8x8 LED matrix control

```c
// Software Timer Implementation
void setTimer(int index, int value)
{
    if (index < MAX_COUNTER)
    {                                 
        timer_counter[index] = value;
        timer_flag[index] = 0;        
    }
}

int isTimerExpired(int index)
{
    if (index < MAX_COUNTER)
    { // Kiểm tra chỉ số có hợp lệ không
        if (timer_flag[index] == 1)
        {                         
            timer_flag[index] = 0; 
            return 1;             
        }
    }
    return 0; 
}

void timerRun()
{
    for (int i = 0; i < MAX_COUNTER; i++)
    { // Quét qua tất cả các timer
        if (timer_counter[i] > 0)
        {                      
            timer_counter[i]--; 
            if (timer_counter[i] <= 0)
            {                      
                timer_flag[i] = 1; 
            }
        }
    }
}

```

---

### LAB 3: Buttons & Switches with FSM
**Difficulty:** ⭐⭐⭐⭐☆ | **Duration:** Week 5-6

**Topics:** Button debouncing, Finite State Machines, state transitions

**Key Features:**
- 3-sample debouncing algorithm
- Long press detection (2s threshold)
- Multi-mode traffic controller
- Mode switching with settings

```c
// Button Debouncing
void button_reading() {
    for(int i = 0; i < NUM_BUTTONS; i++) {
        debounceButtonBuffer3[i] = debounceButtonBuffer2[i];
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(BUTTON_PORT, button_pins[i]);
        
        if(debounceButtonBuffer1[i] == debounceButtonBuffer2[i] && 
           debounceButtonBuffer2[i] == debounceButtonBuffer3[i]) {
            if(buttonBuffer[i] != debounceButtonBuffer3[i]) {
                buttonBuffer[i] = debounceButtonBuffer3[i];
                if(buttonBuffer[i] == PRESSED_STATE) {
                    flagForButtonPress[i] = 1;
                }
            }
        }
    }
}
```

**FSM States:**
- AUTO mode: Normal traffic operation
- MODE 2-4: Modify RED/YELLOW/GREEN durations

---

### LAB 4: Cooperative Scheduler
**Difficulty:** ⭐⭐⭐⭐☆ | **Duration:** Week 7-8

**Topics:** Task scheduling, Round-robin, Task Control Blocks (TCB)

**Scheduler Functions:**
```c
void SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Update(void);        // Called in timer ISR
void SCH_Dispatch_Tasks(void); // Called in main loop
uint8_t SCH_Delete_Task(uint32_t taskID);
```

**Example Usage:**
```c
int main(void) {
    SCH_Init();
    
    SCH_Add_Task(task_LED_blink, 0, 500);      // Every 500ms
    SCH_Add_Task(task_read_button, 0, 10);     // Every 10ms
    SCH_Add_Task(task_display_update, 0, 2);   // Every 2ms
    
    HAL_TIM_Base_Start_IT(&htim2);
    
    while(1) {
        SCH_Dispatch_Tasks();
    }
}
```

---

### LAB 5: UART & ADC Communication
**Difficulty:** ⭐⭐⭐⭐⭐ | **Duration:** Week 9-10

**Topics:** UART protocol, ADC reading, command parser, sensor data

**UART Configuration:**
```c
void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    HAL_UART_Init(&huart2);
}
```

**ADC Reading:**
```c
uint32_t read_adc(void) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return value;
}

float adc_to_voltage(uint32_t adc_value) {
    return (adc_value * 3.3) / 4095.0; // 12-bit ADC, Vref=3.3V
}
```

**Command Protocol:**
- Format: `!COMMAND#`
- Examples: `!RST#`, `!ADC#`, `!LED1#`

**Command Parser:**
```c
typedef enum {
    WAIT_START,
    RECEIVING_COMMAND,
    COMMAND_COMPLETE
} ParserState;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // State machine for parsing commands
    // Handles: !RST#, !ADC#, !LED1#, !LED0#
}
```

---

## 🎓 Key Concepts Learned

| Lab | Core Concepts |
|-----|---------------|
| **1** | GPIO, HAL library, LED control, multiplexing |
| **2** | Timers, interrupts, ISR, display scanning |
| **3** | Debouncing, FSM design, event handling |
| **4** | Task scheduling, TCB, cooperative multitasking |
| **5** | UART, ADC, protocols, command parsing |

---

## 📚 Resources

- **STM32 HAL Documentation:** [stm32-base.org](https://stm32-base.org/)
- **Reference Manual:** STM32F103xx RM0008
- **Datasheet:** STM32F103C6 DS5319
- **IDE:** [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

---

## 👥 Contributors

**Course:** Microprocessor & Microcontroller  
**University:** Ho Chi Minh City University of Technology (HCMUT)  
**Semester:** 2024-2025

---

## 📝 License

This project is for educational purposes as part of HCMUT coursework.
