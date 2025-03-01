#pragma once


	class AbstractCommand
	{
	public:
		virtual void Execute() = 0;
		virtual void Undo() = 0;
		virtual ~AbstractCommand() {};
	};
