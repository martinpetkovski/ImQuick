#pragma once

#include "imgui.h"
#include<unordered_map>
#include<vector>
#include<functional>
#include<typeindex>
#include<string>

#define IMQ_DEFAULT_WND_NAME "Main Window"
#define IMQ_DEFAULT_WND_CAT "Windows"

#define IMQ_DYNAMIC_CAST(type, input_var, cast_var_name) IM_ASSERT(value); type* cast_var_name = dynamic_cast<type*>(input_var); IM_ASSERT(cast_var_name)
#define IMQ_STATIC_CAST(type, input_var, cast_var_name) IM_ASSERT(value); type* cast_var_name = static_cast<type*>(input_var); IM_ASSERT(cast_var_name)

namespace ImQuickRender
{
	void Render_TypeGeneric(const char* label, void* value)
	{
		IM_ASSERT(value);
	}

	void Render_TypeFloat(const char* label, void* value)
	{
		IMQ_STATIC_CAST(float, value, fValue);
		ImGui::InputFloat(label, fValue);
	}

	void Render_TypeBool(const char* label, void* value)
	{
		IMQ_STATIC_CAST(bool, value, bValue);
		ImGui::Checkbox(label, bValue);
	}

	void Render_TypeConstChar(const char* label, void* value)
	{
		IMQ_STATIC_CAST(char, value, cValue);
		ImGui::InputText(label, cValue, std::strlen(cValue));
	}

	void Render_TypeDouble(const char* label, void* value)
	{
		IMQ_STATIC_CAST(double, value, dValue);
		ImGui::InputDouble(label, dValue);
	}

	void Render_TypeInt(const char* label, void* value)
	{
		IMQ_STATIC_CAST(int, value, iValue);
		ImGui::InputInt(label, iValue);
	}
}

namespace ImQuick
{
	struct SImQuickWindow
	{
		std::string name = IMQ_DEFAULT_WND_NAME;
		bool isOpen = true;
		std::string category = IMQ_DEFAULT_WND_CAT;

		SImQuickWindow() { }
	};

	struct SImQuickProperty
	{
		std::string name;
		std::string label;
		std::type_index paramType = std::type_index(typeid(int));
		void* param = nullptr;
		SImQuickWindow* wnd;

		SImQuickProperty() { }
	};

	struct SImQuickContext
	{
		std::unordered_map<std::type_index, std::function<void(const char*, void*)>> mTypeRenderFunctionMap;
		std::vector<SImQuickProperty> mPropertyMap;
		std::vector<SImQuickWindow> mWindowsMap;
		std::vector<void*> mVariablePool;
		bool mIsRenderMainMenu = true;

		SImQuickContext() {}
	};

	static SImQuickContext* gImQuickContext;

	inline void CreateContext()
	{
		gImQuickContext = IM_NEW(SImQuickContext);
	}

	inline void DestroyContext()
	{
		IM_ASSERT(gImQuickContext);
		for (const auto& var : gImQuickContext->mVariablePool)
		{
			delete var;
		}

		IM_FREE(gImQuickContext);
	}


	template<typename T>
	void RegisterFunction(std::function<void(const char*, void*)> function)
	{
		IM_ASSERT(gImQuickContext);
		gImQuickContext->mTypeRenderFunctionMap.insert_or_assign(std::type_index(typeid(T)), function);
	}

	inline void Initialize()
	{
		ImQuick::InitializeWindow(IMQ_DEFAULT_WND_NAME, IMQ_DEFAULT_WND_CAT);

		RegisterFunction<float>(std::bind(&ImQuickRender::Render_TypeFloat, std::placeholders::_1, std::placeholders::_2));
		RegisterFunction<bool>(std::bind(&ImQuickRender::Render_TypeBool, std::placeholders::_1, std::placeholders::_2));
		RegisterFunction<char>(std::bind(&ImQuickRender::Render_TypeConstChar, std::placeholders::_1, std::placeholders::_2));
		RegisterFunction<double>(std::bind(&ImQuickRender::Render_TypeDouble, std::placeholders::_1, std::placeholders::_2));
		RegisterFunction<int>(std::bind(&ImQuickRender::Render_TypeInt, std::placeholders::_1, std::placeholders::_2));
	}


	inline SImQuickWindow* GetWindow(const std::string name)
	{
		IM_ASSERT(gImQuickContext);
		SImQuickWindow* retVal = nullptr;

		for (auto& wnd : gImQuickContext->mWindowsMap)
		{
			if (wnd.name == name)
			{
				retVal = &wnd;
			}
		}

		return retVal;
	}

	inline void InitializeWindow(const std::string& wndName, const std::string& catName = IMQ_DEFAULT_WND_CAT)
	{
		IM_ASSERT(gImQuickContext);

		const auto& currentWnd = GetWindow(wndName);
		if (currentWnd)
			return;

		SImQuickWindow wnd;
		wnd.name = wndName;
		wnd.isOpen = true;
		wnd.category = catName;

		gImQuickContext->mWindowsMap.push_back(wnd);
	}

