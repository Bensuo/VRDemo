/*
Name: Stuart D. Adams
Student ID: B00265262
I declare that I produced the following code for the GED module and that this is my own work.
I am aware of the penalties incurred by submitting in full or in part work that is not our own and that was developed by third parties that are not appropriately acknowledged.
*/
#include "ShaderFactory.h"
#include "Shader.h"

namespace Engine
{
	namespace Content
	{
        std::unordered_map<std::string, Rendering::Shader> ShaderFactory::resources;
	}
}
