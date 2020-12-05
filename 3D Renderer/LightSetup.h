#pragma once
#include "Diffuse_type.h"
#include "Light_Type.h"

struct Light
{
	virtual void setDiffuse(Diffuse_Type type) = 0;
	virtual void setAmbient(float ambient_const)
	{
		amb_constant = ambient_const;
	}
	virtual void setSpecular(float exponent, float intensity)
	{
		spec_exponent = exponent;
		spec_intensity = intensity;
	}

	double amb_constant;
	double spec_exponent;
	double spec_intensity;
	Diffuse_Type diffuse_type;
	Light_Type light_type;
};


struct PointLightSetup : Light
{
	PointLightSetup() {
		light_type = Light_Type::PointLight;
	}
	
	void setDiffuse(Diffuse_Type type) override
	{
		diffuse_type = type;
	}

	void setLightPos(Vector3f light_pos)
	{
		lightpos = light_pos;
	}

	void setAttenuation(double quad, double linear, double constant)
	{
		a = quad;
		b = linear;
		c = constant;
	}

	Vector3f lightpos;
	double a, b, c;		//a = quad, b = linear, c = costant
};

struct DirectionalLightSetup : Light
{
	DirectionalLightSetup() {
		light_type = Light_Type::DirLight;
	}

	void setDiffuse(Diffuse_Type type) override		
	{
		diffuse_type = type;
	}

	void setLightDir(Vector3f light_dir)
	{
		lightdir = light_dir;
	}

	Vector3f lightdir;
};