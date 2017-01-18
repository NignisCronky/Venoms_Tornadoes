#pragma once
#include "Globals.h"

struct Camera
{
	XMFLOAT4X4 m_camera;
	Timer m_timer;
	float delta_time;
	float cooldown;
	long long keyframe;
	bool wireFram;

	Camera();
	void Create(XMFLOAT4X4 cam);
	void UpdateCamera(float const moveSpd, float const rotSpd, float delta_time = 1.0f);
	float* Camera::GetDelta() { return &delta_time; }
	long long* Camera::GetKeyframe() { return &keyframe; }
	bool* Camera::GetWireframe() { return &wireFram; }
	XMFLOAT4X4* Camera::GetCamera() { return &m_camera; }
};