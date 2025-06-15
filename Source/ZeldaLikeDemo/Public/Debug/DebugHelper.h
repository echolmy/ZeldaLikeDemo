#pragma once

namespace Debug
{
	static void PrintInfo(const FString& DebugMessage)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMessage);
		}
	}
}
