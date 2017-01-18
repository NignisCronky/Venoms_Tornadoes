#include "Camera.h"

Camera::Camera()
{
}

void Camera::Create(XMFLOAT4X4 cam)
{
	m_camera = cam;
	m_timer.Reset();
	delta_time = 0.0f;
	Keycooldown = Changecooldown = 0.0f;
	wireFram = change = false;
}

void Camera::UpdateCamera(float const moveSpd, float const rotSpd)
{
	delta_time= m_timer.GetElapsedTime();
	Keycooldown -= delta_time;
	Changecooldown -= delta_time;
	m_timer.Reset();
	//W
	if (GetAsyncKeyState(0x57))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//S
	if (GetAsyncKeyState(0x53))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//A
	if (GetAsyncKeyState(0x41))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//D
	if (GetAsyncKeyState(0x44))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//X
	if (GetAsyncKeyState(0x58))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//Space
	if (GetAsyncKeyState(0x20))
	{
		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, -moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	//Right Mouse Button
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		POINT mousePos;
		//SetCursorPos(width / 2, height / 2);
		GetCursorPos(&mousePos);
		float dx = (float)width / 2 - mousePos.x;
		float dy = (float)height / 2 - mousePos.y;

		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);

		m_camera._41 = m_camera._42 = m_camera._43 = 0.0f;

		XMMATRIX rotX = DirectX::XMMatrixRotationX(dy * rotSpd * delta_time);
		XMMATRIX rotY = DirectX::XMMatrixRotationY(dx * rotSpd * delta_time);

		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		temp_camera = XMMatrixMultiply(rotX, temp_camera);
		temp_camera = XMMatrixMultiply(temp_camera, rotY);

		XMStoreFloat4x4(&m_camera, temp_camera);

		m_camera._41 = pos.x;
		m_camera._42 = pos.y;
		m_camera._43 = pos.z;
	}
	//Right key
	if (Keycooldown < 0.0f && (GetAsyncKeyState(VK_RIGHT) || change))
	{
		keyframe++;
		Keycooldown = 1.0f/30.0f;
	}
	//Left key
	if (Changecooldown < 0.0f && GetAsyncKeyState(VK_LEFT))
	{
		change = !change;
		Changecooldown = 3.0f / 30.0f;
	}
	//1 Key
	if (GetAsyncKeyState(0x31) & 1)
	{
		wireFram = !wireFram;
	}
}
