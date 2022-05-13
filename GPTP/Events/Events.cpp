#include "Events.h"

#ifdef EVENTS_SYSTEM

EventManager eventManager;

void EventManager::RegisterEvent(int eventId, func_pointer function_to_call) {
	eventManager.registered_events[eventId].push_back(function_to_call);
}

;

void EventManager::RegisterEventNamed(int eventId, func_pointer function_to_call, char* event_name) {
	eventManager.registered_events[eventId].push_back(function_to_call);
	eventManager.named_events[function_to_call] = event_name;
}

;

void EventManager::UnregisterEvent(int eventId, func_pointer function_to_call) {

	eventManager.registered_events[eventId].remove(function_to_call);

	std::map<func_pointer, char*>::iterator names_iterator = eventManager.named_events.find(function_to_call);

	if (names_iterator != eventManager.named_events.end())
		eventManager.named_events.erase(names_iterator);

}

;

bool EventManager::EventExists(int eventId) {

	if (eventId == EventId::ANY_EVENT)
		return !eventManager.registered_events.empty();
	else
	{
		eventManager.registration_iterator = eventManager.registered_events.find(eventId);

		if (eventManager.registration_iterator != eventManager.registered_events.end())
			return true;
		else
			return false;
	}

}

;

bool EventManager::EventExists(int eventId, func_pointer function_to_call) {

	bool return_value = false;

	eventManager.registration_iterator = eventManager.registered_events.find(eventId);

	if (eventManager.registration_iterator != eventManager.registered_events.end()) {

		std::list<func_pointer>::iterator list_iterator =
			std::find(
				eventManager.registration_iterator->second.begin(),
				eventManager.registration_iterator->second.end(),
				function_to_call
			);

		return_value = (list_iterator != eventManager.registration_iterator->second.end());

	}

	return return_value;

}

;

bool EventManager::EventExists(int eventId, char* event_name) {

	bool return_value = false;

	eventManager.registration_iterator = eventManager.registered_events.find(eventId);

	if (eventManager.registration_iterator != eventManager.registered_events.end()) {

		std::list<func_pointer>::iterator list_iterator = eventManager.registration_iterator->second.begin();
		std::list<func_pointer>::iterator list_iterator_end = eventManager.registration_iterator->second.end();
		std::map<func_pointer, char*>::iterator names_iterator;
		std::map<func_pointer, char*>::iterator names_iterator_end = eventManager.named_events.end();

		while (list_iterator != list_iterator_end && !return_value) {

			names_iterator = eventManager.named_events.find(*list_iterator);

			if (names_iterator != names_iterator_end) {

				if (strcmp(names_iterator->second, event_name) == 0) {
					return_value = true;
				}

			}

		}

	}

	return return_value;
	
}

;

void EventManager::ClearEvents(int eventId) {
		
	eventManager.registration_iterator = eventManager.registered_events.find(eventId);

	if (eventManager.registration_iterator != eventManager.registered_events.end()) {


		std::list<func_pointer>::iterator list_iterator = eventManager.registration_iterator->second.begin();
		std::list<func_pointer>::iterator list_iterator_end = eventManager.registration_iterator->second.end();

		while (list_iterator != list_iterator_end) {

			std::map<func_pointer, char*>::iterator named_func_iterator = eventManager.named_events.find(*list_iterator);

			if (named_func_iterator != eventManager.named_events.end())
				eventManager.named_events.erase(named_func_iterator);

			list_iterator++;

		}

		eventManager.registration_iterator->second.clear();
		eventManager.registered_events.erase(eventManager.registration_iterator);

	}

}

;

void EventManager::EventCalled(int eventId, std::vector<int*> parameters) {

	eventManager.registration_iterator = eventManager.registered_events.find(eventId);

	if (eventManager.registration_iterator != eventManager.registered_events.end() && !eventManager.registration_iterator->second.empty()) {

		std::list<func_pointer>::iterator list_iterator = eventManager.registration_iterator->second.begin();
		std::list<func_pointer>::iterator last_pos = eventManager.registration_iterator->second.end();

		while (list_iterator != last_pos) {
			(*list_iterator)(parameters);
			list_iterator++;
		}

	}

}

;

void EventManager::EventCalling(int eventId, bool* bCancelNormalBehavior, std::vector<int*> parameters) {

	*bCancelNormalBehavior = false;
	eventManager.registration_iterator = eventManager.registered_events.find(eventId);

	if (eventManager.registration_iterator != eventManager.registered_events.end() && !eventManager.registration_iterator->second.empty()) {

		std::list<func_pointer>::iterator list_iterator = eventManager.registration_iterator->second.begin();
		std::list<func_pointer>::iterator last_pos = eventManager.registration_iterator->second.end();

		while (list_iterator != last_pos) {

			if ((*list_iterator)(parameters) && !*bCancelNormalBehavior) //always execute event function, but ignore the cancel result if cancel already true
				*bCancelNormalBehavior = true;

			list_iterator++;

		}

	}

}

;

#endif