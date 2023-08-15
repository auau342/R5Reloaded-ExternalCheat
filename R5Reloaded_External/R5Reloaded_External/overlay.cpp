#include "overlay.h"
#include "ImGui\customed.h"
#include "Utils\NotSDK.h"
#include <string>

// Aim
bool AimBot = true;
bool AimAtTeam = false;
// Options
bool AimVisCheck = true;
bool AimNoSway = true;
bool DrawFov = true;
float AimFov = 150.f;
float AimSmooth = 1.f;
int Bone = 1;
int TargetBone = 3;
int PredictValue = 350;
// FOV Colors
ImColor AimFovColor = { 1.f, 1.f, 1.f, 1.f };
ImColor AimUserColor = { 1.f, 1.f, 1.f, 1.f };

// ESP
bool DummyESP = false;
bool TeamESP = false;
// Options
bool ESP_Box = true;
int BoxType = 1;
bool ESP_Line = false;
bool ESP_Distance = true;
bool ESP_HealthBar = true;
// Colors
ImColor vColor_Normal = { 1.f, 0.f, 0.0f, 1.f };
ImColor vColor_Visible = { 0.f, 1.f, 0.f, 1.f };
ImColor vColor_Team = { 0.f, 1.f, 1.f, 1.f };

// 共通
uint64_t TargetEntity = 0;
float MaxDistance = 500.f;

// Menu
const char* BoxList[] = { "2D Box", "2D Corner Box" };
const char* BoneList[] = { "Head", "Chest" };

// その他
int AimKey0 = VK_RBUTTON;
int AimKeu1 = VK_LBUTTON;
float vPlayerInfo[75] = {};

void Overlay::m_Info()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)GameSize.right, (float)GameSize.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::Text("unknowncheats.me R5R-External [%.1f FPS]", ImGui::GetIO().Framerate);

    // Time
    time_t t = time(nullptr);
    struct tm nw;
    errno_t nTime = localtime_s(&nw, &t);
    ImGui::Text("%d:%d:%d", nw.tm_hour, nw.tm_min, nw.tm_sec);

    if (AimBot && DrawFov)
    {
        AimFovColor = AimUserColor;
        Circle((float)GameSize.right, (float)GameSize.bottom, AimFov, AimFovColor, 1.f);
    }

    switch (Bone)
    {
    case 0:
        TargetBone = 8;
        break;
    case 1:
        TargetBone = 3;
        break;
    default:
        break;
    }

    ImGui::End();
}

