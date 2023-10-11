#pragma once

#include "core/InputHandler.h"
#include "core/FPSCamera.h"
#include "core/WindowManager.hpp"
#include "parametric_shapes.hpp"
#include "bonafide.hpp"

class Window;


namespace edaf80
{
	//! \brief Wrapper class for Assignment 5
	class Assignment5 {
	public:

		Assignment5(WindowManager& windowManager);
        FPSCameraf mCamera;
        
		~Assignment5();
		void run();

	private:
        WindowManager& mWindowManager;
        InputHandler inputHandler;
		GLFWwindow*    window;
        enum gameState {
            MENU = 0,
            GAME = 1,
            PAUSE = 2,
            EXIT = 3
        };
	};
}
