#pragma once

#include "JsonParsing.h"
#include "SDL.h"

class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	void Start();
	void FinishUpdate();

	inline void SetDesiredCappedMs(int miliseconds) { cappedMs = miliseconds; }
	inline void SetDesiredDeltaTime(float dt) { deltaTime = dt; }

	int GetEngineTimeStartup() const;
	int GetTime() const;
	inline int GetFramesSinceStartup() const { return frameCounter; }
	inline float GetDeltaTime() const { return deltaTime; }
	inline int GetFps() const { return (1.0f / ((float)cappedMs) * 1000.0f); }

	float ReadSec()
	{
		if (gameStarted == true)
		{
			timer = float(SDL_GetTicks() - started_at) / 1000.0f;
			return timer;
		}
		else
		{
			timer = float(stopped_at - started_at) / 1000.0f;
			return timer;
		}
	}

	void ResetTimer();

	void ReadConfig(JsonParsing& node);
	void SaveConfig(JsonParsing& node);
private:
	// Normal timings
	int timer;
	float deltaTime;
	int frameCounter;
	int lastFrameMs;
	int cappedMs;

	// Engine timings
	int gameTimer;
	bool gameStarted;
	Uint32	started_at;
	Uint32	stopped_at;

	// Json Parsing for loading and saving things
	JsonParsing jsonFile;
};