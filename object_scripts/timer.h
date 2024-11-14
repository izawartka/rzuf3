#pragma once
#include "../events/update.h"
#include "../event_macros.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UpdateEvent;

struct RZUF3_TimerOptions {
	std::string id;
	double interval;
	bool dispatchToScene = false;
	bool enabled = true;
};

class RZUF3_Timer : public RZUF3_ObjectScript {
public:
	RZUF3_Timer(std::string id, double interval);
	RZUF3_Timer(RZUF3_TimerOptions options);
	~RZUF3_Timer();

	void init();
	void deinit();

	void setInterval(double interval);
	void setEnabled(bool enabled);
	void setTime(double time);

	double getInterval() const { return m_options.interval; }
	bool isEnabled() const { return m_options.enabled; }
	double getTime() const { return m_time; }

protected:
	void onUpdate(RZUF3_UpdateEvent* event);

	RZUF3_TimerOptions mp_options;

	RZUF3_TimerOptions m_options;
	double m_time = 0.0;
	RZUF3_EventsManager* m_selEventsManager = nullptr;

	_DECLARE_LISTENER(Update)
};