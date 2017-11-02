#include "stdafx.h"
#include "SettingsScreen.hpp"
#include "Application.hpp"
#include <Shared/Profiling.hpp>
#include "GameConfig.hpp"
#include "Scoring.hpp"
#include <Audio/Audio.hpp>
#include "Track.hpp"
#include "Camera.hpp"
#include "Background.hpp"
#include <GUI/GUI.hpp>
#include <GUI/CommonGUIStyle.hpp>
#include <GUI/Button.hpp>
#include <GUI/Slider.hpp>
#include <GUI/ScrollBox.hpp>
#include <GUI/SettingsBar.hpp>
#include <GUI/Spinner.hpp>
#include "HealthGauge.hpp"
#include "Shared/Jobs.hpp"
#include "ScoreScreen.hpp"
#include "Shared/Enum.hpp"
#include "Input.hpp"


class SettingsScreen_Impl : public SettingsScreen
{
private:
	Ref<CommonGUIStyle> m_guiStyle;
	Ref<Canvas> m_canvas;
	Ref<SettingsBar> m_settings;

	Vector<String> m_speedMods = { "XMod", "MMod", "CMod" };
	Vector<String> m_laserModes = { "Keyboard", "Mouse", "Controller" };
	Vector<String> m_buttonModes = { "Keyboard", "Controller" };
	Vector<String> m_gamePads;

	int m_speedMod = 0;
	int m_laserMode = 0;
	int m_buttonMode = 0;
	int m_selectedGamepad = 0;
	float m_modSpeed = 400.f;
	float m_hispeed = 1.f;
	float m_laserSens = 1.0f;

	//TODO: Use argument instead of many functions if possible.
	void SetKey_BTA()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_BT0, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_BT0));
	}
	void SetKey_BTB()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_BT1, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_BT1));
	}
	void SetKey_BTC()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_BT2, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_BT2));
	}
	void SetKey_BTD()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_BT3, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_BT3));
	}
	void SetKey_FXL()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_FX0, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_FX0));
	}
	void SetKey_FXR()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_FX1, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_FX1));
	}
	void SetKey_ST()
	{
		if (m_buttonMode == 1)
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Controller_BTS, true, m_selectedGamepad));
		else
			g_application->AddTickable(ButtonBindingScreen::Create(GameConfigKeys::Key_BTS));
	}

	void Exit()
	{
		Map<String, InputDevice> inputModeMap = {
			{ "Keyboard", InputDevice::Keyboard },
			{ "Mouse", InputDevice::Mouse },
			{ "Controller", InputDevice::Controller },
		};

		g_gameConfig.SetEnum<Enum_InputDevice>(GameConfigKeys::ButtonInputDevice, inputModeMap[m_buttonModes[m_buttonMode]]);
		g_gameConfig.SetEnum<Enum_InputDevice>(GameConfigKeys::LaserInputDevice, inputModeMap[m_laserModes[m_laserMode]]);

		g_gameConfig.Set(GameConfigKeys::HiSpeed, m_hispeed);
		g_gameConfig.Set(GameConfigKeys::ModSpeed, m_modSpeed);
		g_gameConfig.Set(GameConfigKeys::Controller_DeviceID, m_selectedGamepad);

		switch (inputModeMap[m_laserModes[m_laserMode]])
		{
		case InputDevice::Controller:
			g_gameConfig.Set(GameConfigKeys::Controller_Sensitivity, m_laserSens);
			break;
		case InputDevice::Mouse:
			g_gameConfig.Set(GameConfigKeys::Mouse_Sensitivity, m_laserSens);
			break;
		case InputDevice::Keyboard:
		default:
			g_gameConfig.Set(GameConfigKeys::Key_Sensitivity, m_laserSens);
			break;
		}

		if (m_speedMod == 2)
		{
			g_gameConfig.Set(GameConfigKeys::UseCMod, true);
			g_gameConfig.Set(GameConfigKeys::UseMMod, false);
		}
		else if (m_speedMod == 1)
		{
			g_gameConfig.Set(GameConfigKeys::UseCMod, false);
			g_gameConfig.Set(GameConfigKeys::UseMMod, true);
		}
		else
		{
			g_gameConfig.Set(GameConfigKeys::UseCMod, false);
			g_gameConfig.Set(GameConfigKeys::UseMMod, false);
		}

		g_input.Init(*g_gameWindow);
		g_application->RemoveTickable(this);
	}

