#ifndef KYDAS_DRIVER_H
#define KYDAS_DRIVER_H

#include "kydas_driver/rs232.h"

#include "ros/ros.h"


#include "kydas_driver/DisableMotor.h"
#include "kydas_driver/EnableMotor.h"
#include "kydas_driver/SetPosition.h"
#include "kydas_driver/SetSpeed.h"
#include "kydas_driver/SetTorque.h"

#include "kydas_driver/RequestQueryData.h"

#include "kydas_driver/MotorControllerStatus.h"
#include "kydas_driver/MotorCurrent.h"
#include "kydas_driver/MotorEletricAngle.h"
#include "kydas_driver/MotorFaultCode.h"
#include "kydas_driver/MotorPosition.h"
#include "kydas_driver/MotorProgramVersion.h"
#include "kydas_driver/MotorRotorPosition.h"
#include "kydas_driver/MotorSpeed.h"
#include "kydas_driver/MotorVoltage.h"

#include "sensor_msgs/Temperature.h"

#include <sstream>
#include <string>

const unsigned char CONTROL_HEADER = 0xE0;
const unsigned char QUERY_HEADER = 0xED;
const unsigned char HEARTBEAT_HEADER = 0xEE;

const char DEBUGGER_NAME_DATA_PREVIEW []= "data_preview";
const char DEBUGGER_NAME_MESSAGE_RECEIVED [] = "message_received";
const char DEBUGGER_NAME_MESSAGE_SENT [] = "message_sent";
const char DEBUGGER_NAME_COMMAND_SENT [] = "command_sent";

const int BUFFER_SIZE = 4096;

enum class Control_Data{
  SpeedMode = 1,
  TorqueMode,
  PositionMode
};

enum class Query_Data{
  ControlStatus = 0,
  EletricalAngle,
  Speed,
  Current,
  RotorPosition,
  Voltage,
  Temperature,
  FaultCode,
  Position,
  ProgramVersion,
  MAX_AMOUNT
};

/*Serao apenas necessarios caso queiramos fazer esse codigo fazer a analise do controller status

enum class ControlStatus_ControlMode{
  Analog = 1,
  CAN,
  RS232,
  RC
};

enum class ControlStatus_Feedback{
  Encoder = 1,
  Hall,
  AS5147,
  RotaryTransformer,
  Autosteering = 6,
  HallPlusEncoder,
  HallCloseLoop,
  HallPlusEncoderCloseLoop,
  Test,
  AbsoluteValueEncoder
};

enum class ControlStatus_WorkingMode{
  Speed = 1,
  Torque,
  AbsolutePosition,
  RelativePosition
};
*/
void displayMessage(const unsigned char* bytes, int n, const char * debugName);
void displayFaultCode(short faultCode);

class KydasDriverNode{
  public:

    KydasDriverNode();
    ~KydasDriverNode();

    void update();
    int openComport();

    int loop_rate;

  private:
    //Dados para comunicacao serial
    int m_cport_nr;
    int m_bdrate;
    std::string m_mode;
    //Buffers e outros para receber dados do serial
    unsigned char * m_buf;
    int m_bufferMaxSize;
    int m_positionInBuf;
    int m_bufSize;
    unsigned char m_currentHeaderBeingRead;

    //Dados a serem enviados pela serial------
    bool m_isConnected; //Indica se o driver foi conectado
    bool m_isEnabled; //Indica se o motor está ativado
    int m_setSpeed;
    int m_currentSpeed;
    int m_setTorque;
    int m_currentTorque;
    int m_setPosition;
    int m_currentPosition;
    //----------------------------------------

    //Para enviar as informacoes ao driver ---
    int m_oldSetValue;
    unsigned char m_oldSetWorkingMode;
    unsigned char m_setWorkingMode;
    //----------------------------------------

    //Dados do driver ------------------------
    int m_current;
    int m_rotorPosition;
    int m_voltage;
    int m_temperature;
    short m_faultCode;
    
    int m_programVersion;

    int m_eletricalAngle;

    unsigned char m_controlMode;
    unsigned char m_feedbackWay;
    unsigned char m_currentWorkingMode;
    //----------------------------------------
    //ROS-------------------------------------
    ros::NodeHandle m_nh;
    
    //Loop de enviar dados ao motor com delay
    ros::Timer m_setValueTimer;
    float m_sendDelay;
    void sendMotorCommandLoopCallback(const ros::TimerEvent&);

    //Publicadores
    ros::Publisher m_controllerStatus_pub;
    ros::Publisher m_current_pub;
    ros::Publisher m_eletricAngle_pub;
    ros::Publisher m_faultCode_pub;
    ros::Publisher m_position_pub;
    ros::Publisher m_programVersion_pub;
    ros::Publisher m_rotorPosition_pub;
    ros::Publisher m_speed_pub;
    ros::Publisher m_temp_pub;
    ros::Publisher m_voltage_pub;

    //Servicos
    ros::ServiceServer m_enableMotorService;
    ros::ServiceServer m_disableMotorService;
    ros::ServiceServer m_setPositionService;
    ros::ServiceServer m_setSpeedService;
    ros::ServiceServer m_setTorqueService;
    ros::ServiceServer m_requestQueryDataService;

    //Funcoes gerais de ler serial
    void readSerial();
    void readMessagesOnBuffer();
    //Funcoes para enviar comandos
    void sendMotorCommand();
    int setMotorCommand(int value, unsigned char controlMode);
    int enableMotor();
    int disableMotor();
    int requestQueryData(unsigned char command);
    //Funcoes para receber dados
    int readQueryData(unsigned char* bytes, int currentPosition);
    int readHeartbeatData(unsigned char* bytes, int currentPosition);

    //Callbacks dos servicos
    bool enableMotor(kydas_driver::EnableMotor::Request  &req, kydas_driver::EnableMotor::Response &res);
    bool disableMotor(kydas_driver::DisableMotor::Request  &req, kydas_driver::DisableMotor::Response &res);
    bool setSpeed(kydas_driver::SetSpeed::Request  &req, kydas_driver::SetSpeed::Response &res);
    bool setTorque(kydas_driver::SetTorque::Request  &req, kydas_driver::SetTorque::Response &res);
    bool setPosition(kydas_driver::SetPosition::Request  &req, kydas_driver::SetPosition::Response &res);
    bool requestQueryData(kydas_driver::RequestQueryData::Request  &req, kydas_driver::RequestQueryData::Response &res);  
};

#endif