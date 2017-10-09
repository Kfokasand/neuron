#include <iostream>
#include <cmath>
#include "neuron.hpp"

using namespace std;

const double H = 0.1;

//unsigned int Neuron::NbStepsequalDelay = 15; // Delay = 1.5ms

const double Neuron::THO_ = 20;
const double Neuron::C_ = 1;
const double Neuron::SPIKE_THRESHOLD_ = 20;
const double Neuron::V_RESET_ = 10; 
const double Neuron::REFRACT_TIME_ = 2.0;
const double Neuron::R_= THO_/C_;
const double Neuron::EXP1_ = exp(-H/THO_);

Neuron::Neuron(double memb_pot, unsigned int local_clock)
: memb_pot_(memb_pot), local_clock_(local_clock)
{
	//array<double, NbStepsequalDelay> buffer_spikes_;
}

double Neuron::getMemPot() const
{
	return memb_pot_;
}

unsigned int Neuron::getNbSpike() const
{
	return spikes_historic_.size();
}

double Neuron::getLastSpike() const
{
	if (!spikes_historic_.empty()) {
		return spikes_historic_.back();
	} else {
		return -10; //by convention
	}
}

void Neuron::setMemPot(double pot)
{
	memb_pot_=pot;
}

void Neuron::addSpike(double time)
{
	spikes_historic_.push_back(time);
}

void Neuron::addTarget(Neuron* p_neuron)
{
	targets_list_.push_back(p_neuron);
}


void Neuron::addArrivingSpike(unsigned int firing_local_clock, double J)
{
		buffer_spikes_.at((firing_local_clock - local_clock_ + 15 - 1) % buffer_spikes_.size()) += J;
		//si firing est en retard, indice = D-1
		//si firing = receiver, indice = D
		//firing est incrémenté seulement après, impossible que firing > receiver
		// le -1 viebnt de l'implémentation des array, premier indice étant 0
}


void Neuron::update(unsigned int nbStep, double I_ext)
{
	if ((!spikes_historic_.empty()) && (local_clock_ < getLastSpike() + REFRACT_TIME_)) {
		//neuron is insensitive to stimulation during refract time, membrane potential 
		//doesn't change
	} else if (getMemPot() > SPIKE_THRESHOLD_) {
		addSpike(local_clock_*H);
		//neuron reached the spike, the value of the membrane potential is already
		//stored (value of the last step), it has now to be reset
		
		for (auto& tar_neuron : targets_list_)
		{
			tar_neuron->addArrivingSpike(local_clock_, getMemPot()); //
			/*éventuellement multiplier getMemPOt()
			par le facteur efficacy entre les deux neurones, cf les informations de 
			connected_neurons
			*/
		}

		setMemPot(V_RESET_);
	} else {
		setMemPot(EXP1_*memb_pot_ + I_ext*R_*(1-EXP1_) 
		//	+ buffer_spikes_.at(local_clock_ % buffer_spikes_.size())
			+ buffer_spikes_.at(local_clock_ % buffer_spikes_.size())
		);
	}
	local_clock_+= nbStep*H;
}