public:

	//TODO: Controller support and the rest of the options and better layout
	bool Init()
	{
		m_guiStyle = g_commonGUIStyle;
		m_canvas = Utility::MakeRef(new Canvas());
		m_gamePads = g_gameWindow->GetGamepadDeviceNames();

		if (g_gameConfig.GetBool(GameConfigKeys::UseCMod))
			m_speedMod = 2;
		else if (g_gameConfig.GetBool(GameConfigKeys::UseMMod))
			m_speedMod = 1;

		switch (g_gameConfig.GetEnum<Enum_InputDevice>(GameConfigKeys::ButtonInputDevice))
		{
		case InputDevice::Controller:
			m_buttonMode = 1;
			break;
		case InputDevice::Keyboard:
		default:
			m_buttonMode = 0;
			break;
		}

		switch (g_gameConfig.GetEnum<Enum_InputDevice>(GameConfigKeys::LaserInputDevice))
		{
		case InputDevice::Controller:
			m_laserMode = 2;
			m_laserSens = g_gameConfig.GetFloat(GameConfigKeys::Controller_Sensitivity);
			break;
		case InputDevice::Mouse:
			m_laserMode = 1;
			m_laserSens = g_gameConfig.GetFloat(GameConfigKeys::Mouse_Sensitivity);
			break;
		case InputDevice::Keyboard:
		default:
			m_laserMode = 0;
			m_laserSens = g_gameConfig.GetFloat(GameConfigKeys::Key_Sensitivity);
			break;
		}

		m_modSpeed = g_gameConfig.GetFloat(GameConfigKeys::ModSpeed);
		m_hispeed = g_gameConfig.GetFloat(GameConfigKeys::HiSpeed);
		m_selectedGamepad = g_gameConfig.GetInt(GameConfigKeys::Controller_DeviceID);

		//Options select
		ScrollBox* scroller = new ScrollBox(m_guiStyle);

		Canvas::Slot* slot = m_canvas->Add(scroller->MakeShared());

		slot->anchor = Anchor(0.1f, 0.f, 0.9f, 1.f);
		//slot->autoSizeX = true;
		//slot->alignment = Vector2(0.5, 0.0);
		
		LayoutBox* box = new LayoutBox();
		box->layoutDirection = LayoutBox::Vertical;
		scroller->SetContent(box->MakeShared());
		LayoutBox::Slot* btnSlot;
		// Start Button
		{
			LayoutBox* stBox = new LayoutBox();

			stBox->layoutDirection = LayoutBox::Horizontal;
			{
				Button* stBtn = new Button(m_guiStyle);
				stBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_ST);
				stBtn->SetText(L"Start");
				stBtn->SetFontSize(32);
				btnSlot = stBox->Add(stBtn->MakeShared());
				btnSlot->padding = Margin(2);
				btnSlot->fillX = true;
			}
			LayoutBox::Slot* stSlot = box->Add(stBox->MakeShared());
			stSlot->alignment = Vector2(0.5f, 0.f);
		}
		// BT Buttons
		{
			LayoutBox* btBox = new LayoutBox();
			btBox->layoutDirection = LayoutBox::Horizontal;

			{
				Button* btaBtn = new Button(m_guiStyle);
				btaBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_BTA);
				btaBtn->SetText(L"BT-A");
				btaBtn->SetFontSize(32);
				btnSlot = btBox->Add(btaBtn->MakeShared());
				btnSlot->padding = Margin(2);
				btnSlot->fillX = true;
			}
			{
				Button* btbBtn = new Button(m_guiStyle);
				btbBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_BTB);
				btbBtn->SetText(L"BT-B");
				btbBtn->SetFontSize(32);
				btnSlot = btBox->Add(btbBtn->MakeShared());
				btnSlot->padding = Margin(2);
				btnSlot->fillX = true;
			}
			{
				Button* btcBtn = new Button(m_guiStyle);
				btcBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_BTC);
				btcBtn->SetText(L"BT-C");
				btcBtn->SetFontSize(32);
				btnSlot = btBox->Add(btcBtn->MakeShared());
				btnSlot->padding = Margin(2);
				btnSlot->fillX = true;
			}
			{
				Button* btdBtn = new Button(m_guiStyle);
				btdBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_BTD);
				btdBtn->SetText(L"BT-D");
				btdBtn->SetFontSize(32);
				btnSlot = btBox->Add(btdBtn->MakeShared());
				btnSlot->padding = Margin(2);
				btnSlot->fillX = true;
			}
			LayoutBox::Slot* btSlot = box->Add(btBox->MakeShared());
			btSlot->alignment = Vector2(0.5f, 0.f);
		}
		// FX Buttons
		{
			LayoutBox* fxBox = new LayoutBox();

			fxBox->layoutDirection = LayoutBox::Horizontal;
			{
				Button* fxlBtn = new Button(m_guiStyle);
				fxlBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_FXL);
				fxlBtn->SetText(L"FX-L");
				fxlBtn->SetFontSize(32);
				btnSlot = fxBox->Add(fxlBtn->MakeShared());
				btnSlot->padding = Margin(20.f,2.f);
				btnSlot->fillX = true;
				btnSlot->alignment = Vector2(0.25f, 0.f);
			}
			{
				Button* fxrBtn = new Button(m_guiStyle);
				fxrBtn->OnPressed.Add(this, &SettingsScreen_Impl::SetKey_FXR);
				fxrBtn->SetText(L"FX-R");
				fxrBtn->SetFontSize(32);
				btnSlot = fxBox->Add(fxrBtn->MakeShared());
				btnSlot->padding = Margin(20.f, 2.f);
				btnSlot->alignment = Vector2(0.75f, 0.f);
				btnSlot->fillX = true;
			}
			LayoutBox::Slot* fxSlot = box->Add(fxBox->MakeShared());
			fxSlot->alignment = Vector2(0.5f, 0.f);
		}

		// Setting bar
		{
			SettingsBar* sb = new SettingsBar(m_guiStyle);
			
			m_settings = Ref<SettingsBar>(sb);

			sb->AddSetting(&m_buttonMode, m_buttonModes, m_buttonModes.size(), "Button Input Mode");
			sb->AddSetting(&m_laserMode, m_laserModes, m_laserModes.size(), "Laser Input Mode");
			sb->AddSetting(&m_speedMod, m_speedMods, m_speedMods.size(), "Speed mod");
			sb->AddSetting(&m_laserSens, 0.f, 20.0f, "Laser Sensitivity");
			sb->AddSetting(&m_hispeed, 0.25f, 10.0f, "HiSpeed");
			sb->AddSetting(&m_modSpeed, 50.0f, 1500.0f, "ModSpeed");
			if (m_gamePads.size() > 0)
			{
				sb->AddSetting(&m_selectedGamepad, m_gamePads, m_gamePads.size(), "Selected Controller");

			}
			LayoutBox::Slot* slot = box->Add(sb->MakeShared());
			slot->fillX = true;
		}

		Button* exitBtn = new Button(m_guiStyle);
		exitBtn->OnPressed.Add(this, &SettingsScreen_Impl::Exit);
		exitBtn->SetText(L"Back");
		exitBtn->SetFontSize(32);
		btnSlot = box->Add(exitBtn->MakeShared());
		btnSlot->padding = Margin(2);
		btnSlot->alignment = Vector2(0.5f, 0.f);

		return true;
	}



	virtual void OnSuspend()
	{
		g_rootCanvas->Remove(m_canvas.As<GUIElementBase>());
	}
	virtual void OnRestore()
	{
		Canvas::Slot* slot = g_rootCanvas->Add(m_canvas.As<GUIElementBase>());
		slot->anchor = Anchors::Full;
	}
};

