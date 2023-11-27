# Module-integration

<hr>

- [Module-integration](#module-integration)
    - [1. Directory Structure](#1-directory-structure)
    - [2. Libraries](#2-libraries)
      - [AppScheduler](#appscheduler)
        - [Driver\_Stm](#driver_stm)
        - [AppScheduling](#appscheduling)
        - [ERUInterrupt](#eruinterrupt)
      - [Drivers](#drivers)
        - [gpt12](#gpt12)
        - [ASCLIN](#asclin)
      - [IO](#io)
        - [Bluetooth](#bluetooth)
        - [Buzzer](#buzzer)
        - [Motor](#motor)
        - [TOF Sensor](#tof-sensor)
        - [Ultrasonic](#ultrasonic)
    - [3. Main Code](#3-main-code)

<hr>

### 1. Directory Structure

![Alt text](Directory_Tree.png)

<hr>

### 2. Libraries

#### AppScheduler

##### Driver_Stm

- App_Stm structure
    - **stmsSfr** : STM 레지스터 베이스를 가리키는 포인터
    - **stmConfig** : STM 구성을 위한 구조체
    - **counter** : 인터럽트 카운터

- 전역 변수
    - **g_Stm**: App_Stm 구조체의 인스턴스
    - **u32nuCounter1ms**: 1밀리초마다 증가하는 카운터
    - **stSchedulingInfo**: 다양한 타이밍에서 태스크 스케줄링을 위한 플래그

- 함수
    - **Driver_Stm_Init()**: STM을 초기화하는 함수. 인터럽트를 일시적으로 비활성화한 후, STM 설정을 구성하고 다시 인터럽트를 활성화.
    - **STM_Int0Handler()**: STM 인터럽트 핸들러. STM 인터럽트 발생할 때 호출. 인터럽트 카운터를 증가시키고, 다양한 시간 간격(100ms, 500ms 등)에 따라 관련 플래그를 설정.   

##### AppScheduling
  - 주요 변수
    - **TestCnt 구조체**: 다양한 타이밍 카운터를 위한 uint32_t 타입의 변수들을 포함 (예: u32nuCnt1ms, u32nuCnt10ms 등).
    - **static function prototype** :  AppTask100ms() 및 AppTask500ms() 함수의 프로토타입을 정의.
    - **TestCnt 변수**: stTestCnt라는 TestCnt 구조체의 인스턴스를 선언.

  - 주요 함수
    - **AppScheduling()**: 100ms와 500ms 간격으로 실행되는 태스크를 스케줄링. 특정 플래그(u8nuScheduling100msFlag)가 설정되면 해당 태스크를 실행하고 플래그를 재설정.
    - **AppTask100ms() 및 AppTask500ms()**: LED1과 LED2의 상태를 제어. 

##### ERUInterrupt
  - 전역 변수 
    - **g_ERUconfig**: ERU 설정을 저장하는 구조체.
    - **INTERRUT_VAL**: 외부에서 사용될 수 있는 인터럽트 관련 변수.
  - 인터럽트 서비스 루틴(ISR)
    - **SCUERU_Int0_Handler**: 이 함수는 ERU 인터럽트가 발생할 때 실행.
  - 초기화 함수
    - **initPeripheralsAndERU()**: ERU 및 관련 주변 장치를 초기화하는 함수. 이 함수는 트리거 핀과 LED 핀의 모드, ERU의 입력 채널, 출력 채널, 트리거 조건을 설정.
  - 작동 방식
    - **ERU 초기화**: initPeripheralsAndERU 함수는 ERU 모듈을 초기화하고, 특정 핀(예: 버튼)에 대한 입력을 설정하여 인터럽트를 발생시키는 조건을 구성.
    - **인터럽트 핸들링**: SCUERU_Int0_Handler 함수는 ERU 인터럽트가 발생할 때 호출되어 INTERRUT_VAL 변수의 값을 변경. 이는 외부 버튼 입력 등에 반응하여 특정 작업을 수행.

<hr>

#### Drivers

##### gpt12
  - 매크로 설정
    - **ISR_PROVIDER_GPT12_TIMER, GPT1_BLOCK_PRESCALER** 등 타이머 설정에 관련된 매크로 정의.
    - **KP, KI, KD**: PID 제어를 위한 비례, 적분, 미분 계수.
    - 모터와 엔코더 핀 정의, 모터 듀티 사이클, 엔코더 값, 각속도 등을 위한 변수 선언.
  - 인터럽트 서비스 루틴(ISR)
    - **IsrGpt2T6Handler()**: 이 함수는 GPT12의 타이머 6(T6)에 의해 주기적으로 호출. 모터 PWM 제어 및 엔코더 업데이트 수행.
  - 초기화 함수
    - **init_gpt1, init_gpt2**: GPT12 모듈 및 타이머 T3, T4, T6 초기화.
    - **runGpt12_T3, stopGpt12_T3**: 각 타이머의 동작 제어.
  - PID 제어 및 엔코더 처리
    - **update_encoder()**: 엔코더 값을 읽어 각속도를 계산.
    - **motor_pid()**: 주어진 목표 각속도(w_ref)에 따라 모터의 속도를 조절하기 위한 PID 제어 알고리즘을 실행.
  - 작동 방식
    - **타이머 초기화 및 구동**: init_gpt1 및 init_gpt2 함수는 GPT12 모듈 초기화. runGpt12_T3, stopGpt12_T3 등의 함수는 타이머를 동작 제어.
    - **모터 PWM 제어**: IsrGpt2T6Handler ISR은 타이머 인터럽트에 의해 주기적으로 호출되며, 모터의 PWM 신호를 제어.
    - **엔코더 처리**: update_encoder 함수는 엔코더 값을 읽어 모터의 회전 각도와 속도를 계산.
    - **PID 제어**: motor_pid 함수는 주어진 목표 각속도에 따라 모터의 속도를 조절.

##### ASCLIN
- 매크로 및 변수 정의
  - Baud rate(ASC_BAUDRATE, TOF_BAUDRATE, BLUETOOTH_BAUDRATE)와 버퍼 크기(ASC_TX_BUFFER_SIZE, ASC_RX_BUFFER_SIZE) 정의
  - 세 개의 IfxAsclin_Asc 인스턴스(g_ascHandle0, g_ascHandle1, g_ascHandle3) 선언.
  - FIFO 버퍼 정의
- 인터럽트 서비스 루틴(ISR)
  - 각 ASC 인터페이스(g_ascHandle0, g_ascHandle1, g_ascHandle3)에 대한 송신, 수신 및 에러 처리를 위한 ISR(asclin3TxISR, asclin3RxISR, asclin3ErrISR 등) 구현.
- UART 초기화 함수
  - **_init_uart3, _init_uart1, _init_uart0**: 각각 ASC3, ASC1, ASC0 인터페이스 초기화. 해당 인터페이스의 baud rate, 핀 설정, 버퍼 크기 등을 설정.
- 데이터 송수신 및 폴링 함수
  - **_out_uart3, _in_uart3, _poll_uart3**: UART3를 통해 문자를 송수신하고, 데이터의 유무를 체크.
  - **_out_uart1, _in_uart1, _poll_uart1**: UART1에 대한 동일한 기능 수행.
  - **_out_uart0, _in_uart0, _poll_uart0, _nonBlock_poll_uart0**: UART0에 대한 동일한 기능을 수행.
- 작동 방식
    - **UART 초기화**: 각 UART 인터페이스는 해당하는 _init_uartX 함수 사용하여 초기화. 
    - **인터럽트 핸들링**: 각 UART 인터페이스는 자신의 ISR을 통해 송신 및 수신 데이터를 처리하고, 에러가 발생할 경우 에러 ISR 호출.
    - **데이터 송수신**: _out_uartX, _in_uartX 함수를 통해 각 UART 인터페이스에서 데이터 송수신. _poll_uartX 함수는 데이터의 유무를 체크하고, 데이터가 있는 경우 데이터 수신.

<hr>

#### IO

##### Bluetooth
- 설정 함수
  - **setBluetoothName(char *name)**: 블루투스 모듈의 이름 설정. AT 명령어 형식(AT+NAME)으로 이름 설정.
  - **setBluetoothPswd(char *pswd)**: 블루투스 모듈의 비밀번호 설정. AT 명령어 형식(AT+PIN)을 사용해 비밀번호 설정.
- 데이터 송수신 함수
  - **getBluetoothByte_Blocked()**: 동기 함수. 블루투스 모듈로부터 데이터를 수신.
  - **getBluetoothByte_nonBlocked()**: 비동기 함수. 블루투스 모듈로부터 데이터를 수신. 데이터가 없으면 -1을 반환.
  - **setBluetoothByte_Blocked(unsigned char ch)**: 동기 함수. 블루투스 모듈로 데이터 전송.
- 문자열 출력 함수
  - **bl_printf(const char *fmt, ...)**: printf 스타일의 포맷 문자열로 블루투스 모듈에 데이터 전송. 개행 문자(\n) 앞에 복귀 문자(\r) 추가, 블루투스 터미널 포맷으로 진행.

##### Buzzer

- 매크로 정의
  - **Buzzer**: 부저와 연결한 핀 정의.
  - **ISR_PROVIDER_GPT12_TIMER, GPT1_BLOCK_PRESCALER, TIMER_T3_INPUT_PRESCALER, GPT120_MODULE_FREQUENCY**: 타이머 모듈 설정 값.
- 전역 변수
  - **beepCnt**: 부저가 울린 횟수
  - **beepOnOff**: 부저의 울림/멈춤 주기 제어 값.
- 인터럽트 서비스 루틴(ISR)
  - **IsrGpt120T3Handler_Beep**: 타이머 인터럽트에 의해 주기적으로 호출, 부저의 상태 토글. beepCnt와 beepOnOff 값 기반으로 부저 동작 제어.
- 초기화 및 제어 함수
  - **Init_Buzzer()**: 부저 기초 설정. 핀 모드 설정, GPT12 타이머 초기화 및 실행 제어.
  - **setBeepCycle(int cycle)**: 부저의 울림 주기 설정. beepOnOff 값을 조정, 부저가 울리는 지속 시간 및 간격 제어.
  - **Beep(unsigned int hz)**: 지정된 주파수로 부저 활성화. 주파수에 따라 loop 변수를 계산, 지정된 시간 동안 지연 루프를 수행.

##### Motor

- 핀 정의
  - 모터 A와 B에 대한 방향(DIR_A, DIR_B), PWM(PWM_A, PWM_B), 제동(BREAK_A, BREAK_B) 핀 정의
- 초기화 함수
  - **Init_DCMotors()**: 모터 A와 B의 방향, PWM, 제동 핀을 출력 모드로 설정, 정지상태로 초기화. init_gpt2() 함수를 호출, 타이머 초기화.
- 모터 제어 함수
  - **movChA(int dir), movChB(int dir)**: 모터 A와 B 방향 설정, 모터를 구동. dir 매개변수에 따라 정방향 또는 역방향으로 회전.
  - **stopChA(), stopChB()**: 모터 A와 B를 정지. 제동 핀을 활성화, 모터를 정지 상태로 제어.
  - **movChA_PWM(int duty, int dir), movChB_PWM(int duty, int dir)**: PWM 신호를 사용, 모터 A와 B의 속도 제어 및 방향 설정. duty 매개변수는 PWM 듀티 사이클 결정, dir 매개변수는 회전 방향을 결정.

##### TOF Sensor

- 초기화 함수
  - **Init_ToF()**: uart1 초기화.
- 인터럽트 서비스 루틴(ISR)
  - **asclin1RxISR()**: uart1 수신 인터럽트 함수. ToF 센서로부터 데이터 수신, gBuf_tof 버퍼에 저장. 버퍼가 꽉 차면(TOF_length에 도달하면) 수신된 데이터를 gBuf_tof에 복사하고 인덱스를 0으로 초기화.
- 데이터 검증 및 처리 함수
  - **verifyCheckSum(unsigned char data[])**: 수신된 데이터의 체크섬을 계산하여 유효성을 검증.
  - **checkTofStrength(unsigned char data[])**: ToF 센서의 신호 강도와 거리 데이터를 확인해 측정이 유효한지 판단.
  - **getTofDistance()**: 검증된 ToF 데이터로부터 실제 거리를 계산해 반환. 데이터의 유효성을 검사하고, 유효한 경우 거리를 계산하여 반환.


##### Ultrasonic

- 초기화 함수
- **Init_Ultrasonics()**: 트리거(TRIG) 핀을 출력 모드, 에코(ECHO) 핀 입력 모드로 설정. init_gpt1() 함수로 타이머 초기화.
- 거리 측정 함수
- 각 초음파 센서에 대해 두 가지 유형의 거리 측정 함수 제공
  - **ReadRearUltrasonic_noFilt()** : 원시 거리 데이터 반환
  - **ReadLeftUltrasonic_Filt(), ReadRightUltrasonic_Filt()** : 이동 평균 필터를 적용한 거리 데이터를 반환.
- 트리거 핀을 활성화, 초음파 신호를 발생.
- 에코 핀에서 신호를 기다린 후, 타이머를 사용하여 신호의 지속 시간을 측정.
지속 시간을 기반 거리를 계산.
- 필터링
  - **이동 평균 필터** : 노이즈를 줄이고, 안정적인 거리 측정값을 제공.

<hr>

### 3. Main Code

- Pseudo Code

```
Program Start

    Enable Interrupt
    Disable CPU & safety watchdogs (re-enable & service periodically if needed)

    Wait for CPU synchronization event

    Initialize peripherals & ERU
    Initialize Mystdio, GPIO, Buzzer, ToF, DC Motors

    Turn off LED1 & LED2 & Buzzer

    Begin Main Loop

        Set mode(use Interrupt value)

        Measure distance using Tof module
        Adjust speed deceleration & beep cycle based on distance

            if distance >= 100 and distance <= 200:
                speed /= 2
                beep every 0.5 seconds

            if distance <= 100:
                stop movement
                beep every 0.5 seconds

        Toggle PID control On/Off based on mode
            if mode is OFF:
                Move motor at PWM speed, log current speed
            if mode is ON:
                Control motor speed using PID, log current speed

    - (When adding periodic functionalities, AppScheduling can be incorporated for their implementation.)

    End Main Loop
Program End
```

```
프로그램 시작

    인터럽트 활성화
    CPU 및 안전 워치독 비활성화 (필요시 다시 활성화 및 주기적으로 서비스)

    CPU 동기화 이벤트 대기

    주변 장치 및 ERU 초기화
    Mystdio, GPIO, 부저, ToF, DC 모터 초기화

    LED1 및 LED2, 부저 끄기

    메인 루프 시작

        모드 설정 (인터럽트 값을 사용)

        ToF 모듈을 사용하여 거리 측정
        거리에 따라 속도 감소 및 부저 주기 조절
        
            거리가 100 이상이고 200 이하일 경우:
                속도 /= 2
                0.5초마다 부저

            거리가 100 이하일 경우:
                움직임 정지
                0.5초마다 부저

        모드에 따라 PID 제어 On/Off 토글
            모드가 꺼져 있을 경우:
                PWM 속도로 모터 움직임, 현재 속도 로그 출력
            모드가 켜져 있을 경우:
                PID를 사용하여 모터 속도 제어, 현재 속도 로그 출력

    - (주기적 기능을 추가할 때, AppScheduling을 사용하여 구현 가능.)

    메인 루프 종료
프로그램 종료
```