void Overlay::m_Menu()
{
    ImGui::SetNextWindowBgAlpha(0.95f);
    ImGui::Begin("unknowncheats.me - R5R-External", &ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    // Select Menu
    static int menu = 0;

    if (ImGui::Button("Aim", ImVec2(224.f, 45.f)))
        menu = 0;
    ImGui::SameLine();
    if (ImGui::Button("Visual", ImVec2(224.f, 45.f)))
        menu = 1;

    // Menu Start
    switch (menu)
    {
    case 0:
        // Aim MainTab
        ImGui::Toggle("AimBot", &AimBot, AimBot);
        ImGui::Toggle("Aim at Team", &AimAtTeam, AimAtTeam);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Checkbox("Visibility Check", &AimVisCheck);
        ImGui::Checkbox("NoSway", &AimNoSway);
        ImGui::Checkbox("Show AimFOV", &DrawFov);
        ImGui::NewLine();
        ImGui::SliderFloat("FOV", &AimFov, 25.f, 500.f);
        ImGui::SliderFloat("Smooth", &AimSmooth, 1.f, 50.f);
        ImGui::SliderFloat("Distance", &MaxDistance, 15.f, 1000.f);
        ImGui::SliderInt("Prediction", &PredictValue, 200, 600); // m_latestPrimaryWeaponsが取得不可能な為、このような方法でBulletSpeedをユーザーに調整させる
        ImGui::Combo("TargetBone", &Bone, BoneList, IM_ARRAYSIZE(BoneList));
        ImGui::NewLine();
        ImGui::ColorEdit4("FOV Color", &AimUserColor.Value.x);

        break;
    case 1:
        // Visual MainTab
        ImGui::Toggle("Player ESP", &ESP, ESP);
        ImGui::Toggle("Team ESP", &TeamESP, TeamESP);

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine();

        ImGui::Checkbox("Box", &ESP_Box);
        ImGui::Checkbox("Line", &ESP_Line);
        ImGui::Checkbox("Distance", &ESP_Distance);
        ImGui::Checkbox("HealthBar (HP + Shield)", &ESP_HealthBar);
        ImGui::SliderFloat("Distance (m)", &MaxDistance, 25.f, 1000.f);
        ImGui::Combo("Box Style", &BoxType, BoxList, IM_ARRAYSIZE(BoxList));
        ImGui::NewLine();
        ImGui::ColorEdit4("Normal ", &vColor_Normal.Value.x);
        ImGui::ColorEdit4("Visible", &vColor_Visible.Value.x);
        ImGui::ColorEdit4("Team", &vColor_Team.Value.x);
        break;
    default:
        break;
    }

    ImGui::End();
}

void Overlay::m_ESP()
{
    // ImGui Window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)GameSize.right, (float)GameSize.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // LocalPlayer
    CPlayer LPlayer, *pLocal = &LPlayer;
    uint64_t LocalPLayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
    LPlayer = m.Read<CPlayer>(LocalPLayer + 0x140);

    // ESP Loop
    for (int i = 0; i < 75; i++)
    {
        // Local Check
        if (pLocal->m_iHealth <= 0)
            continue;
        else if (pLocal->m_localOrigin == Vector3(0.f, 0.f, 0.f))
            continue;

        // Get Entity
        CPlayer BaseEntity, *pEntity = &BaseEntity;
        uint64_t Entity = GetEntityById(m.BaseAddress, i);
        
        // Entity pointer check
        if (Entity == NULL || Entity == LocalPLayer)
            continue;

        // sdk.hで作ったプレイヤーの構造体を読み取る
        BaseEntity = m.Read<CPlayer>(Entity + 0x140); // 0x140を起点とする

        // 読み取った値のチェックをし、ターゲットが生存しているかの確認をする
        if (!TeamESP && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;
        else if (pEntity->m_iTeamNum == 0) // ?
            continue;
        else if (pEntity->m_iHealth <= 0 || pEntity->m_iHealth == 0)
            continue;
        else if (pEntity->m_localOrigin == Vector3(0.f, 0.f, 0.f))
            continue;

        // 上のチェックを通ったターゲットのWorldToScreen
        uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
        uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
        Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);
        Vector2 ScreenPosition = {};
        WorldToScreen(pEntity->m_localOrigin, &ViewMatrix._11, (float)GameSize.right, (float)GameSize.bottom, ScreenPosition);

        // W2Sが有効であったらレンダリングする
        if (ScreenPosition != Vector2(0.f, 0.f))
        {   
            // distance
            float pDistance = ((pEntity->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);

            // ESP, Aimの範囲内にいるか？
            if (pDistance < MaxDistance)
            {
                // ターゲットの頭の座標を取得
                Vector3 HeadGamePosition = GetEntityBonePosition(Entity, 8, pEntity->m_localOrigin);
                Vector2 HeadScreenPosition = {};
                WorldToScreen(HeadGamePosition, &ViewMatrix._11, (float)GameSize.right, (float)GameSize.bottom, HeadScreenPosition);

                // Box用の値の計算
                float Height = abs(abs(HeadScreenPosition.y) - abs(ScreenPosition.y));
                float Width = Height / 1.85f;
                float BoxMiddle = ScreenPosition.x - (Width / 2.f);

                /* 
                    [ Visibility Check]
                  -> 敵が見えていると数値が増えていく。

                  前回読み取った値より大きい -> 見えている
                  前回読み取った値と変化なし -> 見えていない
                */
                bool visible = false;
                float LastVisibleTime = m.Read<float>(Entity + 0x1754); // m_lastVisibletime
                visible = LastVisibleTime > 0.f && LastVisibleTime > vPlayerInfo[i];
                vPlayerInfo[i] = LastVisibleTime;

                // Set ESP Color
                ImVec4 color = visible ? vColor_Visible : vColor_Normal;
                if (TeamESP && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
                    color = vColor_Team;

                // ESP Line
                if (ESP_Line)
                    DrawLine(ImVec2((float)GameSize.right / 2, (float)GameSize.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), color, 1);

                // ESP Box
                if (ESP_Box)
                {
                    switch (BoxType)
                    {
                    case 0:
                        // Normal 2D Box
                        DrawBox(color, BoxMiddle, HeadScreenPosition.y, Width, Height);
                        break;
                    case 1:
                        // Corner ESP Box
                        DrawLine(ImVec2((HeadScreenPosition.x + Width / 2), HeadScreenPosition.y), ImVec2((HeadScreenPosition.x + Width / 3), HeadScreenPosition.y), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x - Width / 2), HeadScreenPosition.y), ImVec2((HeadScreenPosition.x - Width / 3), HeadScreenPosition.y), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x + Width / 2), HeadScreenPosition.y), ImVec2((HeadScreenPosition.x + Width / 2), HeadScreenPosition.y + Height / 4), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x - Width / 2), HeadScreenPosition.y), ImVec2((HeadScreenPosition.x - Width / 2), HeadScreenPosition.y + Height / 4), color, 1);

                        DrawLine(ImVec2((HeadScreenPosition.x + Width / 2), ScreenPosition.y), ImVec2((HeadScreenPosition.x + Width / 2), ScreenPosition.y - (Height / 4)), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x - Width / 2), ScreenPosition.y), ImVec2((HeadScreenPosition.x - Width / 2), ScreenPosition.y - (Height / 4)), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x + Width / 2), ScreenPosition.y), ImVec2((HeadScreenPosition.x + Width / 3), ScreenPosition.y), color, 1);
                        DrawLine(ImVec2((HeadScreenPosition.x - Width / 2), ScreenPosition.y), ImVec2((HeadScreenPosition.x - Width / 3), ScreenPosition.y), color, 1);
                        break;
                    default:
                        break;
                    }
                }

                // Health Bar - 開発中、追加でBaseBarという後ろの黒いバーをテストで追加してうまくいったがずっっっっっっっとそのままです。HealthBarとの統合自体は簡単だよ
                if (ESP_HealthBar)
                {
                    BaseBar(HeadScreenPosition.x - (Width / 2.f) - 7, (HeadScreenPosition.y + Height) + 1, 4, -Height - 1);
                    ProgressBar((HeadScreenPosition.x - (Width / 2.f) - 6), (HeadScreenPosition.y + Height), 2, -Height, pEntity->m_iHealth + pEntity->m_shieldHealth, pEntity->m_iMaxHealth + pEntity->m_shieldHealthMax);
                }

                // Distance
                if (ESP_Distance)
                {
                    std::string esp_text = std::to_string((int)pDistance) + "m";

                    // Get text center position
                    ImVec2 textSize = ImGui::CalcTextSize(esp_text.c_str());
                    float TextCentor = textSize.x / 2.f;

                    String(ImVec2(ScreenPosition.x - TextCentor, ScreenPosition.y), ImColor(1.f, 1.f, 1.f, 1.f), esp_text.c_str());
                }

                // AimChecks
                if (AimBot && IsKeyDown(AimKey0) || IsKeyDown(AimKeu1))
                {
                    if (AimVisCheck && visible || !AimVisCheck)
                    {
                        // AimFOV内に敵がいるかをいくつかのBoneをチェックして判別する
                        for (int bone = 1; bone < 25; bone++)
                        {
                            // WorldToScreen
                            Vector3 BonePosition = GetEntityBonePosition(Entity, bone, pEntity->m_localOrigin);
                            Vector2 BoneScreenPosition = {};
                            WorldToScreen(BonePosition, &ViewMatrix._11, (float)GameSize.right, (float)GameSize.bottom, BoneScreenPosition);

                            // Fov Check
                            Vector2 ScreenMiddle = { (float)GameSize.right / 2, (float)GameSize.bottom / 2 };
                            float fov = abs((ScreenMiddle - BoneScreenPosition).Length());

                            if (fov < AimFov)
                            {
                                SetAimAngle(Entity, pEntity->m_localOrigin, pDistance);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    ImGui::End();
}

void Overlay::SetAimAngle(uint64_t Entity, Vector3 EntityPosition, float distance)
{
    uint64_t Local = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
    Vector3 LocalHead = m.Read<Vector3>(Local + offset::camera_origin);
    Vector3 TGTBonePosition = GetEntityBonePosition(Entity, TargetBone, EntityPosition);

    TGTBonePosition += GetPredict(Entity, distance, PredictValue);

    // Angle
    Vector3 AimAngle = CalcAngle(LocalHead, TGTBonePosition);
    Vector3 TempViewAngles = m.Read<Vector3>(Local + 0x2188);
    Vector3 SwayAngle = m.Read<Vector3>(Local + 0x2178);

    Vector3 Delta = AimAngle - TempViewAngles;

    if (AimNoSway)
    {
        Vector3 Breath = SwayAngle - TempViewAngles;

        if (Breath != Vector3(0.f, 0.f, 0.f))
            Delta = (AimAngle - TempViewAngles) - Breath;
    }

    NormalizeAngles(Delta);
    Vector3 SmoothedAngle = TempViewAngles + Delta / AimSmooth;
    NormalizeAngles(SmoothedAngle);

    if (SmoothedAngle != Vector3(0.f, 0.f, 0.f))
    {
        // Write Angle
        m.Write<Vector3>(Local + 0x2188, SmoothedAngle);
    }
}