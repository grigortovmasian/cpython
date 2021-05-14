#pragma once

namespace aadc {

	struct MemInfo {
		size_t pageSize;		// Virtual memory page size.
		bool   cpu_AVX512;		// cpu supports AVX512
	};

	struct osutils {

		static const MemInfo& getMemInfo(); // noexcept; no support in VS2012

		static void* allocMem(size_t size);
		static void  protectMem(void* p, size_t size);
		static void  releaseMem(void* p, size_t size);

		//! Get the current CPU tick count, used for benchmarking (1ms resolution).
		static unsigned int getTickCount(); // noexcept;

		// path separator
		static std::string path_sep();
	};

}