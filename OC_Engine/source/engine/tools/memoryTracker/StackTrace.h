#pragma once

#include <functional>

namespace MemoryTracker
{
	struct StackTraceImpl;
	class StackTrace
	{
	public:
		StackTrace() : myImpl(nullptr) {}

		static StackTrace CaptureStackTrace(int aSkipDepth);
		void Print() const;
		std::size_t ComputeHash() const;
		bool operator==(const StackTrace& aStackTrace) const noexcept
		{
			return myImpl == aStackTrace.myImpl;
		}
	private:
		StackTrace(const StackTraceImpl&);
		const StackTraceImpl* myImpl;
	};
}

namespace std
{
	template<> struct hash<MemoryTracker::StackTrace>
	{
		std::size_t operator()(const MemoryTracker::StackTrace& aStackTrace) const noexcept
		{
			return aStackTrace.ComputeHash();
		}
	};
}