	inline void SetWindowState(const std::string& wndName, bool isOpen)
	{
		IM_ASSERT(gImQuickContext);
		const auto& wndIt = GetWindow(wndName);
		IM_ASSERT(wndIt);
		wndIt->isOpen = isOpen;
	}

	inline void SetWindowCategory(const std::string& wndName, const std::string& category)
	{
		IM_ASSERT(gImQuickContext);
		const auto& wndIt = GetWindow(wndName);
		if (!wndIt)
		{
			InitializeWindow(wndName, category);
		}
		if (!wndIt)
		{
			wndIt->category = category;
		}
	}

	inline void SetRenderMainMenu(bool render)
	{
		IM_ASSERT(gImQuickContext);
		gImQuickContext->mIsRenderMainMenu = render;
	}

	template<typename T>
	void RegisterProperty(
		const std::string& name,
		T* param,
		const std::string& label = "",
		const std::string& wndName = IMQ_DEFAULT_WND_NAME)
	{
		IM_ASSERT(gImQuickContext);

		SImQuickProperty typeProperty;
		typeProperty.name = name;
		if (label == "")
			typeProperty.label = name;
		else
			typeProperty.label = label;

		typeProperty.paramType = std::type_index(typeid(*param));
		typeProperty.param = param;

		auto wndIt = GetWindow(wndName);
		IM_ASSERT(wndIt); // Initialize the window before assigning it to a property

		typeProperty.wnd = wndIt;
		gImQuickContext->mPropertyMap.push_back(typeProperty);
	}

	template<typename T>
	void RegisterProperty(
		const std::string& name,
		const std::string& label = "",
		const std::string& wndName = IMQ_DEFAULT_WND_NAME)
	{
		IM_ASSERT(gImQuickContext);

		T* param = new T();
		gImQuickContext->mVariablePool.push_back(param);

		RegisterProperty<T>(name, param, label, wndName);
	}

	inline void RemoveProperty(const std::string& name)
	{
		IM_ASSERT(gImQuickContext);

		auto propertyIt = gImQuickContext->mPropertyMap.begin();
		while (propertyIt != gImQuickContext->mPropertyMap.end())
		{
			if (propertyIt->name == name)
				break;
		}

		gImQuickContext->mPropertyMap.erase(propertyIt);
	}

	template<typename T>
	void UpdateProperty(
		const std::string& name,
		const std::string& newName,
		T* param,
		const std::string& wndName = IMQ_DEFAULT_WND_NAME)
	{
		RemoveProperty(name);
		RegisterProperty<T>(newName, param, wndName);
	}

	inline SImQuickProperty* GetProperty(const std::string& name)
	{
		IM_ASSERT(gImQuickContext);
		SImQuickProperty* retVal = nullptr;

		for (auto& propertyIt : gImQuickContext->mPropertyMap)
		{
			if (propertyIt.name == name)
				retVal = &propertyIt;
		}

		return retVal;
	}

	template<typename T>
	T* GetPropertyValue(const std::string& name)
	{
		IM_ASSERT(gImQuickContext);
		T* retVal = nullptr;
		const auto& paramIt = GetProperty(name);
		if (paramIt)
		{
			retVal = reinterpret_cast<T*>(paramIt->param);
		}

		return retVal;
	}

	template<typename T>
	T* SetPropertyValue(const std::string& name, const T& value)
	{
		const auto& param = GetPropertyValue<T>(name);
		IM_ASSERT(param)
		
		*param = T(value);
	}

	inline void Render()
	{
		IM_ASSERT(gImQuickContext);
		if (gImQuickContext->mIsRenderMainMenu)
		{
			if (ImGui::BeginMainMenuBar())
			{
				auto& windowsMap = gImQuickContext->mWindowsMap;
				for (auto& wndIt : windowsMap)
				{
					if (ImGui::BeginMenu(wndIt.category.c_str()))
					{
						if (ImGui::MenuItem(wndIt.name.c_str(), NULL, wndIt.isOpen))
						{
							wndIt.isOpen = !wndIt.isOpen;
						}
						ImGui::EndMenu();
					}
				}

				ImGui::EndMainMenuBar();
			}
		}

		auto& properties = gImQuickContext->mPropertyMap;
		for (auto& propertyIt : properties)
		{
			IM_ASSERT(propertyIt.wnd);
			if (!propertyIt.wnd->isOpen)
				continue;

			auto propRenderFuncIt = gImQuickContext->mTypeRenderFunctionMap.find(propertyIt.paramType);
			IM_ASSERT(propRenderFuncIt != gImQuickContext->mTypeRenderFunctionMap.end()); // unsupported type, render function not found
			const char* label = propertyIt.label.c_str();
			void* value = propertyIt.param;

			if (ImGui::Begin(propertyIt.wnd->name.c_str(), &propertyIt.wnd->isOpen))
			{
				propRenderFuncIt->second(label, value);
				ImGui::End();
			}
		}
	}
}