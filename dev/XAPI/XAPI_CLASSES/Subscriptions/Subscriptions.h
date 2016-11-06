
#ifndef SUBSCRIPTIONS_h
#define SUBSCRIPTIONS_h
#include "Arduino.h"
#include "Universal.h"

//**********************************************
//**********************************************
// This class simply keeps track of of what
// TUN ID's the service wants to be notified of.

class Subscriptions
{
	// for all callers
	public:
		void set_subscriptions(uint8_t* type_array, uint8_t items);
		boolean add_subscription(uint8_t type);
		boolean is_subscribed(uint8_t type);
		
	// Constructors
	public:
		Subscriptions();
	
	// private functions
	private:
		void reset_subscriptions();
	
	// state variables
	private:
		uint8_t m_subscriptions[SUB_MAX_SUBSCRIPTIONS];
		uint8_t m_total_subscriptions;
	 
};

#endif