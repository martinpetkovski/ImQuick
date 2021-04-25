# ImQuick
ImQuick is a single header ImGui extension library for clearer separation of interface from logic. ImQuick renders ImGui UI elements automagically based only on the variable type. 

ImQuick requires [ImGui](https://github.com/ocornut/imgui) and C++11.

# Workflow
### Outside the Render loop
1. Create windows **during** the initialization.
2. Create custom render functions for custom types (or skip this step if using basic C++ types).
3. Register variables to be rendered automagically by ImQuick.
### Inside the Render loop
- Simply let ImQuick handle the UI rendering based on the variable type.

# *ImQuick*start
### Outside the Render loop
1. Create the ImQuick context by calling `ImQuick::CreateContext()` right after calling `ImGui::CreateContext()`.
2. (Optional) Call `ImQuick::Initialize()` after creating the context to initialize the render functions for the basic C++ types.
3. Initialize all windows by calling `ImQuick::InitializeWindow("WINDOW NAME", "WINDOW CATEGORY")`
4. (Optional) You can register a custom rendering function by calling `ImQuick::RegisterFunction<TYPE>(std::bind<YourTYPERenderingFunction, std::placeholders::_1, std::placeholders::_2)`. You can register a rendering function of any type.
5. You can then register an existing property by calling `ImQuick::RegisterProperty<TYPE>("PROPERTY LABEL", &propertyVariableOfTYPE, "WINDOW NAME")`. Or you can create a new property using `ImQuick::RegisterProperty<TYPE>("PROPERTY LABEL", "WINDOW NAME")`. You can get any property value by calling `ImQuick::GetProperty<TYPE>("PROPERTY LABEL")`.
6. Call `ImQuick::DestroyContext()` before calling `ImGui::DestroyContext()`.
### Inside the Render loop
1. Call `ImQuick::Render()` **before** calling `ImGui::Render()`.

# Use case
Let's say you have a class named Character which you want to render inside the Main Window. The character class looks something like:
```
class CCharacter
{
  public:
    int age = 0;
    float stamina = 0.0f;
    double skill = 0.0;
}
```

First, you need to write a rendering function for the character class, either inside the class or in the global scope. It would look something like:
```
void Render_TypeCharacter(const char* label, void* value)
{
  IMQ_STATIC_CAST(CCharacter, value, character);
  ImGui::InputInt("Age", &character->age);
  ImGui::InputFloat("Stamina", &character->stamina);
  ImGui::InputDouble("Skill", &character->skill);
}
```

Then, you register the render function with ImQuick like so:
```
  ImQuick::RegisterFunction<CCharacter>(std::bind(&Render_TypeCharacter, std::placeholders::_1, std::placeholders::_2));

```

Finally, you register the character property you want rendered like so:
```
  CCharacter cJohn;
  ImQuick::RegisterProperty<CCharacter>("John", &cJohn, "John", "Main Window");
```

And your all set without even touching the render loop at all!

# License 
ImQuick is licensed under the MIT license, see [License.txt](https://github.com/martinpetkovski/ImQuick/blob/master/License.txt) for more information. 
