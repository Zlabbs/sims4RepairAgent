#pragma once

#include <iostream>
#include <string>
#include <thread>

namespace standardShared
{
	namespace con
	{
		enum PrintType
		{
			Failure, //somethings wong
			Warning,
			Completeion,
			Normal,
			Notice,
			InStageA,
			InStageB,
			InStageC,
			InStageD,
		};


		void PrintText(PrintType type, std::string textStart, std::string textMiddle, std::string textEnd)
		{
			switch (type)
			{
			case standardShared::con::PrintType::Failure:
				std::cout << "\033[0;91;49m" << textStart << "\033[0m" << textMiddle << textEnd << "\n";
				break;
			case standardShared::con::PrintType::Warning:
				std::cout << "\033[0;93;49m" << textStart << "\033[0m" << textMiddle << textEnd << "\n";
				break;
			case standardShared::con::PrintType::Completeion:
				std::cout << "\033[0;92;49m" << textStart << "\033[0m" << textMiddle << textEnd << "\n";
				break;
			case standardShared::con::PrintType::Normal:
				std::cout << "\033[0m" << textStart << textMiddle << textEnd << "\n";
				break;
			case standardShared::con::PrintType::Notice:
				std::cout << "\033[0;96;49m" << textStart << textMiddle << textEnd << "\n";
				break;
			case standardShared::con::PrintType::InStageA:
				std::cout << "\033[0;97;44m" << textStart << "\033[0m" << textMiddle << "\033[0;92;49m" << textEnd << "\n";
				break;
			case standardShared::con::PrintType::InStageB:
				std::cout << "\033[0;97;46m" << textStart << "\033[0m" << textMiddle << "\033[0;92;49m" << textEnd << "\n";
				break;
			case standardShared::con::PrintType::InStageC:
				std::cout << "\033[0;97;104m" << textStart << "\033[0m" << textMiddle << "\033[0;92;49m" << textEnd << "\n";
				break;
			case standardShared::con::PrintType::InStageD:
				std::cout << "\033[0;97;45m" << textStart << "\033[0m" << textMiddle << "\033[0;92;49m" << textEnd << "\n";
				break;
			default:
				break;
			}
		}
	}
}