/*
Name: Stuart D. Adams
Student ID: B00265262
I declare that the following code was produced by Stuart Adams for the GED module and that this is my own work.
I am aware of the penalties incurred by submitting in full or in part work that is not our own and that was developed by third parties that are not appropriately acknowledged.
*/
#pragma once
#include "Player.h"
#include <memory>

namespace Engine
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(std::unique_ptr<Player>& character) = 0;
	};

	class csMoveBackward : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveBackward();
		}
	};

	class csMoveForward : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveForward();
		}
	};

	class csMoveRight : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveRight();
		}
	};

	class csMoveLeft : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveLeft();
		}
	};

	class csMouseMove : public Command
	{
		glm::vec2 mouseMotion;
	public:
		void mouseMove(const glm::vec2& mouseMotion)
		{
			this->mouseMotion = mouseMotion;
		}
		void Execute(std::unique_ptr<Player>& character) override { character->ProcessMouseMovement(mouseMotion.x, mouseMotion.y, true); }
	};
}