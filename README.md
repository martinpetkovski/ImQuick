# ImQuick
Single header ImGui extension library for clearer separation of interface from logic.

# Workflow
### Outside the Render loop
1. Initialize all Windows.
2. Initialize ImQuick with custom render functions (or skip this step if using basic C++ types).
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