SettingsScreen* SettingsScreen::Create()
{
	SettingsScreen_Impl* impl = new SettingsScreen_Impl();
	return impl;
}

class ButtonBindingScreen_Impl : public ButtonBindingScreen
{
private:
	Ref<CommonGUIStyle> m_guiStyle;
	Ref<Canvas> m_canvas;
	Ref<Gamepad> m_gamepad;
	GameConfigKeys m_key;
	bool m_isGamepad;
	int m_gamepadIndex;
	bool m_completed = false;

public:
	ButtonBindingScreen_Impl(GameConfigKeys key, bool gamepad, int controllerindex)
	{
		m_key = key;
		m_gamepadIndex = controllerindex;
		m_isGamepad = gamepad;
	}

	bool Init()
	{
		m_guiStyle = g_commonGUIStyle;
		m_canvas = Utility::MakeRef(new Canvas());

		//Prompt Text
		LayoutBox* box = new LayoutBox();
		Canvas::Slot* slot = m_canvas->Add(box->MakeShared());
		slot->anchor = Anchor(0.5f, 0.5f);
		slot->autoSizeX = true;
		slot->autoSizeY = true;
		slot->alignment = Vector2(0.5f, 0.5f);

		Label* titleLabel = new Label();
		titleLabel->SetText(L"Press Key");
		titleLabel->SetFontSize(100);
		box->Add(titleLabel->MakeShared());

		if (m_isGamepad)
		{
			titleLabel->SetText(L"Press Button");
			m_gamepad = g_gameWindow->OpenGamepad(m_gamepadIndex);
			m_gamepad->OnButtonPressed.Add(this, &ButtonBindingScreen_Impl::OnButtonPressed);
		}

		return true;
	}

	void Tick(float deltatime)
	{
		if (m_completed)
		{
			m_gamepad->OnButtonPressed.RemoveAll(this);
			m_gamepad.Release();

			g_application->RemoveTickable(this);
		}
	}

	void OnButtonPressed(uint8 key)
	{
		g_gameConfig.Set(m_key, key);
		m_completed = true;
	}

	virtual void OnKeyPressed(int32 key)
	{
		if (!m_isGamepad)
		{
			g_gameConfig.Set(m_key, key);
			g_application->RemoveTickable(this);
		}
	}

	virtual void OnSuspend()
	{
		g_rootCanvas->Remove(m_canvas.As<GUIElementBase>());
	}
	virtual void OnRestore()
	{
		Canvas::Slot* slot = g_rootCanvas->Add(m_canvas.As<GUIElementBase>());
		slot->anchor = Anchors::Full;
	}
};

ButtonBindingScreen* ButtonBindingScreen::Create(GameConfigKeys key, bool gamepad, int controllerIndex)
{
	ButtonBindingScreen_Impl* impl = new ButtonBindingScreen_Impl(key, gamepad, controllerIndex);
	return impl;
}