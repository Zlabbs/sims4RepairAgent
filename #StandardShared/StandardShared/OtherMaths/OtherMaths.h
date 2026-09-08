#pragma once

namespace standardShared
{
	namespace otherMaths
	{
		float Remap(float value, float LowerBoundPoint, float HigherBoundPoint, float LowerBoundValue, float HigherBoundValue)
		{
			return LowerBoundValue + (((value - LowerBoundPoint) / (HigherBoundPoint - LowerBoundPoint)) * (HigherBoundValue - LowerBoundValue));
		}
	}
}