/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' Game Engine Design coursework
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

	class MoveBackward : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveBackward();
		}
	};

	class MoveForward : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveForward();
		}
	};

	class MoveRight : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveRight();
		}
	};

	class MoveLeft : public Command
	{
	public:
		void Execute(std::unique_ptr<Player>& character) override
		{
			character->MoveLeft();
		}
	};

	class MouseMove : public Command
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
