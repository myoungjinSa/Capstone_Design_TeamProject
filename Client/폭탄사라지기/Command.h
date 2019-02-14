#pragma once

class Command
{
public:
	virtual ~Command() {}

	virtual void Execute() = 0;
};

class JumpCommand : public Command
{

public:
	virtual void Execute() { jump(); }
};