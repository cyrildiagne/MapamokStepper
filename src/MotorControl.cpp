//
//  MotorControl.cpp
//  vase
//
//  Created by kikko on 11/17/12.
//
//

#include "MotorControl.h"

MotorControl::MotorControl() {
    bAvailable = bExiting = bExitOk = false;
    rotation = 0;
}

void MotorControl::setup() {
    bAvailable = serial.setup("/dev/tty.usbmodemfd121");
    ofAddListener(serial.SerialLinkEvent, this, &MotorControl::serialLinkEventHandler);
}

void MotorControl::exit()
{
    if (!bAvailable) return;
    
    setSpeed(0);
    bExiting = true;
    
    while(!bExitOk) {
        update();
        ofSleepMillis(50);
    }
}

void MotorControl::setupControlTab() {
    
    panel.addPanel("Motor control");
    
    guiBaseObject *item = panel.addSlider("motor_speed", 0, 0, 100);
    ofAddListener(item->guiEvent, this, &MotorControl::panelValueChanged);
    
    panel.addSlider("motor_amp", 1., 0., 10.);
}

void MotorControl::update() {
    
    if(!bAvailable) return;
    
    serial.addCommand("R", "");
    
    serial.update();
    
    if(bExiting) {
        if (serial.getCmdBuffer().size() == 0) {
            ofLog() << "Exit ok.";
            bExitOk = true;
        }
    }
}

void MotorControl::setSpeed(int speed) {
    serial.addCommand("S", ofToString(speed));
}

void MotorControl::reset() {
    serial.addCommand("0", "");
}

void MotorControl::panelValueChanged(guiCallbackData& data) {
    
    string name = data.getXmlName();
    if(name=="motor_speed") setSpeed( getf("motor_speed") );
}

void MotorControl::serialLinkEventHandler(SerialLinkEventArgs& e)
{
    switch (e.event) {
        
        case VAL_CHANGED: {
            rotation = atoi(e.args[0].c_str());
        } break;
            
        default:
            break;
    }
}