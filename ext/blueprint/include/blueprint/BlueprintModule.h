#pragma once

#include <moon/Module.h>

namespace bp
{

class BlueprintModule : public moon::Module
{
public:
	virtual moon::ModuleTypeID TypeID() const override {
		return moon::GetModuleTypeID<BlueprintModule>();
	}
	virtual const char* GetName() const override { 
		return "moon.bp"; 
	}

}; // BlueprintModule

}