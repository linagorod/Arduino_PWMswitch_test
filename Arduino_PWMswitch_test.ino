#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>

//сервы
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); //инициализация i2c для pca с адресом 0x40

/*----------------------------------------------------------------------------------------------------------*/
//НАСТРОЙКА КАНАЛОВ И КНОПОК
//подключение серв (выводы/каналы pca)
#define SERVO_HOLD_CH     7  //канал, уровень которого изменяется удержанием кнопки
#define SERVO_SWITCH_CH   6 //канал, уровень которого переключается по нажатию кнопки
//Обозначения кнопок смотреть в библиотеке PS2X_lib (в примере)
#define BUTTON_HOLD     PSB_TRIANGLE  //кнопка "удержания" высокого уровня на канале SERVO_HOLD_CH (треугольник)
#define BUTTON_SWITCH   PSB_CROSS //кнопка переключения уровня на канале SERVO_SWITCH_CH (крестик)
/*----------------------------------------------------------------------------------------------------------*/

#define SERVO_FREQ  60 //частота ШИМ (~57Гц)
#define SERVO_DELAY 3 //задержка для правильной работы

//Выводы джойстика
#define PS2_DAT        5
#define PS2_CMD        6
#define PS2_SEL        7
#define PS2_CLK        8

//Режимы работы джойстика (раскомментировать нужное)
//- pressures = аналоговое считывание нажатия кнопок
//- rumble    = вибромоторы
#define pressures   true
//#define pressures   false
//#define rumble      true
#define rumble      false

char button_state = 0;

//Создание класса для джойстика
PS2X ps2x;
int error = 0;
byte type = 0;
byte vibrate = 0;

void setup()
{
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);  // Установка частоты ШИМ (здесь значения не играет)

  //установка выводов и настроек: GamePad(clock, command, attention, data, Pressures?, Rumble?) проверка ошибок
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
}

void loop()
{
  //Опрос джойстика
  ps2x.read_gamepad(false, vibrate); //считывание данных с джойстика и установка скорости вибрации


  //если нажата (и удерживается) кнопка BUTTON_HOLD
  if (ps2x.Button(BUTTON_HOLD))
  {
    pwm.setPWM(SERVO_HOLD_CH, 4096, 0); //высокий уровень
  }
  else pwm.setPWM(SERVO_HOLD_CH, 0, 4096); //низкий уровень, если кнопка отжата


  //если была нажата кнопка BUTTON_SWITCH
  if (ps2x.ButtonPressed(BUTTON_SWITCH))
  {
    switch (button_state) //смотрим, который раз была нажата кнопка
    {
      case 0: //первый раз -> включение канала
        {
          pwm.setPWM(SERVO_SWITCH_CH, 4096, 0); //высокий уровень
          button_state = 1;
          break;
        }

      case 1: //второй раз -> выключение канала
        {
          pwm.setPWM(SERVO_SWITCH_CH, 0, 4096); //низкий уровень
          button_state = 0;
          break;
        }
    }
  }
}
