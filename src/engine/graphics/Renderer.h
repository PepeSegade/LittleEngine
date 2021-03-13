#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <glm/glm.hpp>
#include "ProgramObject.h"
#include "ViewProj.h"

namespace LittleEngine 
{
	class Renderer 
	{
	private:
		glm::vec4 bgColor;
	public: 
		Renderer();

		Renderer* changeBackgroungColor					(glm::vec4 color);
		Renderer* startRenderConfig						();
		Renderer* clearBuffersFw						();
		Renderer* clearBuffersPP						();
		Renderer* resize								(int width, int height);
		Renderer* uploadTimeToProgram					(ProgramObject* program);
		Renderer* uploadResolutionToProgram				(ProgramObject* program);
		Renderer* uploadViewMatrixToProgram				(ProgramObject* program, glm::mat4 view);
		Renderer* uploadProjectionMatrixToProgram		(ProgramObject* program, glm::mat4 proj);
		Renderer* uploadViewProjectionMatricesToProgram	(ProgramObject* program, ViewProj* viewProj);
	};
}

#endif // !RENDERER_H_

