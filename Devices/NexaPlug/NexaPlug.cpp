#include "NexaPlug.hh"
#include "nexa.h"
#include "radio.h"
#include "../../Helpfunctions/helpfunctions.hh"

#include <iostream>
#include <stdlib.h>

static const uint8_t brand_ = 0x01;

NexaPlug::NexaPlug(unsigned int id): 
			id_(id), 
			state_(0),
			channel_(0), 
			group_(0), 
			dim_level_(0x10),
			repeats_(5)
{
	std::cout << "Luotiin NexaPlug idlla: " << id_ <<"\n";
	deviceStatus = auxilo::deviceState::unknown;
}

NexaPlug::~NexaPlug()
{

}

 // ./pihat --repeats=50 --id=1 --channel=0 --state=1
 // ./pihat --id=1 --channel=0 --state=0

void NexaPlug::sendPacket(uint8_t state, uint8_t  repeats)
{
	initRF();
	uint64_t data = 0;
	state_ = state;
	/* Fill the data-variable and call the TX-function */
	data |= ((uint64_t)id_) << 10;
	data |= ((uint64_t)group_) << 9;
	data |= ((uint64_t)state_) << 8;
	data |= channel_ << 4;

	if (dim_level_ != 0x10)
	{
		data |= dim_level_;	
	} 

	nexaTxPacket(&data, (dim_level_ != 0x10), repeats); //FIXME
}

void NexaPlug::socketOn()
{
	sendPacket(1);
	deviceStatus = auxilo::deviceState::on;
	std::cout << "nexa " << id_  << " socketOn "<<"\n";
}

void NexaPlug::socketOff()
{
	sendPacket(0);
	deviceStatus = auxilo::deviceState::off;
	std::cout << "nexa " << id_  << " socketOFF "<<"\n";
}

bool NexaPlug::isSocketOn()
{
	if(state_ == 0)
	{
		return false;
	}

	return true;
}

void NexaPlug::pair()
{
    sendPacket(1,50);
}

void NexaPlug::unpair()
{
    sendPacket(0,50);
}


void NexaPlug::initRF()
{
	setup_io();
	setup_fm();
	ACCESS(CM_GP0DIV, getAllof()) = (0x5a << 24) + 0x374F; // Tune to 144.64 MHz to get the third harmonic at 433.92 MHz
}

void NexaPlug::setStatus(auxilo::deviceState status)
{
	switch (status)
	{
		case auxilo::deviceState::on:
		socketOn();
		break;

		case auxilo::deviceState::off:
		socketOff();
		break;

		default:
		break;
	}
}
auxilo::deviceState NexaPlug::getStatus() const
{
	return deviceStatus;
}

std::string NexaPlug::getID() const
{
	std::cout << "NexaPlug::getID(): " << "NECAAAAAAA " + Help::intToStr(id_) <<"\n";
	return "nexa " + Help::intToStr(id_);
}

void NexaPlug::initialize()
{
	//Nothing to do here really...
}