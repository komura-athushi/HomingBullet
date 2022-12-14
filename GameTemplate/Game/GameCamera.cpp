#include "stdafx.h"
#include "GameCamera.h"

#include "Player.h"

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	m_player = FindGO<Player>("player");

	//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(0.0f, 100.0f, -200.0f);

	return true;
}

void GameCamera::Update()
{
	if (m_player->IsLockOn() == false)
	{
		ProcessNormal();
	}
	else
	{
		ProcessLockOn();
	}
}

void GameCamera::ProcessNormal()
{
	//カメラを更新。
	//注視点を計算する。
	Vector3 target = m_player->GetPosition();
	//プレイヤの足元からちょっと上を注視点とする。
	target.y += 80.0f;
	target += g_camera3D->GetForward() * 20.0f;

	Vector3 toCameraPosOld = m_toCameraPos;
	//パッドの入力を使ってカメラを回す。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y軸周りの回転
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 1.5f * x);
	qRot.Apply(m_toCameraPos);
	//X軸周りの回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 1.5f * y);
	qRot.Apply(m_toCameraPos);
	//カメラの回転の上限をチェックする。
	//注視点から視点までのベクトルを正規化する。
	//正規化すると、ベクトルの大きさが１になる。
	//大きさが１になるということは、ベクトルから強さがなくなり、方向のみの情報となるということ。
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//カメラが上向きすぎ。
		m_toCameraPos = toCameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//カメラが下向きすぎ。
		m_toCameraPos = toCameraPosOld;
	}

	//視点を計算する。
	Vector3 pos = target + m_toCameraPos;

	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(target);
}

void GameCamera::ProcessLockOn()
{
	Vector3 target= m_player->GetTargetPosition();
	Vector3 toPos = target - m_player->GetPosition();
	float r = toPos.Length() + 250.0f;
	toPos.y = 0.0f;
	toPos.Normalize();
	Vector3 position = target - toPos * r;
	position.y = m_player->GetPosition().y + 200.0f;

	g_camera3D->SetPosition(position);
	g_camera3D->SetTarget(target);
}