
#ifndef SUBSCRIPTIONS_cpp
#define SUBSCRIPTIONS_cpp
#include "Subscriptions.h"

//***************************************************
//***************************************************
// Subscriptions allow a service to subscribe to a certain
// TUN packet IDs. 

//***************************************************
//***************************************************
// Returns TRUE if got subscription to type, and
// FALSE if there is no subscription to type
boolean Subscriptions::is_subscribed(uint8_t type)
{
	boolean got_sub = false;

	// search to see if we have a subscription to type
	for(uint8_t i = 0; i < m_total_subscriptions; i++)
	{
		if( type == m_subscriptions[i])
			got_sub = true;
	}
	
	return got_sub;
}

//***************************************************
//***************************************************
// Adds subscriptions
// returns TRUE or FALSE depending on if
// a subscription was added
boolean Subscriptions::add_subscription(uint8_t type)
{
	boolean added = false;

	// ensure we are not filled of subscriptions
	if( m_total_subscriptions < SUB_MAX_SUBSCRIPTIONS)
	{
		m_subscriptions[m_total_subscriptions++] = type;
		added = true;
	}
	return added;
}

//***************************************************
//***************************************************
// Sets the subscriptions
void Subscriptions::set_subscriptions(uint8_t* type_array, uint8_t items)
{
		
	// copy over the subscriptions
	for(uint8_t i =0; i < items; i++)
		 add_subscription(type_array[i]);
}

//***************************************************
//***************************************************
// Resets the subscription numbers
void Subscriptions::reset_subscriptions()
{
	for(uint8_t i = 0; i < SUB_MAX_SUBSCRIPTIONS; i++)
		m_subscriptions[i] = SENT_NULL_TUN_TYPE;
	
	m_total_subscriptions = 0;
}

//***************************************************
//***************************************************
Subscriptions::Subscriptions()
{
	reset_subscriptions();
}



#endif